# MDR wire fixtures

This directory contains curated, sanitized MDR response frames used by
`mdr_tests`.

Each `.bin` file is one complete packed wire frame, including the start/end
markers, escaped bytes, data type, sequence number, payload length, and
checksum. Keep the filename produced by the recorder:

```text
mdr-packet-<timestamp>-<counter>-rx.type-<type-name>-<hex>.seq-<seq>.cmd-<command-name>-<hex>.bin
```

Only `-rx.` data frames are dynamically replayed. TX and ACK captures may be
kept for framing tests, but the fixture runner does not dispatch them as
headphone responses.

## Capturing

Packet recording is implemented by the client through libmdr's synchronous
raw-frame callback and remains disabled by default. Pass an output directory
when starting the client:

```powershell
.\SonyHeadphonesClient.exe "D:\captures\sony"
```

Omitting the directory launches the client normally without recording.
Providing it is explicit because captures may contain private device data.
At startup, the client deletes only `mdr-packet-*.bin` files from the selected
directory; unrelated contents are preserved. Closing the client disables
recording; no persistent setting is stored.

The inbound frame is closed on disk before dynamic dispatch begins. A frame
that subsequently triggers `MDR_TRAP` is therefore still available.

## Curating

Captured frames can contain Bluetooth addresses, paired-device names,
playback metadata, and unique device identifiers. Before copying a capture
here:

1. Confirm it reproduces the issue with `mdr_tests`.
2. Replace private values while preserving all wire lengths.
3. Recompute framing and checksum with `MDRPackCommand` if payload bytes were
   changed.
4. Keep only the minimum frame needed for the regression.

Run the suite with:

```sh
ctest --test-dir out/build --output-on-failure
```
