# Packet Capture and Test Contribution Guide

On-device packet captures for tests are always welcome. This guide also provides instructions for debugging malformed packets, which usually implies a bug in our protocol implementation (and you should report it too!).

> [!WARNING]
> Packet captures may contain Bluetooth addresses, device names, media
> metadata, unique identifiers, and other private information. Run
> `tooling/scrub-capture.py` over a capture before committing it, and check
> what is left - see [Scrub personal data](#scrub-personal-data).


## Capture a session

When the device is disconnected with an error, you will be *prompted* to dump either the last packet or your entire session to a folder, regardless of the `--record` option.

You can manually trigger this by selecting **Trigger disconnect error** in the top-left drop-down menu.

Furthermore, the Client always includes the functionality to record a session. Run the client with the `--record` option:

```powershell
.\SonyHeadphonesClient.exe --record capture-folder
```

**NOTE:** Existing captures within the same folder are always overwritten.

## Replay Test Usage

```sh
mdr_replay_tests <packet-directory>
```

The `mdr_replay_tests`, once built, replays the entire TX-RX packet history from the specified directory, and returns non-zero codes should any errors occur.

Within the source tree, each immediate subdirectory of `tests/` is registered as a separate CTest test named after it. See existing tests for details.

## With The Protocol Debugger

When the client includes the protocol debugger (a Debug build, or a build configured with `-DMDR_CLIENT_DEBUGGER=ON`), you can replay a capture without connecting to headphones.

```powershell
.\SonyHeadphonesClient.exe --replay <capture-folder/packet-bin-file>
```

Also, at any time, you can drag-and-drop the folder/bin file to the Client's viewport to replay the captures immediately. This works for both the Desktop and the Web client.

## Scrub personal data

A capture carries more than the protocol exchange. `PERI_*_PARAM` holds the
paired device list - the names and addresses of every phone, laptop and car kit
the headphones know about - and `PLAY_*_PARAM` holds whatever was playing.

```sh
tooling/scrub-capture.py --dry-run <capture-folder>   # report what would change
tooling/scrub-capture.py <capture-folder>             # rewrite in place
```

Placeholders are the same length as what they replace, so every length prefix
and each frame's size field still hold and only the checksum changes. The
address the device reports for itself is kept, since it identifies the captured
hardware; pass `--scrub-device-address` to drop that too.

Check the result before committing:

```sh
cat <capture-folder>/*.bin | strings -n 4
```

## Submitting the data

- Create an immediate subdirectory in the source tree named `<model>-<firmware version>` under
   `tests/`, for example `tests/WF-1000XM5-6.1.0/`
- You can find the FW version to your device under the **About** tab
- Testing locally is encouraged though not necessary, as successful application runs imply validated packets.
- To validate captures locally, build the aggregate `mdr_tests` target and run
  `mdr_replay_tests <packet-directory>`.

- Commit and make your PR. Do note that only `rx` packets would be uploaded for validation.
