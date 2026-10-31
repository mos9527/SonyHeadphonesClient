package com.mos9527.SonyHeadphonesClient;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.widget.Toast;

import org.libsdl.app.SDLActivity;

public final class MainActivity extends SDLActivity {
    private static final int BLUETOOTH_PERMISSION_REQUEST = 1;
    private ClientTransport transport;
    private volatile FileShare fileShare;
    private boolean resumed;
    private boolean permissionRequested;
    private boolean enableRequested;

    @Override
    protected String[] getLibraries() {
        return new String[] { "c++_shared", "SDL3", "main" };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (mBrokenLibraries || isFinishing()) {
            return;
        }
        fileShare = new FileShare(this);
        NativeClient.setScale(getResources().getDisplayMetrics().density);
        transport = new ClientTransport(getApplicationContext(), message ->
            runOnUiThread(() -> {
                if (!isFinishing() && !isDestroyed() && resumed) {
                    Toast.makeText(this, message, Toast.LENGTH_LONG).show();
                }
            }));
    }

    @Override
    protected void onResume() {
        super.onResume();
        resumed = true;
        startTransport();
    }

    private void startTransport() {
        if (!resumed || transport == null) {
            return;
        }
        NativeClient.setScale(getResources().getDisplayMetrics().density);
        if (Build.VERSION.SDK_INT >= 31 &&
            checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            if (!permissionRequested) {
                permissionRequested = true;
                requestPermissions(new String[] { Manifest.permission.BLUETOOTH_CONNECT },
                                   BLUETOOTH_PERMISSION_REQUEST);
            }
            return;
        }
        BluetoothManager manager = getSystemService(BluetoothManager.class);
        BluetoothAdapter adapter = manager == null ? null : manager.getAdapter();
        if (adapter != null && !adapter.isEnabled() && !enableRequested) {
            enableRequested = true;
            startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 2);
            return;
        }
        transport.start();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == BLUETOOTH_PERMISSION_REQUEST) {
            if (grantResults.length == 1 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                startTransport();
            } else {
                Toast.makeText(this, "Bluetooth permission is required. Enable Nearby devices in app settings.",
                               Toast.LENGTH_LONG).show();
            }
        }
    }

    @Override
    protected void onPause() {
        resumed = false;
        super.onPause();
    }

    @Override
    public void onConfigurationChanged(Configuration configuration) {
        super.onConfigurationChanged(configuration);
        if (!mBrokenLibraries && transport != null) {
            NativeClient.setScale(getResources().getDisplayMetrics().density);
        }
    }

    public void shareBlob(long id, String filename, byte[] data, String mimeType) {
        FileShare share = fileShare;
        if (share == null) {
            NativeClient.shareCompleted(id, false, "File sharing is unavailable");
        } else {
            share.share(id, filename, data, mimeType);
        }
    }

    @Override
    protected void onDestroy() {
        FileShare share = fileShare;
        fileShare = null;
        if (share != null) {
            share.close();
        }
        if (transport != null) {
            transport.stop();
            transport = null;
        }
        super.onDestroy();
    }
}
