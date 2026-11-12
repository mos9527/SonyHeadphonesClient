"""Serializable intermediate representation for extracted MDR declarations."""

from __future__ import annotations

from dataclasses import asdict, dataclass, field
import json
from pathlib import Path
from typing import Any


SCHEMA_VERSION = 1


@dataclass(frozen=True)
class SourceRef:
    address: int
    symbol: str


@dataclass(frozen=True)
class EnumValue:
    name: str
    value: int
    source_address: int


@dataclass(frozen=True)
class EnumDecl:
    objc_name: str
    cpp_name: str
    values: tuple[EnumValue, ...]
    sources: tuple[SourceRef, ...]


@dataclass(frozen=True)
class FieldDecl:
    name: str
    cpp_type: str
    wire_kind: str
    offset: int | None = None
    default: str | None = None
    source_type: str | None = None
    semantic_rules: tuple[str, ...] = ()


@dataclass(frozen=True)
class PayloadDecl:
    objc_name: str
    cpp_name: str
    classification: str
    command: str | None
    fields: tuple[FieldDecl, ...]
    serialization: str
    parser: str | None
    factory_signature: str | None
    discriminator_field: str | None = None
    discriminator_value: str | None = None
    parent: str | None = None
    sources: tuple[SourceRef, ...] = ()
    evidence: tuple[str, ...] = ()


@dataclass(frozen=True)
class Candidate:
    objc_name: str
    classification: str
    reason: str
    source_addresses: tuple[int, ...]


@dataclass
class TableIR:
    version: int
    table: int
    binary_fingerprint: str
    enums: list[EnumDecl] = field(default_factory=list)
    payloads: list[PayloadDecl] = field(default_factory=list)
    candidates: list[Candidate] = field(default_factory=list)

    @property
    def namespace(self) -> str:
        return f"mdr::v{self.version}::t{self.table}"

    def normalize(self) -> None:
        self.enums.sort(key=lambda item: (item.cpp_name, item.objc_name))
        self.payloads.sort(key=lambda item: (item.cpp_name, item.objc_name))
        self.candidates.sort(key=lambda item: item.objc_name)

    def to_dict(self) -> dict[str, Any]:
        self.normalize()
        return {
            "schema_version": SCHEMA_VERSION,
            "version": self.version,
            "table": self.table,
            "namespace": self.namespace,
            "binary_fingerprint": self.binary_fingerprint,
            "enums": [asdict(item) for item in self.enums],
            "payloads": [asdict(item) for item in self.payloads],
            "candidates": [asdict(item) for item in self.candidates],
        }

    def write(self, path: Path) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)
        text = json.dumps(
            self.to_dict(), indent=2, sort_keys=True, ensure_ascii=False
        )
        path.write_text(text + "\n", encoding="utf-8", newline="\n")


def _tuple_sources(values: list[dict[str, Any]]) -> tuple[SourceRef, ...]:
    return tuple(SourceRef(**item) for item in values)


def read_table(path: Path) -> TableIR:
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("schema_version") != SCHEMA_VERSION:
        raise ValueError(
            f"unsupported IR schema {data.get('schema_version')!r} in {path}"
        )
    table = TableIR(
        version=data["version"],
        table=data["table"],
        binary_fingerprint=data["binary_fingerprint"],
    )
    for item in data["enums"]:
        table.enums.append(
            EnumDecl(
                objc_name=item["objc_name"],
                cpp_name=item["cpp_name"],
                values=tuple(EnumValue(**value) for value in item["values"]),
                sources=_tuple_sources(item["sources"]),
            )
        )
    for item in data["payloads"]:
        table.payloads.append(
            PayloadDecl(
                objc_name=item["objc_name"],
                cpp_name=item["cpp_name"],
                classification=item["classification"],
                command=item["command"],
                fields=tuple(FieldDecl(**value) for value in item["fields"]),
                serialization=item["serialization"],
                parser=item["parser"],
                factory_signature=item["factory_signature"],
                discriminator_field=item.get("discriminator_field"),
                discriminator_value=item.get("discriminator_value"),
                parent=item.get("parent"),
                sources=_tuple_sources(item["sources"]),
                evidence=tuple(item.get("evidence", ())),
            )
        )
    table.candidates.extend(Candidate(**item) for item in data["candidates"])
    table.normalize()
    return table
