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
> metadata, unique identifiers, and other private information. 

On-device packet captures for tests are generally welcome.

### Capture a session

Simply by running the Client with an folder argument enables packet recording.

You may also create a folder and drag-n-drop to the executable icon to do the same.

```powershell
.\SonyHeadphonesClient.exe capture-folder
```

The client creates the directory if needed and deletes existing
`mdr-packet-*.bin` files in it at startup.

### Submitting the data

- Create an immediate subdirectory in the source tree named `<model>-<firmware version>` under
   `tests/`, for example `tests/WF-1000XM5-6.1.0/`
- You can find the FW version to your device under the **About** tab
- Testing locally is encouraged though not necessary, as successful application runs imply validated packets.
- To validate captures locally, build the aggregate `mdr_tests` target and run
  `mdr_replay_tests <packet-directory>`.

- Commit and make your PR. Do note that only `rx` packets would be uploaded for validation.
