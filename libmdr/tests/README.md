# libmdr tests

`mdr_tests` is a dependency-free test executable registered with CTest. It
covers:

- MDR command framing, escaping, metadata, and checksum round trips;
- representative trivial and dynamic V1/V2 serialization round trips;
- fragmented and multiple-frame receive buffering;
- V2 Table-1/Table-2 routing through a fake `MDRConnection`;
- recursive replay of curated inbound wire captures in `fixtures/`.

V1 currently has generated serialization but no runtime headphones
dispatcher, so its fixtures are tested through explicit deserializers rather
than `MDRHeadphones::PollEvents`.

The fake connection accepts generated ACKs and feeds transport chunks through
the public polling API. Tests therefore exercise the same receive, unpack,
dispatch, and state-update path as a real connection.
