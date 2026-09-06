"""Deterministic C++ header renderer for normalized protocol IR."""

from __future__ import annotations

import argparse
from collections import defaultdict
from dataclasses import replace
import json
import re
from pathlib import Path

from .model import EnumDecl, FieldDecl, PayloadDecl, TableIR, read_table


GENERATED_BANNER = (
    "// Generated from Sound Connect iOS J2ObjC metadata. Do not edit by hand."
)

OUT_OF_RANGE_IGNORE_REASON = (
    "OUT_OF_RANGE is expected"
)

V2_FUNCTION_TYPE_CLASS = (
    "ComSonySongpalTandemfamilyMessageMdrV2FunctionType"
)


def _render_function_type_helpers(
    enum: EnumDecl,
    table: int,
) -> list[str]:
    qualified = f"t{table}::FunctionType"
    lines = [
        f"    static const char* format_as({qualified} value)",
        "    {",
        f"        using enum {qualified};",
        "        switch (value)",
        "        {",
    ]
    lines.extend(
        f'        case {value.name}: return "{value.name}";'
        for value in enum.values
    )
    lines.extend(
        [
            '        default: return "Unknown";',
            "        }",
            "    }",
            "",
            f"    static bool is_valid({qualified} value)",
            "    {",
            f"        using enum {qualified};",
            "        switch (value)",
            "        {",
        ]
    )
    lines.extend(
        f"        case {value.name}:"
        for value in enum.values
    )
    lines.extend(
        [
            "            return true;",
            "        default:",
            "            return false;",
            "        }",
            "    }",
        ]
    )
    return lines


def _render_v2_shared_types(
    function_types: dict[int, EnumDecl],
) -> list[str]:
    lines = [
        "    struct Range",
        "    {",
        "        UInt8 min;",
        "        UInt8 max;",
        "        UInt8 step;",
        "",
        "        MDR_DEFINE_TRIVIAL_SERIALIZATION(Range);",
        "    };",
    ]
    for table in (1, 2):
        lines.extend(
            [
                "",
                f"    namespace t{table}",
                "    {",
            ]
        )
        lines.extend(_render_enum(function_types[table], indent="        "))
        lines.extend(
            [
                "",
                "        struct SupportFunction",
                "        {",
                "            FunctionType functionType;",
                "            UInt8 priority;",
                "        };",
                f"    }} // namespace t{table}",
            ]
        )
    for table in (1, 2):
        lines.append("")
        lines.extend(
            _render_function_type_helpers(function_types[table], table)
        )
    return lines


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
    field: FieldDecl,
    enums_by_name: dict[str, EnumDecl],
    payload: PayloadDecl,
) -> tuple[str, ...]:
    del payload
    rules = list(field.semantic_rules)
    enum = enums_by_name.get(field.cpp_type)
    has_out_of_range = enum is not None and _enum_has_out_of_range(enum)
    if has_out_of_range:
        # Open enums map unknown wire bytes to OUT_OF_RANGE. EnumRange pins
        # (including discriminators inferred from the payload name) reject
        # that sentinel. Nested-variant tags use CODEGEN Field ... EnumRange
        # instead, emitted separately in _render_struct.
        rules = [
            rule for rule in rules if not rule.startswith("EnumRange ")
        ]
    if any(
        rule == "Ignore"
        or rule.startswith("Ignore ")
        or rule.startswith("EnumRange ")
        for rule in rules
    ):
        return tuple(rules)
    if has_out_of_range:
        rules.append(f"Ignore {OUT_OF_RANGE_IGNORE_REASON}")
    return tuple(rules)


