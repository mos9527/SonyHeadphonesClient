#!/usr/bin/env python3
"""Replace personal data in MDR packet captures with placeholders.

A capture carries more than the protocol exchange. `PERI_*_PARAM` holds the paired
device list - the names and Bluetooth addresses of the phones, laptops and car kits
the headphones know about - and `PLAY_*_PARAM` holds the track, album and artist
that were playing. Run this over a capture directory before committing it.

Placeholders are the same length as what they replace, so every length prefix and
each frame's size field still hold; only the checksum and the escaping change. The
address the device reports for itself in `CONNECT_RET_CAPABILITY_INFO` is kept, as
it identifies the captured hardware - pass --scrub-device-address to drop it too.

    tooling/scrub-capture.py --dry-run tests/WF-1000XM5-6.1.0
    tooling/scrub-capture.py tests/WF-1000XM5-6.1.0

Run it before `git add`, and check the result with
`cat <dir>/*.bin | strings -n 4`.
"""

import argparse
import glob
import os
import re
import sys

START, END, ESCAPE = 0x3E, 0x3C, 0x3D

# Data types, from mdr::MDRDataType. Table 2 renumbers the commands, so which table
# a frame belongs to decides how its command byte reads.
TABLE1_TYPES = {0x0C, 0x1C}  # DATA_MDR, SHOT_MDR
TABLE2_TYPES = {0x0E, 0x1E}  # DATA_MDR_NO2, SHOT_MDR_NO2

CONNECT_RET_CAPABILITY_INFO = 0x03
PLAY_PARAM_COMMANDS = {0xA7, 0xA9}  # PLAY_RET_PARAM, PLAY_NTFY_PARAM
PERIPHERAL_PARAM_COMMANDS = {0x37, 0x39}  # PERI_RET_PARAM, PERI_NTFY_PARAM

ADDRESS = re.compile(rb"[0-9A-F]{2}(?::[0-9A-F]{2}){5}")
TRACK_FIELDS = (b"Example Track", b"Example Album", b"Example Artist")


class FrameError(Exception):
    """A file that does not hold one well-formed frame."""


def unescape(data):
    out, index = bytearray(), 0
    while index < len(data):
        if data[index] == ESCAPE:
            if index + 1 >= len(data):
                raise FrameError("escape at end of frame")
            out.append(data[index + 1] + 0x10)
            index += 2
        else:
            out.append(data[index])
            index += 1
    return bytes(out)


def escape(data):
    out = bytearray()
    for byte in data:
        if byte in (START, END, ESCAPE):
            out += bytes([ESCAPE, byte - 0x10])
        else:
            out.append(byte)
    return bytes(out)


def unpack(frame):
    if len(frame) < 3 or frame[0] != START or frame[-1] != END:
        raise FrameError("missing start/end marker")
    body = unescape(frame[1:-1])
    if len(body) < 7:
        raise FrameError("frame too short")
    size = int.from_bytes(body[2:6], "big")
    payload, checksum = body[6:-1], body[-1]
    if len(payload) != size:
        raise FrameError(f"size field says {size}, payload is {len(payload)}")
    if checksum != (sum(body[:-1]) & 0xFF):
        raise FrameError("checksum mismatch")
    return body[0], body[1], payload


def pack(data_type, sequence, payload):
    body = bytes([data_type, sequence]) + len(payload).to_bytes(4, "big") + payload
    return bytes([START]) + escape(body + bytes([sum(body) & 0xFF])) + bytes([END])


def fit(text, length):
    """`text` trimmed or padded with '-' to exactly `length` bytes."""
    return text[:length] if len(text) >= length else text + b"-" * (length - len(text))


def is_text(data):
    return len(data) > 0 and all(0x20 <= byte < 0x7F for byte in data)


