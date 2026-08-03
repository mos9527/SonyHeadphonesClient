"""Deterministic C++ header renderer for normalized protocol IR."""

from __future__ import annotations

import argparse
from collections import defaultdict
import re
from pathlib import Path

from .model import EnumDecl, FieldDecl, PayloadDecl, TableIR, read_table


GENERATED_BANNER = (
    "// Generated from Sound Connect iOS J2ObjC metadata. Do not edit by hand."
)

OUT_OF_RANGE_IGNORE_REASON = (
    "OUT_OF_RANGE is expected"
)


def _render_v2_shared_types() -> list[str]:
    return [
        "    struct Range",
        "    {",
        "        UInt8 min;",
        "        UInt8 max;",
        "        UInt8 step;",
        "",
        "        MDR_DEFINE_TRIVIAL_SERIALIZATION(Range);",
        "    };",
        "",
        "    struct SupportFunction",
        "    {",
        "        union",
        "        {",
        "            FunctionType_Table1 table1;",
        "            FunctionType_Table2 table2;",
        "        };",
        "        UInt8 priority;",
        "    };",
    ]


def _render_enum(enum: EnumDecl, indent: str = "    ") -> list[str]:
    lines = [
        f"{indent}enum class {enum.cpp_name} : UInt8",
        f"{indent}{{",
    ]
    lines.extend(
        f"{indent}    {value.name} = 0x{value.value:02X},"
        for value in enum.values
    )
    lines.append(f"{indent}}};")
    return lines


def _initializer(field: FieldDecl) -> str:
    if field.default is None:
        return ""
    if field.default == "{}":
        return "{}"
    return "{" + field.default + "}"


def _offset_comment(field: FieldDecl) -> str:
    return f" // 0x{field.offset:X}" if field.offset is not None else ""


def _enum_has_out_of_range(enum: EnumDecl) -> bool:
    return any(value.name == "OUT_OF_RANGE" for value in enum.values)


def _field_semantic_rules(
    field: FieldDecl, enums_by_name: dict[str, EnumDecl]
) -> tuple[str, ...]:
    rules = list(field.semantic_rules)
    if any(
        rule == "Ignore"
        or rule.startswith("Ignore ")
        or rule.startswith("EnumRange ")
        for rule in rules
    ):
        return tuple(rules)
    enum = enums_by_name.get(field.cpp_type)
    if enum is not None and _enum_has_out_of_range(enum):
        rules.append(f"Ignore {OUT_OF_RANGE_IGNORE_REASON}")
    return tuple(rules)


def _render_struct(
    payload: PayloadDecl,
    declarations: dict[str, PayloadDecl],
    enums_by_name: dict[str, EnumDecl],
) -> list[str]:
    lines = [
        f"    // {payload.objc_name}",
        f"    struct {payload.cpp_name}",
        "    {",
    ]
    fields = list(payload.fields)
    if payload.parent:
        parent = declarations[payload.parent]
        parent_field_names = {field.name for field in parent.fields}
        command = next(
            (field for field in fields if field.name == "command"), None
        )
        discriminator = next(
            (
                field
                for field in fields
                if field.name == payload.discriminator_field
            ),
            None,
        )
        if command is None or discriminator is None:
            raise ValueError(
                f"{payload.objc_name} names parent {payload.parent} without "
                "command and discriminator fields"
            )
        if "command" in parent_field_names:
            lines.append(
                f"        // CODEGEN Field command EnumRange {command.default}"
            )
        else:
            lines.append(
                f"        // CODEGEN EnumRange {command.default}"
            )
            lines.append(
                f"        {command.cpp_type} {command.name}"
                f"{_initializer(command)};{_offset_comment(command)}"
            )
        if (
            payload.discriminator_value
            and payload.discriminator_field in parent_field_names
        ):
            lines.append(
                "        // CODEGEN Field "
                f"{payload.discriminator_field} EnumRange "
                f"{payload.discriminator_value}"
            )
        elif payload.discriminator_value:
            lines.append(
                f"        // CODEGEN EnumRange {payload.discriminator_value}"
            )
            lines.append(
                f"        {discriminator.cpp_type} {discriminator.name}"
                f"{_initializer(discriminator)};"
                f"{_offset_comment(discriminator)}"
            )
        parent_values = []
        if "command" in parent_field_names:
            parent_values.append(command.default or "{}")
        if payload.discriminator_field in parent_field_names:
            parent_values.append(
                payload.discriminator_value or discriminator.default or "{}"
            )
        lines.append(
            f"        {payload.parent} base"
            f"{{{', '.join(parent_values)}}};"
        )
        fields = [
            field
            for field in fields
            if field not in (command, discriminator)
        ]

    for field in fields:
        for rule in _field_semantic_rules(field, enums_by_name):
            lines.append(f"        // CODEGEN {rule}")
        lines.append(
            f"        {field.cpp_type} {field.name}"
            f"{_initializer(field)};{_offset_comment(field)}"
        )
    lines.append("")
    if payload.classification == "field_helper":
        lines.append(
            f"        MDR_DEFINE_EXTERN_READ_WRITE({payload.cpp_name});"
        )
    elif payload.serialization == "trivial":
        lines.append(
            f"        MDR_DEFINE_TRIVIAL_SERIALIZATION({payload.cpp_name});"
        )
    else:
        lines.append(
            f"        MDR_DEFINE_EXTERN_SERIALIZATION({payload.cpp_name});"
        )
    lines.append("    };")
    return lines


def _dependencies(
    payload: PayloadDecl, declarations: dict[str, PayloadDecl]
) -> set[str]:
    names = set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", " ".join(
        field.cpp_type for field in payload.fields
    )))
    if payload.parent:
        names.add(payload.parent)
    return (names & declarations.keys()) - {payload.cpp_name}