def _render_struct(
    payload: PayloadDecl,
    declarations: dict[str, PayloadDecl],
    enums_by_name: dict[str, EnumDecl],
    *,
    ignore_serialization: bool = False,
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
        for rule in _field_semantic_rules(field, enums_by_name, payload):
            lines.append(f"        // CODEGEN {rule}")
        lines.append(
            f"        {field.cpp_type} {field.name}"
            f"{_initializer(field)};{_offset_comment(field)}"
        )
    lines.append("")
    if ignore_serialization:
        lines.append("        MDR_CODEGEN_IGNORE_SERIALIZATION")
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
            if (
                table.version == 2
                and enum.objc_name == V2_FUNCTION_TYPE_CLASS
            ):
                continue
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


def render_shared_header(
    version: int,
    enums: list[EnumDecl],
    function_types: dict[int, EnumDecl] | None = None,
) -> str:
    rendered_enums = list(enums)
    if version == 2:
        if function_types is None or set(function_types) != {1, 2}:
            raise ValueError("V2 requires function types for tables 1 and 2")
    lines = [
        "#pragma once",
        '#include "Protocol.hpp"',
        "",
        GENERATED_BANNER,
        f"namespace mdr::v{version}",
        "{",
    ]
    for index, enum in enumerate(rendered_enums):
        if index:
            lines.append("")
        lines.extend(_render_enum(enum, indent="    "))
    if version == 2:
        lines.append("")
        lines.extend(_render_v2_shared_types(function_types))
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
    table: TableIR,
    shared_enums: list[EnumDecl],
    ignored_serialization: frozenset[str] = frozenset(),
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
        lines.extend(
            _render_struct(
                payload,
                declarations,
                enums_by_name,
                ignore_serialization=(
                    payload.cpp_name in ignored_serialization
                ),
            )
        )
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
    overrides_path: Path | None = None,
) -> None:
    tables = [
        read_table(ir_directory / f"v{version}_t{table}.json")
        for version in (1, 2)
        for table in (1, 2)
    ]
    overrides = (
        json.loads(overrides_path.read_text(encoding="utf-8"))
        if overrides_path is not None
        else {}
    )
    ignored_serialization: dict[tuple[int, int], frozenset[str]] = {}
    for table in tables:
        table_overrides = overrides.get(
            f"v{table.version}_t{table.table}", {}
        )
        payload_names = {
            payload.cpp_name for payload in table.payloads
        }
        unknown_payloads = set(table_overrides) - payload_names
        if unknown_payloads:
            raise ValueError(
                f"unknown render overrides for v{table.version}_t"
                f"{table.table}: {sorted(unknown_payloads)}"
            )
        updated_payloads: list[PayloadDecl] = []
        ignored: set[str] = set()
        for payload in table.payloads:
            payload_override = table_overrides.get(payload.cpp_name, {})
            unknown_options = set(payload_override) - {
                "field_offsets",
                "field_order",
                "ignore_serialization",
            }
            if unknown_options:
                raise ValueError(
                    f"unknown override options for {payload.cpp_name}: "
                    f"{sorted(unknown_options)}"
                )
            field_order = payload_override.get("field_order")
            if field_order is not None:
                by_name = {field.name: field for field in payload.fields}
                if set(field_order) != set(by_name):
                    raise ValueError(
                        f"field_order override for {payload.cpp_name} "
                        f"does not match extracted fields"
                    )
                payload = replace(
                    payload,
                    fields=tuple(by_name[name] for name in field_order),
                )
            field_offsets = payload_override.get("field_offsets", {})
            unknown_fields = set(field_offsets) - {
                field.name for field in payload.fields
            }
            if unknown_fields:
                raise ValueError(
                    f"unknown offset fields for {payload.cpp_name}: "
                    f"{sorted(unknown_fields)}"
                )
            if field_offsets:
                payload = replace(
                    payload,
                    fields=tuple(
                        replace(
                            field,
                            offset=field_offsets.get(
                                field.name, field.offset
                            ),
                        )
                        for field in payload.fields
                    ),
                )
            if payload_override.get("ignore_serialization", False):
                ignored.add(payload.cpp_name)
            updated_payloads.append(payload)
        table.payloads = updated_payloads
        ignored_serialization[(table.version, table.table)] = frozenset(
            ignored
        )
    shared = _shared_enums(tables)
    function_types = {
        table.table: next(
            enum
            for enum in table.enums
            if enum.objc_name == V2_FUNCTION_TYPE_CLASS
        )
        for table in tables
        if table.version == 2
    }
    include_directory.mkdir(parents=True, exist_ok=True)
    for version in (1, 2):
        destination = include_directory / f"ProtocolV{version}.hpp"
        destination.write_text(
            render_shared_header(
                version,
                shared.get(version, []),
                function_types if version == 2 else None,
            ),
            encoding="utf-8",
            newline="\n",
        )
        shared_enums = shared.get(version, [])
        for table in [item for item in tables if item.version == version]:
            rendered = render_table_header(
                table,
                shared_enums,
                ignored_serialization[(version, table.table)],
            )
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
    parser.add_argument(
        "--overrides",
        type=Path,
        default=(
            repository
            / "tooling"
            / "ida"
            / "j2objc"
            / "render_overrides.json"
        ),
    )
    arguments = parser.parse_args()
    render_all(
        arguments.ir,
        arguments.include,
        arguments.overrides,
    )


if __name__ == "__main__":
    main()
