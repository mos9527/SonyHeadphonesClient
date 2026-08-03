"""Extract every V1/V2 protocol table from the currently loaded IDB.

Run inside IDA, either from File -> Script file or with ``ida64 -S``.  The
entry point writes normalized JSON and classification manifests only after all
four tables have extracted successfully.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys
import tempfile


if __package__ in (None, ""):
    _IDA_DIRECTORY = Path(__file__).resolve().parent.parent
    if str(_IDA_DIRECTORY) not in sys.path:
        sys.path.insert(0, str(_IDA_DIRECTORY))
    from j2objc.extractors import ProtocolExtractor
    from j2objc.ida_database import IDADatabase, binary_fingerprint
else:
    from .extractors import ProtocolExtractor
    from .ida_database import IDADatabase, binary_fingerprint


EXPECTED_INVENTORY = {
    (1, 1): {"enums": 101, "serializers": 160},
    (1, 2): {"enums": 12, "serializers": 47},
    (2, 1): {"enums": 111, "serializers": 585},
    (2, 2): {"enums": 47, "serializers": 300},
}


def _serializer_count(database: IDADatabase, version: int, table: int) -> int:
    prefix = f"THMSGV{version}T{table}"
    if (version, table) == (1, 1):
        return sum(
            1
            for class_name in database.protocol_classes()
            if class_name.startswith(prefix)
            for method in database.methods(class_name)
            if method.selector == "getCommandStream"
        )
    return sum(
        1
        for class_name in database.protocol_classes()
        if class_name.startswith(prefix)
        for method in database.methods(class_name)
        if method.selector.startswith("parseBytesWith")
    )


def _atomic_write_json(path: Path, data: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    text = json.dumps(data, indent=2, sort_keys=True) + "\n"
    with tempfile.NamedTemporaryFile(
        "w",
        encoding="utf-8",
        newline="\n",
        dir=path.parent,
        prefix=path.name + ".",
        suffix=".tmp",
        delete=False,
    ) as output:
        output.write(text)
        temporary = Path(output.name)
    temporary.replace(path)


def extract_all(output_directory: Path, *, verify_inventory: bool = True) -> None:
    database = IDADatabase()
    extractor = ProtocolExtractor(database)
    fingerprint = binary_fingerprint()
    tables = {}
    summary = {
        "binary_fingerprint": fingerprint,
        "tables": {},
    }
    for coordinates, expected in EXPECTED_INVENTORY.items():
        version, table = coordinates
        actual_enum_count = sum(
            1
            for class_name in database.protocol_classes()
            if class_name.startswith(f"THMSGV{version}T{table}")
            and not class_name.endswith("_Factory")
            and any(
                method.selector == "byteCode"
                for method in database.methods(class_name)
            )
        )
        actual_serializer_count = _serializer_count(
            database, version, table
        )
        actual = {
            "enums": actual_enum_count,
            "serializers": actual_serializer_count,
        }
        if verify_inventory and actual != expected:
            raise RuntimeError(
                f"inventory mismatch for V{version}T{table}: "
                f"expected {expected}, found {actual}"
            )
        extracted = extractor.extract_table(version, table, fingerprint)
        tables[coordinates] = extracted
        summary["tables"][f"v{version}_t{table}"] = {
            **actual,
            "payload_declarations": len(extracted.payloads),
            "candidates": len(extracted.candidates),
            "classifications": {
                classification: sum(
                    1
                    for candidate in extracted.candidates
                    if candidate.classification == classification
                )
                for classification in sorted(
                    {
                        candidate.classification
                        for candidate in extracted.candidates
                    }
                )
            },
        }

    # Do not leave a partially refreshed protocol model.
    for (version, table), extracted in tables.items():
        extracted.write(output_directory / f"v{version}_t{table}.json")
        _atomic_write_json(
            output_directory / f"v{version}_t{table}.manifest.json",
            {
                "version": version,
                "table": table,
                "binary_fingerprint": fingerprint,
                "candidates": [
                    {
                        "objc_name": candidate.objc_name,
                        "classification": candidate.classification,
                        "reason": candidate.reason,
                        "source_addresses": [
                            f"0x{address:X}"
                            for address in candidate.source_addresses
                        ],
                    }
                    for candidate in extracted.candidates
                ],
            },
        )
    _atomic_write_json(output_directory / "inventory.json", summary)


def main(argv: list[str] | None = None) -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output",
        type=Path,
        default=Path(__file__).resolve().parent.parent / "generated",
    )
    parser.add_argument(
        "--no-inventory-check",
        action="store_true",
        help="allow use with a different binary while developing extraction",
    )
    arguments = parser.parse_args(argv)
    extract_all(
        arguments.output.resolve(),
        verify_inventory=not arguments.no_inventory_check,
    )


if __name__ == "__main__":
    main()
