package com.mos9527.SonyHeadphonesClient;

final class NativeClient {
    static final class TransportSnapshot {
        final long generation;
        final int action;
        final boolean refreshDevices;
        final String address;
        final String uuid;
        final byte[] data;

        TransportSnapshot(long generation, int action, boolean refreshDevices,
                          String address, String uuid, byte[] data) {
            this.generation = generation;
            this.action = action;
            this.refreshDevices = refreshDevices;
            this.address = address;
            this.uuid = uuid;
            this.data = data;
        }
    }

    static native void setScale(float scale);
    static native TransportSnapshot pollTransport(boolean takeTx);
    static native void transportConnected(long generation);
    static native void transportFailed(long generation, String error);
    static native boolean feedRx(long generation, byte[] data, int size);
    static native void setDevices(String[] names, String[] addresses);
    static native void stopTransport();
    static native void shareCompleted(long id, boolean dispatched, String error);

    private NativeClient() {}
}