def _ordered_payloads(payloads: list[PayloadDecl]) -> list[PayloadDecl]:
    declarations: dict[str, PayloadDecl] = {}
    for payload in payloads:
        prior = declarations.get(payload.cpp_name)
        if prior is not None:
            raise ValueError(
                f"C++ declaration collision: {prior.objc_name} and "
                f"{payload.objc_name} both map to {payload.cpp_name}"
            )
        declarations[payload.cpp_name] = payload
    dependencies = {
        name: _dependencies(payload, declarations)
        for name, payload in declarations.items()
    }
    ordered: list[PayloadDecl] = []
    emitted: set[str] = set()
    while len(ordered) != len(payloads):
        ready = sorted(
            name
            for name, required in dependencies.items()
            if name not in emitted and required <= emitted
        )
        if not ready:
            remaining = {
                name: sorted(required - emitted)
                for name, required in dependencies.items()
                if name not in emitted
            }
            raise ValueError(f"cyclic declaration dependencies: {remaining}")
        for name in ready:
            ordered.append(declarations[name])
            emitted.add(name)
    return ordered


def _shared_enums(tables: list[TableIR]) -> dict[int, list[EnumDecl]]:
    output: dict[int, dict[str, EnumDecl]] = defaultdict(dict)
    for table in tables:
        for enum in table.enums:
            if re.match(r"^THMSGV[12]T[12]", enum.objc_name):
                continue
            prior = output[table.version].get(enum.cpp_name)
            if prior is not None and prior.values != enum.values:
                raise ValueError(
                    f"conflicting shared enum {enum.cpp_name}: "
                    f"{prior.objc_name} and {enum.objc_name}"
                )
            output[table.version][enum.cpp_name] = enum
    return {
        version: sorted(values.values(), key=lambda enum: enum.cpp_name)
        for version, values in output.items()
    }


def render_shared_header(version: int, enums: list[EnumDecl]) -> str:
    lines = [
        "#pragma once",
        '#include "Protocol.hpp"',
        "",
        GENERATED_BANNER,
        f"namespace mdr::v{version}",
        "{",
    ]
    for index, enum in enumerate(enums):
        if index:
            lines.append("")
        lines.extend(_render_enum(enum, indent="    "))
    if version == 2:
        lines.append("")
        lines.extend(_render_v2_shared_types())
    lines.extend(
        [
            f"}} // namespace mdr::v{version}",
            "",
            f'#include "Generated/ProtocolV{version}Enum.hpp"',
            "",
        ]
    )
    return "\n".join(lines)


def render_table_header(
    table: TableIR, shared_enums: list[EnumDecl]
) -> str:
    version = table.version
    number = table.table
    local_prefix = f"THMSGV{version}T{number}"
    shared_enum_names = {enum.cpp_name for enum in shared_enums}
    local_enums = [
        enum
        for enum in table.enums
        if enum.cpp_name not in shared_enum_names
        and enum.objc_name.startswith(local_prefix)
    ]
    lines = [
        "#pragma once",
        f'#include "ProtocolV{version}.hpp"',
        *(
            ['#include "ProtocolV2T1.hpp"']
            if version == 2 and number == 2
            else []
        ),
        "#pragma pack(push, 1)",
        "",
        GENERATED_BANNER,
        f"namespace mdr::v{version}::t{number}",
        "{",
        "#pragma region Enums",
    ]
    for index, enum in enumerate(local_enums):
        if index:
            lines.append("")
        lines.extend(_render_enum(enum))
    lines.extend(["#pragma endregion Enums", "", "#pragma region Declarations"])
    declarations = {
        payload.cpp_name: payload for payload in table.payloads
    }
    enums_by_name = {enum.cpp_name: enum for enum in shared_enums}
    enums_by_name.update({enum.cpp_name: enum for enum in table.enums})
    for payload in _ordered_payloads(table.payloads):
        lines.append("")
        lines.extend(_render_struct(payload, declarations, enums_by_name))
    lines.extend(
        [
            "#pragma endregion Declarations",
            f"}} // namespace mdr::v{version}::t{number}",
            "",
            "#pragma pack(pop)",
            "",
            f'#include "Generated/ProtocolV{version}T{number}Enum.hpp"',
            f'#include "Generated/ProtocolV{version}T{number}Traits.hpp"',
            "",
        ]
    )
    return "\n".join(lines)


def render_all(
    ir_directory: Path,
    include_directory: Path,
) -> None:
    tables = [
        read_table(ir_directory / f"v{version}_t{table}.json")
        for version in (1, 2)
        for table in (1, 2)
    ]
    shared = _shared_enums(tables)
    include_directory.mkdir(parents=True, exist_ok=True)
    for version in (1, 2):
        destination = include_directory / f"ProtocolV{version}.hpp"
        destination.write_text(
            render_shared_header(version, shared.get(version, [])),
            encoding="utf-8",
            newline="\n",
        )
        shared_enums = shared.get(version, [])
        for table in [item for item in tables if item.version == version]:
            rendered = render_table_header(table, shared_enums)
            destination = (
                include_directory / f"ProtocolV{version}T{table.table}.hpp"
            )
            destination.write_text(
                rendered,
                encoding="utf-8",
                newline="\n",
            )


def main() -> None:
    parser = argparse.ArgumentParser()
    repository = Path(__file__).resolve().parents[3]
    parser.add_argument(
        "--ir",
        type=Path,
        default=repository / "tooling" / "ida" / "generated",
    )
    parser.add_argument(
        "--include",
        type=Path,
        default=repository / "libmdr" / "include" / "mdr",
    )
    arguments = parser.parse_args()
    render_all(
        arguments.ir,
        arguments.include,
    )


if __name__ == "__main__":
    main()
