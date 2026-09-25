# MDR Packet Replay Tests

The `mdr_replay_tests` executable replays the `-rx.` `.bin` packets in one
directory, and checks on malformed/incompatible payloads.

The separate, pure-C `mdr_abi_tests` executable checks the public ABI,
state staging, operation serialization, event delivery, and protocol
bootstrap behavior through the shared libmdr test library.

## Usage

```sh
mdr_replay_tests <packet-directory>
```

Each immediate subdirectory of `tests/` is registered as a separate CTest
test named after that directory. Packet lookup inside those directories is
non-recursive.

## Capturing & Contributing

> [!WARNING]
> Packet captures may contain Bluetooth addresses, device names, media
> metadata, unique identifiers, and other private information. Run
> `tooling/scrub-capture.py` over a capture before committing it, and check
> what is left - see [Scrub personal data](#scrub-personal-data).

On-device packet captures for tests are generally welcome.

### Capture a session

```powershell
.\SonyHeadphonesClient.exe --record capture-folder
```

The client creates the directory if needed and deletes existing
`mdr-packet-*.bin` files in it at startup.

### Inspect a recorded session

When the client includes the protocol debugger (a Debug build, or a build configured
with `-DMDR_CLIENT_DEBUGGER=ON`), replay a capture without connecting to headphones:

```powershell
.\SonyHeadphonesClient.exe --replay capture-folder
```

A single `.bin` packet can be supplied instead of the folder, and either path can
also be dropped onto a running client window. Replay mode loads both TX and RX
packet files in filename order and exposes only the debugger interface. Use
**Quit replay** to return to the device-selection panel.

The debugger's export actions open native save dialogs on desktop platforms with
`.bin` and `.zip` default filenames. **Export ZIP** stores the complete packet
history without compression. Emscripten starts a browser download directly; other
platforms without dialog support use the default filename in the current directory.
The disconnect error dialog can export either the latest packet or the complete ZIP
for a bug report; packet captures may contain private device information.

### Scrub personal data

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

### Submitting the data

- Create an immediate subdirectory in the source tree named `<model>-<firmware version>` under
   `tests/`, for example `tests/WF-1000XM5-6.1.0/`
- You can find the FW version to your device under the **About** tab
- Testing locally is encouraged though not necessary, as successful application runs imply validated packets.
- To validate captures locally, build the aggregate `mdr_tests` target and run
  `mdr_replay_tests <packet-directory>`.

- Commit and make your PR. Do note that only `rx` packets would be uploaded for validation.