class Scrubber:
    def __init__(self, keep_device_address=True):
        self.keep_device_address = keep_device_address
        self.addresses = {}

    def address(self, value):
        """A stable placeholder per distinct address, so records stay distinguishable."""
        self.addresses.setdefault(value, len(self.addresses) + 1)
        return b"AA:BB:CC:00:00:%02X" % (self.addresses[value] & 0xFF)

    def sweep_addresses(self, payload):
        return ADDRESS.sub(lambda m: self.address(m.group(0)), payload)

    def track_info(self, payload):
        """PLAY_*_PARAM track info: repeated <charset><length><text>."""
        if len(payload) < 2 or payload[1] != 0x01:
            return payload
        out, index, field = bytearray(payload[:2]), 2, 0
        while index + 1 < len(payload):
            charset, length = payload[index], payload[index + 1]
            text = payload[index + 2:index + 2 + length]
            if length == 0 or len(text) != length or not is_text(text):
                break
            out += bytes([charset, length]) + fit(TRACK_FIELDS[field % len(TRACK_FIELDS)], length)
            index += 2 + length
            field += 1
        return bytes(out) + payload[index:]

    def peripheral_list(self, payload):
        """PERI_*_PARAM device list: <address:17><4 bytes><namelength><name> per record."""
        if len(payload) < 3 or payload[1] != 0x02:
            return payload
        out, index, record = bytearray(payload[:3]), 3, 0
        while index + 22 <= len(payload):
            address = payload[index:index + 17]
            meta, length = payload[index + 17:index + 21], payload[index + 21]
            name = payload[index + 22:index + 22 + length]
            if not ADDRESS.fullmatch(address) or len(name) != length:
                break
            record += 1
            # A binary name holds nothing personal, and one of them is what makes a
            # capture exercise the escaping path. Leave those as they are.
            if is_text(name):
                name = fit(b"Paired Device %02d" % record, length)
            out += self.address(address) + meta + bytes([length]) + name
            index += 22 + length
        return bytes(out) + payload[index:]

    def payload(self, data_type, payload):
        if not payload:
            return payload
        command = payload[0]
        if data_type in TABLE2_TYPES:
            if command in PERIPHERAL_PARAM_COMMANDS:
                return self.peripheral_list(payload)
        elif data_type in TABLE1_TYPES:
            if command in PLAY_PARAM_COMMANDS:
                return self.track_info(payload)
            if command == CONNECT_RET_CAPABILITY_INFO and self.keep_device_address:
                return payload
        # Anything else: still sweep for addresses, so an unmodelled frame that
        # happens to carry one does not slip through.
        return self.sweep_addresses(payload)

    def file(self, path, dry_run):
        original = open(path, "rb").read()
        data_type, sequence, payload = unpack(original)
        if pack(data_type, sequence, payload) != original:
            raise FrameError("frame does not survive a round trip unmodified")
        updated = pack(data_type, sequence, self.payload(data_type, payload))
        if updated == original:
            return False
        if not dry_run:
            open(path, "wb").write(updated)
        return True


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("directory", nargs="+", help="capture directory to scrub in place")
    parser.add_argument("-n", "--dry-run", action="store_true",
                        help="report what would change without writing")
    parser.add_argument("--scrub-device-address", action="store_true",
                        help="also replace the address the device reports for itself")
    args = parser.parse_args()

    scrubber = Scrubber(keep_device_address=not args.scrub_device_address)
    changed = failed = 0
    for directory in args.directory:
        for path in sorted(glob.glob(os.path.join(directory, "*.bin"))):
            try:
                if scrubber.file(path, args.dry_run):
                    changed += 1
                    verb = "would scrub" if args.dry_run else "scrubbed"
                    print(f"{verb} {os.path.basename(path)}")
            except FrameError as error:
                failed += 1
                print(f"skipped {os.path.basename(path)}: {error}", file=sys.stderr)

    print(f"\n{changed} file(s) changed, {len(scrubber.addresses)} address(es) replaced")
    if failed:
        print(f"{failed} file(s) could not be read as a frame", file=sys.stderr)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
