package com.mos9527.SonyHeadphonesClient;

import android.content.ClipData;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;

import androidx.core.content.FileProvider;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;

final class FileShare {
    private static final long RETENTION_MS = 7L * 24 * 60 * 60 * 1000;
    private static boolean cacheCleaned;
    private final WeakReference<MainActivity> activity;
    private final File root;
    private final String authority;
    private final Handler main = new Handler(Looper.getMainLooper());
    private final ExecutorService writer = Executors.newSingleThreadExecutor();
    private volatile boolean closed;

    FileShare(MainActivity activity) {
        this.activity = new WeakReference<>(activity);
        root = new File(activity.getCacheDir(), "exports");
        authority = activity.getPackageName() + ".fileprovider";
    }

    void share(long id, String filename, byte[] data, String mimeType) {
        try {
            writer.execute(() -> prepare(id, filename, data, mimeType));
        } catch (RejectedExecutionException error) {
            NativeClient.shareCompleted(id, false, "File sharing is unavailable");
        }
    }

    private void prepare(long id, String filename, byte[] data, String mimeType) {
        File directory = new File(root, "export-" + UUID.randomUUID());
        try {
            if (closed) {
                throw new IOException("Activity was destroyed");
            }
            if (filename.isEmpty() || filename.equals(".") || filename.equals("..") ||
                filename.indexOf('/') >= 0 || filename.indexOf('\\') >= 0 || data.length == 0) {
                throw new IOException("Invalid export filename or data");
            }
            if (!root.isDirectory() && !root.mkdirs()) {
                throw new IOException("Unable to create export cache");
            }
            cleanCache(root);
            if (!directory.mkdir()) {
                throw new IOException("Unable to create export directory");
            }
            File file = new File(directory, filename);
            try (FileOutputStream output = new FileOutputStream(file)) {
                output.write(data);
            }
            if (!main.post(() -> present(id, file, mimeType))) {
                throw new IOException("Unable to schedule file sharing");
            }
        } catch (IOException | RuntimeException error) {
            removeDirectory(directory);
            NativeClient.shareCompleted(id, false, error.toString());
        }
    }

    private void present(long id, File file, String mimeType) {
        MainActivity host = activity.get();
        try {
            if (closed || host == null || host.isFinishing() || host.isDestroyed()) {
                throw new IllegalStateException("Activity was destroyed");
            }
            Uri uri = FileProvider.getUriForFile(host, authority, file);
            Intent send = new Intent(Intent.ACTION_SEND);
            send.setType(mimeType);
            send.putExtra(Intent.EXTRA_STREAM, uri);
            send.setClipData(ClipData.newRawUri(file.getName(), uri));
            send.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            host.startActivity(Intent.createChooser(send, null));
        } catch (RuntimeException error) {
            removeDirectory(file.getParentFile());
            NativeClient.shareCompleted(id, false, error.toString());
            return;
        }
        NativeClient.shareCompleted(id, true, null);
    }

    void close() {
        closed = true;
        writer.shutdown();
    }

    private static synchronized void cleanCache(File root) {
        if (cacheCleaned) {
            return;
        }
        cacheCleaned = true;
        File[] directories = root.listFiles();
        if (directories == null) {
            return;
        }
        long cutoff = System.currentTimeMillis() - RETENTION_MS;
        for (File directory : directories) {
            if (directory.isDirectory() && directory.getName().startsWith("export-") &&
                directory.lastModified() < cutoff) {
                removeDirectory(directory);
            }
        }
    }

    private static void removeDirectory(File directory) {
        if (directory == null) {
            return;
        }
        File[] files = directory.listFiles();
        if (files != null) {
            for (File file : files) {
                if (file.isFile()) {
                    file.delete();
                }
            }
        }
        directory.delete();
    }
}
