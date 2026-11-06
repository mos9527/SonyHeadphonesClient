package com.mos9527.SonyHeadphonesClient;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

final class ClientTransport {
    private static final String TAG = "SHCBluetooth";
    private final Context context;
    private final BluetoothAdapter adapter;
    private final Consumer<String> onStatus;
    private ScheduledExecutorService timer;
    private Session session;
    private boolean active;
    private boolean writing;

    ClientTransport(Context context, Consumer<String> onStatus) {
        this.context = context;
        this.onStatus = onStatus;
        BluetoothManager manager = context.getSystemService(BluetoothManager.class);
        adapter = manager == null ? null : manager.getAdapter();
    }

    synchronized void start() {
        if (active) {
            return;
        }
        active = true;
        timer = Executors.newSingleThreadScheduledExecutor();
        timer.execute(() -> {
            synchronized (ClientTransport.this) {
                if (active) {
                    refreshDevices();
                }
            }
        });
        timer.scheduleWithFixedDelay(this::tick, 0, 16, TimeUnit.MILLISECONDS);
    }

    synchronized void stop() {
        active = false;
        if (timer != null) {
            timer.shutdownNow();
            timer = null;
        }
        NativeClient.stopTransport();
        closeSession();
    }

    private void checkBluetooth() {
        if (Build.VERSION.SDK_INT >= 31 &&
            context.checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException("Bluetooth permission not granted");
        }
        if (adapter == null || !adapter.isEnabled()) {
            throw new IllegalStateException("Turn on Bluetooth and pair your headphones in system settings.");
        }
    }

    private void refreshDevices() {
        List<String> names = new ArrayList<>();
        List<String> addresses = new ArrayList<>();
        try {
            checkBluetooth();
            List<BluetoothDevice> devices = new ArrayList<>(adapter.getBondedDevices());
            devices.sort(Comparator.comparing(BluetoothDevice::getAddress));
            for (BluetoothDevice device : devices.subList(0, Math.min(devices.size(), 512))) {
                String address = device.getAddress();
                String name = device.getName();
                names.add(name == null || name.isEmpty() ? address : name);
                addresses.add(address);
            }
            Log.i(TAG, addresses.size() + " paired devices · Classic RFCOMM");
        } catch (RuntimeException error) {
            names.clear();
            addresses.clear();
            report(error);
        }
        NativeClient.setDevices(names.toArray(new String[0]), addresses.toArray(new String[0]));
    }

    private synchronized void tick() {
        if (!active) {
            return;
        }
        try {
            NativeClient.TransportSnapshot snapshot = NativeClient.pollTransport(!writing);
            if (snapshot.refreshDevices) {
                refreshDevices();
            }
            if (snapshot.action != 0) {
                closeSession();
                if (snapshot.action == 1) {
                    checkBluetooth();
                    session = new Session(snapshot);
                    session.open();
                }
                return;
            }
            if (snapshot.data.length != 0) {
                if (session == null || session.generation != snapshot.generation) {
                    NativeClient.transportFailed(snapshot.generation, "RFCOMM transport unavailable");
                    return;
                }
                writing = true;
                session.write(snapshot.data);
            }
        } catch (RuntimeException error) {
            if (session != null) {
                NativeClient.transportFailed(session.generation, error.toString());
            } else {
                NativeClient.stopTransport();
            }
            closeSession();
            report(error);
        }
    }

    private void closeSession() {
        Session previous = session;
        session = null;
        writing = false;
        if (previous != null) {
            previous.close();
        }
    }

    private void report(Exception error) {
        Log.e(TAG, error.toString());
        onStatus.accept(error.getMessage() == null ? error.toString() : error.getMessage());
    }

    private final class Session {
        final long generation;
        final String address;
        final UUID uuid;
        final ExecutorService writer = Executors.newSingleThreadExecutor();
        BluetoothSocket socket;

        Session(NativeClient.TransportSnapshot snapshot) {
            generation = snapshot.generation;
            address = snapshot.address;
            uuid = UUID.fromString(snapshot.uuid);
        }

        boolean current() {
            return active && session == this;
        }

        void open() {
            new Thread(() -> {
                BluetoothSocket candidate = null;
                try {
                    candidate = adapter.getRemoteDevice(address).createRfcommSocketToServiceRecord(uuid);
                    synchronized (ClientTransport.this) {
                        if (!current()) {
                            return;
                        }
                        socket = candidate;
                    }
                    candidate.connect();
                    synchronized (ClientTransport.this) {
                        if (!current()) {
                            return;
                        }
                        NativeClient.transportConnected(generation);
                    }
                    InputStream input = candidate.getInputStream();
                    byte[] buffer = new byte[4096];
                    while (true) {
                        int count = input.read(buffer);
                        if (count < 0) {
                            throw new IOException("RFCOMM disconnected by remote device");
                        }
                        synchronized (ClientTransport.this) {
                            if (!current()) {
                                return;
                            }
                            if (!NativeClient.feedRx(generation, buffer, count)) {
                                throw new IOException("Android receive queue overflow");
                            }
                        }
                    }
                } catch (IOException | RuntimeException error) {
                    failed(error);
                } finally {
                    closeSocket(candidate);
                }
            }, "SHC-RFCOMM-Reader").start();
        }

        void write(byte[] data) {
            writer.execute(() -> {
                try {
                    BluetoothSocket currentSocket;
                    synchronized (ClientTransport.this) {
                        if (!current()) {
                            return;
                        }
                        currentSocket = socket;
                    }
                    if (currentSocket == null) {
                        throw new IOException("RFCOMM socket unavailable");
                    }
                    currentSocket.getOutputStream().write(data);
                } catch (IOException | RuntimeException error) {
                    failed(error);
                } finally {
                    synchronized (ClientTransport.this) {
                        if (current()) {
                            writing = false;
                        }
                    }
                }
            });
        }

        void failed(Exception error) {
            synchronized (ClientTransport.this) {
                if (current()) {
                    NativeClient.transportFailed(generation, error.toString());
                    closeSession();
                    report(error);
                }
            }
        }

        void close() {
            writer.shutdownNow();
            closeSocket(socket);
            socket = null;
        }

        void closeSocket(BluetoothSocket value) {
            if (value != null) {
                try {
                    value.close();
                } catch (IOException error) {
                    Log.w(TAG, "Unable to close RFCOMM socket", error);
                }
            }
        }
    }
}
