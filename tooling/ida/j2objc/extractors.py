"""Protocol declaration extractors built on J2ObjC reflection metadata."""

from __future__ import annotations

from collections import Counter
from dataclasses import replace
import re

import ida_bytes
import ida_funcs
import ida_hexrays
import ida_ida
import ida_name
import idaapi
import idautils
import idc

from .ida_database import (
    ExtractionError,
    IDADatabase,
    JavaClassMetadata,
    JavaMethod,
    MethodSymbol,
    parse_descriptors,
    protocol_coordinates,
    read_pointer,
)
from .model import (
    Candidate,
    EnumDecl,
    EnumValue,
    FieldDecl,
    PayloadDecl,
    SourceRef,
    TableIR,
)
_ENUM_MEMBER_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")
_GETTER_RE = re.compile(r"^(?:get|is)(?P<name>[A-Z][A-Za-z0-9_]*)$")
_COMMAND_WORDS = {"GET", "RET", "SET", "NTFY"}
_GENERIC_NAME_TOKENS = {
    "TYPE",
    "STATUS",
    "DATA",
    "PARAM",
    "CAPABILITY",
    "INFO",
    "AND",
}
_FIXED_SIZES = {
    "UInt8": 1,
    "Int8": 1,
    "UInt16BE": 2,
    "Int16BE": 2,
    "Int24BE": 3,
    "UInt32BE": 4,
    "Int32BE": 4,
    "UInt64BE": 8,
    "Int64BE": 8,
    "Range": 3,
    "SupportFunction": 2,
}


def _unwrap(expression):
    while expression.op in (ida_hexrays.cot_cast, ida_hexrays.cot_ref):
        expression = expression.x
    return expression


def _read_cfstring(expression) -> str | None:
    expression = _unwrap(expression)
    if expression.op != ida_hexrays.cot_obj:
        return None
    address = expression.obj_ea
    if not ida_name.get_name(address).lower().startswith("cfstr_"):
        return None
    pointer_size = 8 if ida_ida.inf_is_64bit() else 4
    string_address = read_pointer(address + 2 * pointer_size)
    size = read_pointer(address + 3 * pointer_size)
    raw = ida_bytes.get_bytes(string_address, size)
    if raw is None:
        return None
    return raw.decode("utf-8", errors="strict")


def _read_cfstring_address(address: int) -> str | None:
    if not ida_name.get_name(address).lower().startswith("cfstr_"):
        return None
    pointer_size = 8 if ida_ida.inf_is_64bit() else 4
    string_address = read_pointer(address + 2 * pointer_size)
    size = read_pointer(address + 3 * pointer_size)
    raw = ida_bytes.get_bytes(string_address, size)
    return raw.decode("utf-8", errors="strict") if raw is not None else None


def _register_key(operand: str) -> str | None:
    match = re.fullmatch(r"[WX](\d+)", operand.upper())
    return f"R{match.group(1)}" if match else None


def _enum_values_from_assembly(method: MethodSymbol) -> list[EnumValue]:
    """Recover enum helper arguments when Hex-Rays omits register parameters."""
    function = ida_funcs.get_func(method.address)
    if function is None:
        raise ExtractionError(f"missing function object for {method.symbol}")
    registers: dict[str, int] = {}
    stack: dict[str, int] = {}
    values: list[EnumValue] = []
    address = function.start_ea
    while address < function.end_ea:
        mnemonic = idc.print_insn_mnem(address).upper()
        operand0 = idc.print_operand(address, 0)
        operand1 = idc.print_operand(address, 1)
        destination = _register_key(operand0)
        source = _register_key(operand1)
        if mnemonic == "MOV" and destination is not None:
            if source is not None:
                registers[destination] = registers.get(source, -1)
            elif idc.get_operand_type(address, 1) == idc.o_imm:
                registers[destination] = idc.get_operand_value(address, 1)
            else:
                registers.pop(destination, None)
        elif mnemonic in ("ADR", "ADRL", "ADRP") and destination is not None:
            registers[destination] = idc.get_operand_value(address, 1)
        elif mnemonic.startswith("LDR") and destination is not None:
            if operand1.upper().startswith("[SP"):
                if operand1 in stack:
                    registers[destination] = stack[operand1]
                else:
                    registers.pop(destination, None)
            else:
                registers.pop(destination, None)
        elif mnemonic.startswith("STR"):
            source_key = _register_key(operand0)
            if source_key is not None and operand1.upper().startswith("[SP"):
                if source_key in registers:
                    stack[operand1] = registers[source_key]
        elif mnemonic in ("BL", "BLR"):
            name = _read_cfstring_address(registers.get("R2", 0))
            code = registers.get("R1")
            target_name = idc.get_func_name(idc.get_operand_value(address, 0))
            if (
                (
                    target_name.startswith("sub_")
                    or "JavaLangEnum_initWithNSString_withInt_" in target_name
                )
                and name is not None
                and _ENUM_MEMBER_RE.fullmatch(name)
                and code is not None
            ):
                values.append(EnumValue(name, code & 0xFF, address))
            for register in [f"R{index}" for index in range(18)]:
                registers.pop(register, None)
        elif destination is not None:
            registers.pop(destination, None)
        address = idc.next_head(address, function.end_ea)
    return values


class _EnumVisitor(ida_hexrays.ctree_visitor_t):
    def __init__(self) -> None:
        super().__init__(ida_hexrays.CV_FAST)
        self.values: list[EnumValue] = []

    def visit_expr(self, expression) -> int:
        if expression.op != ida_hexrays.cot_call:
            return 0
        names = [
            value
            for argument in expression.a
            if (value := _read_cfstring(argument)) is not None
            and _ENUM_MEMBER_RE.fullmatch(value)
        ]
        integers = [
            argument.numval() & 0xFF
            for argument in expression.a
            if argument.op == ida_hexrays.cot_num
        ]
        if len(names) == 1 and integers:
            self.values.append(EnumValue(names[0], integers[0], expression.ea))
        return 0


class _ObjectReferenceVisitor(ida_hexrays.ctree_visitor_t):
    def __init__(self) -> None:
        super().__init__(ida_hexrays.CV_FAST)
        self.addresses: list[int] = []

    def visit_expr(self, expression) -> int:
        expression = _unwrap(expression)
        if expression.op == ida_hexrays.cot_obj:
            self.addresses.append(expression.obj_ea)
        return 0


class _V2FunctionTypeVisitor(ida_hexrays.ctree_visitor_t):
    def __init__(self, table_addresses: dict[int, int]) -> None:
        super().__init__(ida_hexrays.CV_FAST)
        self.table_addresses = {
            address: table for table, address in table_addresses.items()
        }
        self.values: dict[int, list[EnumValue]] = {
            table: [] for table in table_addresses
        }

    def visit_expr(self, expression) -> int:
        if expression.op != ida_hexrays.cot_call:
            return 0
        names = [
            value
            for argument in expression.a
            if (value := _read_cfstring(argument)) is not None
            and _ENUM_MEMBER_RE.fullmatch(value)
        ]
        integers = [
            argument.numval() & 0xFF
            for argument in expression.a
            if argument.op == ida_hexrays.cot_num
        ]
        tables = [
            self.table_addresses[argument.obj_ea]
            for argument in expression.a
            if argument.op == ida_hexrays.cot_obj
            and argument.obj_ea in self.table_addresses
        ]
        if len(names) == 1 and integers and len(tables) == 1:
            self.values[tables[0]].append(
                EnumValue(names[0], integers[0], expression.ea)
            )
        return 0


class _EvidenceVisitor(ida_hexrays.ctree_visitor_t):
    def __init__(self) -> None:
        super().__init__(ida_hexrays.CV_FAST)
        self.calls: list[tuple[int, str]] = []

    def visit_expr(self, expression) -> int:
        if expression.op != ida_hexrays.cot_call:
            return 0
        target = _unwrap(expression.x)
        if target.op == ida_hexrays.cot_obj:
            name = idc.get_func_name(target.obj_ea)
            if name:
                self.calls.append((expression.ea, name))
        for argument in expression.a:
            if argument.op == ida_hexrays.cot_str and argument.string:
                selector = argument.string
                if selector.startswith(
                    (
                        "write",
                        "parse",
                        "restore",
                        "read",
                        "toStream",
                    )
                ):
                    self.calls.append((expression.ea, selector))
        return 0


class ProtocolExtractor:
    def __init__(self, database: IDADatabase) -> None:
        self.database = database
        self._enum_cache: dict[tuple[str, str], EnumDecl] = {}
        self._metadata_cache: dict[str, JavaClassMetadata] = {}
        self._enum_global_cache: dict[str, dict[int, str]] = {}
        self._current_coordinates: tuple[int, int] | None = None
        self._decompile_text_cache: dict[int, str] = {}
        self._external_field_cache: dict[
            tuple[str, tuple[str, ...]], tuple[tuple[str, ...], str] | None
        ] = {}

    def metadata(self, class_name: str) -> JavaClassMetadata:
        if class_name not in self._metadata_cache:
            self._metadata_cache[class_name] = self.database.metadata(class_name)
        return self._metadata_cache[class_name]

    def _decompile_text(self, method: JavaMethod | MethodSymbol) -> str:
        address = (
            method.source_address
            if isinstance(method, JavaMethod)
            else method.address
        )
        cached = self._decompile_text_cache.get(address)
        if cached is not None:
            return cached
        if isinstance(method, JavaMethod):
            symbol = MethodSymbol(
                address,
                method.kind,
                "",
                method.selector,
                method.selector,
            )
        else:
            symbol = method
        text = str(self.database.decompile(symbol))
        self._decompile_text_cache[address] = text
        return text

    def _expanded_method_text(self, method: MethodSymbol) -> str:
        text = self._decompile_text(method)
        called = set(re.findall(r"\b(sub_[0-9A-Fa-f]+)\s*\(", text))
        for name in sorted(called):
            address = ida_name.get_name_ea(idaapi.BADADDR, name)
            if address == idaapi.BADADDR or ida_funcs.get_func(address) is None:
                continue
            text += "\n" + self._decompile_text(
                MethodSymbol(address, "", "", name, name)
            )
        return text

    def _external_field_evidence(
        self, class_name: str, field_names: list[str]
    ) -> tuple[tuple[str, ...], str] | None:
        key = (class_name, tuple(field_names))
        if key in self._external_field_cache:
            return self._external_field_cache[key]
        version, table, _ = protocol_coordinates(class_name)
        prefix = f"THMSGV{version}T{table}"
        package_token = self.metadata(class_name).package_name.rsplit(".", 1)[-1]
        token = self._sanitize_identifier(package_token).lower()
        selectors = {
            field: "get" + field[:1].upper() + field[1:]
            for field in field_names
        }
        orders: set[tuple[str, ...]] = set()
        evidence: list[str] = []
        for candidate_class in self.database.protocol_classes():
            if (
                not candidate_class.startswith(prefix)
                or token not in candidate_class.lower()
            ):
                continue
            for method in self.database.methods(candidate_class):
                if not (
                    method.selector.lower().startswith(("write", "get"))
                    or "processor" in candidate_class.lower()
                ):
                    continue
                try:
                    text = self._expanded_method_text(method)
                except ExtractionError:
                    continue
                positions = {
                    field: text.find(f'"{selector}"')
                    for field, selector in selectors.items()
                }
                present = {
                    field: position
                    for field, position in positions.items()
                    if position >= 0
                }
                if len(present) >= 2:
                    evidence.append(text)
                elif "deviceinfo" in method.selector.lower():
                    evidence.append(text)
                if len(present) == len(field_names):
                    orders.add(
                        tuple(
                            field
                            for field, _ in sorted(
                                present.items(), key=lambda item: item[1]
                            )
                        )
                    )
        if len(orders) > 1:
            raise ExtractionError(
                f"contradictory external field orders for {class_name}: "
                f"{sorted(orders)}"
            )
        result = (
            (next(iter(orders)), "\n".join(evidence))
            if len(orders) == 1
            else None
        )
        self._external_field_cache[key] = result
        return result

    def _getter_byte_span(
        self, getter: JavaMethod
    ) -> tuple[int, int] | None:
        """Recover a consecutive byte span read by a translated getter."""
        text = self._decompile_text(getter)
        ranges = self._copyof_range_spans(text)
        if len(ranges) == 1:
            return next(iter(ranges))
        helper_span = self._integer_helper_span(text)
        if helper_span is not None:
            return helper_span
        wrapper_spans = self._wrapper_fixed_spans(text)
        if len(wrapper_spans) == 1:
            return next(iter(wrapper_spans))
        constants = {
            name: int(value, 0)
            for name, value in re.findall(
                r"\b([A-Za-z_][A-Za-z0-9_]*)\s*=\s*"
                r"(0x[0-9A-Fa-f]+|\d+)\s*;",
                text,
            )
        }
        indices: set[int] = set()
        if re.search(r"IOSByteArray_buffer_\s*\]", text):
            indices.add(0)
        for expression in re.findall(
            r"IOSByteArray_buffer_\s*\+\s*"
            r"([A-Za-z_][A-Za-z0-9_]*|0x[0-9A-Fa-f]+|\d+)",
            text,
        ):
            value = constants.get(expression)
            if value is None:
                try:
                    value = int(expression, 0)
                except ValueError:
                    continue
            if 0 <= value <= 0xFFFF:
                indices.add(value)
        if not indices:
            return None
        ordered = sorted(indices)
        if ordered != list(range(ordered[0], ordered[-1] + 1)):
            return None
        return ordered[0], len(ordered)

    @staticmethod
    def _copyof_range_spans(text: str) -> set[tuple[int, int]]:
        return {
            (int(start), int(end) - int(start))
            for start, end in re.findall(
                r"JavaUtilArrays_copyOfRangeWithByteArray_withInt_withInt_"
                r"\([\s\S]{0,400},\s*(\d+),\s*(\d+)\s*\)",
                text,
            )
            if int(end) > int(start)
        }

    def _wrapper_fixed_spans(self, text: str) -> set[tuple[int, int]]:
        """Follow a single-level wrapper (e.g. sub_xxx(bytes, start, len))
        that forwards to a fixed byte-array slice, recovering the fixed
        (start, length) only when the wrapper body proves a matching slice.
        The third wrapper argument is the slice length (not an end offset).
        """
        spans: set[tuple[int, int]] = set()
        for symbol, start, length in re.findall(
            r"\b(sub_[0-9A-Fa-f]+)\([\s\S]*?,\s*"
            r"(0x[0-9A-Fa-f]+|\d+)[uUlL]*\s*,\s*"
            r"(0x[0-9A-Fa-f]+|\d+)[uUlL]*\s*\)",
            text,
        ):
            try:
                start_value = int(start, 0)
                length_value = int(length, 0)
            except ValueError:
                continue
            if length_value < 6 or length_value > 64:
                continue
            address = ida_name.get_name_ea(idaapi.BADADDR, symbol)
            if address == idaapi.BADADDR or ida_funcs.get_func(address) is None:
                continue
            try:
                callee = self._decompile_text(
                    MethodSymbol(address, "", "", symbol, symbol)
                )
            except Exception:
                continue
            # Direct copyOfRange(from, from + length) proves (start, length).
            if (start_value, length_value) in self._copyof_range_spans(callee):
                spans.add((start_value, length_value))
                continue
            # java_stringWithBytes:(bytes, offset, length, charset) is a fixed
            # length byte slice (e.g. a 17-byte MAC address laid out inline).
            if "java_stringWithBytes:offset:length:charset:" in callee:
                spans.add((start_value, length_value))
                continue
            if (start_value, length_value) in self._wrapper_fixed_spans(callee):
                spans.add((start_value, length_value))
        return spans

    def _integer_helper_span(
        self, text: str, depth: int = 2
    ) -> tuple[int, int] | None:
        if depth < 0:
            return None
        calls = re.findall(
            r"\b(sub_[0-9A-Fa-f]+)\s*\(([^;\r\n]*)\)", text
        )
        spans: set[tuple[int, int]] = set()
        for symbol, arguments in calls:
            address = ida_name.get_name_ea(idaapi.BADADDR, symbol)
            if address == idaapi.BADADDR or ida_funcs.get_func(address) is None:
                continue
            callee = self._decompile_text(
                MethodSymbol(address, "", "", symbol, symbol)
            )
            widths = {
                int(value, 0)
                for value in re.findall(
                    r">=\s*[A-Za-z_][A-Za-z0-9_]*\s*\+\s*"
                    r"(0x[0-9A-Fa-f]+|\d+)[uUlL]*",
                    callee,
                )
                if 1 <= int(value, 0) <= 8
            }
            offsets = re.findall(
                r",\s*(0x[0-9A-Fa-f]+|\d+)[uUlL]*\s*$",
                arguments,
            )
            if len(widths) == 1 and len(offsets) == 1:
                spans.add((int(offsets[0], 0), next(iter(widths))))
                continue
            nested = self._integer_helper_span(callee, depth - 1)
            if nested is not None:
                spans.add(nested)
        return next(iter(spans)) if len(spans) == 1 else None

    def _static_getter_span(
        self, class_name: str, getter: JavaMethod
    ) -> tuple[int, int] | None:
        field_name = self._getter_field_name(getter)
        suffix = "_".join(self._camel_tokens(field_name))
        constants = {
            constant.name: constant
            for constant in self.database.static_integer_constants(class_name)
        }
        index = constants.get(f"INDEX_{suffix}")
        size = constants.get(f"BYTE_SIZE_{suffix}")
        if index is not None and size is not None:
            return index.value, size.value
        return None

    @staticmethod
    def _string_field_is_length_prefixed(
        serializer_text: str, getter_selector: str
    ) -> bool:
        """Return True when the serializer writes an NSString field as a
        length-prefixed (dynamic) string rather than a fixed byte slice.

        J2ObjC serializes a dynamic string as a ``java_length`` write followed
        by a ``java_getBytes`` write of the same getter result, while a fixed
        byte array is written with ``java_getBytes`` alone. Matching the
        getter selector to the local variable it is assigned to isolates the
        field from unrelated writes in the same serializer.
        """
        for variable in re.findall(
            r"\b([A-Za-z_]\w*)\s*=\s*objc_msgSend\([^;]*?"
            rf"\"{re.escape(getter_selector)}\"\)",
            serializer_text,
        ):
            if re.search(
                rf"objc_msgSend\(\s*{re.escape(variable)}\s*,\s*"
                r"\"java_length\"\)",
                serializer_text,
            ):
                return True
        return False

    def _getter_span(
        self, class_name: str, getter: JavaMethod
    ) -> tuple[int, int] | None:
        return (
            self._static_getter_span(class_name, getter)
            or self._getter_byte_span(getter)
        )

    @staticmethod
    def _field_ivar_name(field: FieldDecl) -> str:
        return f"m{field.name[:1].upper()}{field.name[1:]}_"

    def _serializer_primitive_width(
        self,
        serializer: JavaMethod | MethodSymbol | None,
        field: FieldDecl,
    ) -> int | None:
        if serializer is None:
            return None
        text = self._decompile_text(serializer)
        ivar = re.escape(self._field_ivar_name(field))
        writes = re.findall(
            r"objc_msgSend\([^;]*?\"writeWithInt:\"\s*,\s*([^;]*?)\);",
            text,
            re.DOTALL,
        )
        matching = [
            expression
            for expression in writes
            if re.search(rf"(?:self|[A-Za-z_][A-Za-z0-9_]*)->{ivar}\b",
                         expression)
        ]
        return len(matching) if 1 <= len(matching) <= 8 else None

    def _parser_primitive_span(
        self,
        parser: JavaMethod,
        field: FieldDecl,
    ) -> tuple[int, int] | None:
        text = self._decompile_text(parser)
        ivar = re.escape(self._field_ivar_name(field))
        target = re.search(
            rf"(?:self|[A-Za-z_][A-Za-z0-9_]*)->{ivar}\s*=\s*([^;]+);",
            text,
        )
        if target is None:
            return None
        assignments = {
            name: expression
            for name, expression in re.findall(
                r"(?m)^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*([^;]+);",
                text,
            )
        }

        def byte_indices(expression: str, seen: set[str]) -> set[int]:
            indices = {
                int(value, 0)
                for value in re.findall(
                    r"IOSByteArray_buffer_\s*\+\s*"
                    r"(0x[0-9A-Fa-f]+|\d+)",
                    expression,
                )
            }
            if re.search(r"IOSByteArray_buffer_\s*\]", expression):
                indices.add(0)
            for name in re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b",
                                   expression):
                if name in seen or name not in assignments:
                    continue
                indices.update(
                    byte_indices(assignments[name], {*seen, name})
                )
            return indices

        indices = sorted(byte_indices(target.group(1), set()))
        if not indices or indices != list(range(indices[0], indices[-1] + 1)):
            return None
        return indices[0], len(indices)

    def _primitive_span_evidence(
        self,
        class_name: str,
        serializer: JavaMethod | MethodSymbol | None,
        parsers: list[JavaMethod],
        fields: list[FieldDecl],
    ) -> tuple[list[FieldDecl], dict[str, int], list[str]]:
        output: list[FieldDecl] = []
        offsets: dict[str, int] = {}
        evidence: list[str] = []
        shift = 0
        for field in fields:
            adjusted_offset = (
                field.offset + shift if field.offset is not None else None
            )
            descriptor = (field.source_type or "").lstrip("+-")
            if descriptor not in ("I", "J"):
                output.append(replace(field, offset=adjusted_offset))
                continue
            serializer_width = self._serializer_primitive_width(
                serializer, field
            )
            parser_spans = {
                span
                for parser in parsers
                if (span := self._parser_primitive_span(parser, field))
                is not None
            }
            if len(parser_spans) > 1:
                raise ExtractionError(
                    f"contradictory parser spans for "
                    f"{class_name}.{field.name}: "
                    f"{sorted(parser_spans)}"
                )
            parser_span = next(iter(parser_spans), None)
            parser_width = parser_span[1] if parser_span is not None else None
            widths = {
                width
                for width in (serializer_width, parser_width)
                if width is not None
            }
            if len(widths) > 1:
                raise ExtractionError(
                    f"serializer/parser width mismatch for "
                    f"{class_name}.{field.name}: "
                    f"serializer={serializer_width}, parser={parser_width}"
                )
            if not widths:
                output.append(replace(field, offset=adjusted_offset))
                continue
            width = next(iter(widths))
            sized = self._sized_primitive(descriptor, width)
            if sized is None:
                raise ExtractionError(
                    f"unsupported inferred width {width} for "
                    f"{class_name}.{field.name} ({descriptor})"
                )
            if sized == field.cpp_type:
                output.append(replace(field, offset=adjusted_offset))
                continue
            # Nested parsers receive a child slice after its discriminator,
            # so parser indices prove width but are not always absolute wire
            # offsets. Preserve the declaration's accumulated offset.
            start = adjusted_offset
            old_size = _FIXED_SIZES.get(field.cpp_type)
            if old_size is None:
                output.append(replace(field, offset=start))
                continue
            output.append(
                replace(
                    field,
                    cpp_type=sized,
                    wire_kind="pod",
                    offset=start,
                )
            )
            if start is not None:
                offsets[field.name] = start
            shift += width - old_size
            if parser_span is not None:
                evidence.append(
                    f"parser reads {field.name} from local bytes "
                    f"{parser_span[0]}.."
                    f"{parser_span[0] + parser_span[1] - 1}"
                )
            if serializer_width is not None:
                evidence.append(
                    f"serializer writes {serializer_width} byte(s) "
                    f"for {field.name}"
                )
        return output, offsets, evidence

    def _fixed_count_from_getter(self, getter: JavaMethod) -> int | None:
        text = self._decompile_text(getter)
        if '"size"' not in text and " size" not in text:
            return None
        counts = {
            int(value)
            for value in re.findall(
                r"\b[A-Za-z_][A-Za-z0-9_]*\s*!=\s*(\d+)", text
            )
            if int(value) > 1
        }
        return next(iter(counts)) if len(counts) == 1 else None

    @staticmethod
    def _sized_primitive(descriptor: str, width: int) -> str | None:
        if descriptor == "I":
            return {
                1: "UInt8",
                2: "Int16BE",
                3: "Int24BE",
                4: "Int32BE",
            }.get(width)
        if descriptor == "J":
            return {4: "Int32BE", 8: "Int64BE"}.get(width)
        return None

    def _static_field_evidence(
        self, class_name: str, fields: list[FieldDecl]
    ) -> tuple[list[FieldDecl], dict[str, int], list[str]]:
        constants = self.database.static_integer_constants(class_name)
        if not constants:
            return fields, {}, []
        by_name = {constant.name: constant for constant in constants}
        output: list[FieldDecl] = []
        offsets: dict[str, int] = {}
        evidence: list[str] = []
        for field in fields:
            suffix = "_".join(self._camel_tokens(field.name))
            index_constant = by_name.get(f"INDEX_{suffix}")
            size_constant = by_name.get(f"BYTE_SIZE_{suffix}")
            updated = field
            if index_constant is not None:
                offsets[field.name] = index_constant.value
                evidence.append(
                    f"0x{index_constant.metadata_address:X} "
                    f"{class_name}.{index_constant.name}="
                    f"{index_constant.value}"
                )
            if size_constant is not None and field.source_type is not None:
                descriptor = field.source_type.lstrip("+-")
                sized = self._sized_primitive(
                    descriptor, size_constant.value
                )
                if sized is None:
                    raise ExtractionError(
                        f"unsupported static width {size_constant.value} for "
                        f"{class_name}.{field.name} ({descriptor})"
                    )
                updated = replace(
                    field, cpp_type=sized, wire_kind="pod"
                )
                evidence.append(
                    f"0x{size_constant.metadata_address:X} "
                    f"{class_name}.{size_constant.name}="
                    f"{size_constant.value}"
                )
            output.append(updated)
        return output, offsets, evidence

    def is_wire_enum(self, class_name: str) -> bool:
        selectors = {
            (method.kind, method.selector)
            for method in self.database.methods(class_name)
        }
        return (
            ("-", "byteCode") in selectors
            or ("-", "getByteCode") in selectors
        ) and ("+", "initialize") in selectors

    def extract_enum(
        self, class_name: str, cpp_name: str | None = None
    ) -> EnumDecl:
        if cpp_name is None:
            cpp_name = self._clean_type_name(class_name)
        key = (class_name, cpp_name)
        if key in self._enum_cache:
            return self._enum_cache[key]
        if not self.is_wire_enum(class_name):
            raise ExtractionError(f"{class_name} is not a byteCode enum")
        initializer = self.database.method(class_name, "initialize", "+")
        function = self.database.decompile(initializer)
        visitor = _EnumVisitor()
        visitor.apply_to(function.body, None)
        values = sorted(visitor.values, key=lambda item: item.source_address)
        if not values:
            values = _enum_values_from_assembly(initializer)
            values.sort(key=lambda item: item.source_address)
        if not values:
            raise ExtractionError(f"no enum values recovered from {class_name}")
        duplicates = [
            name
            for name, count in Counter(item.name for item in values).items()
            if count != 1
        ]
        if duplicates:
            raise ExtractionError(
                f"duplicate enum members in {class_name}: {duplicates}"
            )
        byte_code_method = (
            self.database.find_methods(
                class_name, selector_prefix="byteCode", kind="-"
            )
            or self.database.find_methods(
                class_name, selector_prefix="getByteCode", kind="-"
            )
        )
        if len(byte_code_method) != 1:
            raise ExtractionError(
                f"expected one byte-code accessor for {class_name}"
            )
        declaration = EnumDecl(
            objc_name=class_name,
            cpp_name=cpp_name,
            values=tuple(values),
            sources=(
                SourceRef(initializer.address, initializer.symbol),
                SourceRef(
                    byte_code_method[0].address,
                    byte_code_method[0].symbol,
                ),
            ),
        )
        self._enum_cache[key] = declaration
        return declaration

    def _returned_object_address(self, method: MethodSymbol) -> int:
        function = self.database.decompile(method)
        visitor = _ObjectReferenceVisitor()
        visitor.apply_to(function.body, None)
        addresses = sorted(set(visitor.addresses))
        if len(addresses) != 1:
            raise ExtractionError(
                f"expected one returned object in {method.symbol}, "
                f"found {[hex(address) for address in addresses]}"
            )
        return addresses[0]

    def extract_v2_function_type(self, table: int) -> EnumDecl:
        class_name = (
            "ComSonySongpalTandemfamilyMessageMdrV2FunctionType"
        )
        table_class = class_name + "_Table"
        initializer = self.database.method(class_name, "initialize", "+")
        byte_code = self.database.method(class_name, "byteCode")
        table_number = self.database.method(class_name, "getTableNumber")
        table_methods = {
            number: self.database.method(
                table_class, f"NO_{number}", "+"
            )
            for number in (1, 2)
        }
        table_addresses = {
            number: self._returned_object_address(method)
            for number, method in table_methods.items()
        }
        function = self.database.decompile(initializer)
        visitor = _V2FunctionTypeVisitor(table_addresses)
        visitor.apply_to(function.body, None)
        values = visitor.values.get(table, [])
        if not values:
            raise ExtractionError(
                f"no V2 function types recovered for table {table}"
            )
        duplicates = [
            name
            for name, count in Counter(
                item.name for item in values
            ).items()
            if count != 1
        ]
        if duplicates:
            raise ExtractionError(
                f"duplicate V2 table {table} function types: {duplicates}"
            )
        return EnumDecl(
            objc_name=class_name,
            cpp_name=self.metadata(class_name).java_name,
            values=tuple(values),
            sources=(
                SourceRef(initializer.address, initializer.symbol),
                SourceRef(byte_code.address, byte_code.symbol),
                SourceRef(table_number.address, table_number.symbol),
                SourceRef(
                    table_methods[table].address,
                    table_methods[table].symbol,
                ),
            ),
        )

    def extract_table(
        self, version: int, table: int, fingerprint: str
    ) -> TableIR:
        self._current_coordinates = (version, table)
        prefix = f"THMSGV{version}T{table}"
        classes = sorted(
            {
                class_name
                for class_name in self.database.protocol_classes()
                if class_name.startswith(prefix)
            }
        )
        result = TableIR(version, table, fingerprint)
        enum_classes = [
            class_name
            for class_name in classes
            if not class_name.endswith("_Factory")
            and self.is_wire_enum(class_name)
        ]
        for class_name in enum_classes:
            enum = self.extract_enum(class_name)
            result.enums.append(enum)
            result.candidates.append(
                Candidate(
                    class_name,
                    "enum",
                    "byteCode accessor and translated enum initializer",
                    tuple(source.address for source in enum.sources),
                )
            )
        if version == 2:
            function_type = self.extract_v2_function_type(table)
            result.enums.append(function_type)
            result.candidates.append(
                Candidate(
                    function_type.objc_name,
                    "enum",
                    "shared table-number and byteCode enum initializer",
                    tuple(
                        source.address for source in function_type.sources
                    ),
                )
            )

        command_class = f"{prefix}Command"
        command_enum = next(
            (
                item
                for item in result.enums
                if item.objc_name == command_class
            ),
            None,
        )
        if command_enum is None:
            raise ExtractionError(f"missing command enum {command_class}")

        referenced_shared_enums: set[str] = set()
        handled: set[str] = set(enum_classes)
        for class_name in classes:
            if class_name in handled:
                continue
            is_factory = (
                class_name.endswith("_Factory")
                or "_Factory_" in class_name
                or (
                class_name.endswith("Factory")
                and bool(
                    self.database.find_methods(
                        class_name, selector_prefix="parseBytesWith"
                    )
                )
                )
            )
            if is_factory:
                parent = class_name.split("_Factory", 1)[0]
                parent = parent.removesuffix("Factory")
                result.candidates.append(
                    Candidate(
                        class_name,
                        "alias",
                        f"serializer/parser factory for {parent}",
                        tuple(
                            method.address
                            for method in self.database.methods(class_name)
                        ),
                    )
                )
                handled.add(class_name)
                continue
            if self._is_abstract_factory_base(class_name):
                result.candidates.append(
                    Candidate(
                        class_name,
                        "unsupported",
                        "generic factory-injected dispatch base",
                        tuple(
                            method.address
                            for method in self.database.methods(class_name)
                        ),
                    )
                )
                handled.add(class_name)
                continue
            metadata = self.metadata(class_name)
            is_wire_class = any(
                method.java_name == "getCommandStream"
                or method.java_name.startswith(
                    ("restore", "parse", "writeTo", "createWithPayload")
                )
                for method in metadata.methods
            ) or bool(self.database.methods(class_name + "_Factory"))
            if (
                not is_wire_class
                and class_name.removeprefix(prefix).endswith(
                    ("StringCreator", "HandleCommand")
                )
            ):
                result.candidates.append(
                    Candidate(
                        class_name,
                        "alias",
                        "non-wire protocol utility or marker class",
                        tuple(
                            method.address
                            for method in self.database.methods(class_name)
                        ),
                    )
                )
                handled.add(class_name)
                continue
            extracted_payloads: list[PayloadDecl] = []
            references: set[str] = set()
            constructor_variants = self._constructor_variants(class_name)
            if len(constructor_variants) > 1:
                _, _, simple_name = protocol_coordinates(class_name)
                for constructor in constructor_variants:
                    descriptors = parse_descriptors(
                        constructor.parameter_types or ""
                    )
                    last_descriptor = descriptors[-1]
                    suffix = (
                        self._clean_type_name(
                            self._descriptor_simple_name(last_descriptor)
                        )
                        if last_descriptor.startswith("L")
                        else f"Variant{len(extracted_payloads) + 1}"
                    )
                    payload, payload_references = self.extract_payload(
                        class_name,
                        command_enum,
                        constructor_override=constructor,
                        cpp_name_override=simple_name + suffix,
                    )
                    extracted_payloads.append(payload)
                    references.update(payload_references)
            else:
                payload, references = self.extract_payload(
                    class_name,
                    command_enum,
                    constructor_override=(
                        constructor_variants[0]
                        if constructor_variants
                        else None
                    ),
                )
                extracted_payloads.append(payload)
            extracted_payloads = [
                payload
                for payload in extracted_payloads
                if payload.command is not None or payload.fields
            ]
            if not extracted_payloads:
                result.candidates.append(
                    Candidate(
                        class_name,
                        "alias",
                        "non-wire class with no recoverable serialized fields",
                        tuple(
                            method.address
                            for method in self.database.methods(class_name)
                        ),
                    )
                )
                handled.add(class_name)
                continue
            referenced_shared_enums.update(references)
            result.payloads.extend(extracted_payloads)
            payload = extracted_payloads[0]
            result.candidates.append(
                Candidate(
                    class_name,
                    payload.classification,
                    (
                        "command-bearing serialized payload"
                        if payload.classification in ("payload", "nested_variant")
                        else "serialized field helper"
                    ),
                    tuple(source.address for source in payload.sources),
                )
            )
            handled.add(class_name)

        for class_name in sorted(referenced_shared_enums):
            if class_name.startswith(prefix):
                continue
            if not self.is_wire_enum(class_name):
                raise ExtractionError(
                    f"referenced type {class_name} is not a recoverable enum"
                )
            result.enums.append(self.extract_enum(class_name))

        missing = set(classes) - handled
        if missing:
            raise ExtractionError(
                f"unclassified {prefix} candidates: {sorted(missing)}"
            )
        result.payloads = self._infer_discriminator_defaults(
            result.payloads, result.enums
        )
        result.payloads = self._infer_missing_discriminators(
            result.payloads, result.enums, version
        )
        result.payloads = self._inherit_family_layouts(result.payloads)
        result.payloads = self._infer_discriminator_defaults(
            result.payloads, result.enums
        )
        result.payloads = self._split_contextual_field_helpers(
            result.payloads, result.enums
        )
        result.payloads = self._link_nested_variants(result.payloads)
        unknown_types = self._unknown_field_types(
            result.payloads, {enum.cpp_name for enum in result.enums}
        )
        allocation_dispatch = {
            payload.objc_name: self._parser_allocation_targets(payload)
            for payload in result.payloads
            if any(
                field.cpp_type in unknown_types for field in payload.fields
            )
        }
        result.payloads = self._expand_polymorphic_payloads(
            result.payloads, result.enums, allocation_dispatch
        )
        result.payloads = self._deduplicate_constructor_variants(
            result.payloads
        )
        result.payloads = self._correct_container_kinds(result.payloads)
        self._fail_on_unresolved_types(result)
        return result

    def _parser_allocation_targets(
        self, payload: PayloadDecl
    ) -> frozenset[str]:
        methods = [
            method
            for owner in (payload.objc_name, payload.objc_name + "_Factory")
            for method in self.database.methods(owner)
            if method.selector.startswith(("restore", "parseBytes"))
        ]
        targets: set[str] = set()
        for method in methods:
            try:
                function = self.database.decompile(method)
            except ExtractionError:
                continue
            visitor = _EvidenceVisitor()
            visitor.apply_to(function.body, None)
            for _, symbol in visitor.calls:
                address = ida_name.get_name_ea(idaapi.BADADDR, symbol)
                if (
                    address == idaapi.BADADDR
                    or ida_funcs.get_func(address) is None
                    or address == method.address
                ):
                    continue
                try:
                    target = self.database.allocation_target(
                        MethodSymbol(address, "", "", symbol, symbol)
                    )
                except ExtractionError:
                    continue
                if target.startswith("THMSGV"):
                    targets.add(target)
        return frozenset(targets)

    @classmethod
    def _inherit_family_layouts(
        cls, payloads: list[PayloadDecl]
    ) -> list[PayloadDecl]:
        """Apply a serialized base factory's field layout to concrete siblings."""
        output: list[PayloadDecl] = []
        for payload in payloads:
            bases = [
                candidate
                for candidate in payloads
                if candidate.objc_name != payload.objc_name
                and candidate.command == payload.command
                and candidate.fields
                and any(
                    evidence.startswith(
                        f"-[{candidate.objc_name}_Factory toStream"
                    )
                    for evidence in payload.evidence
                )
            ]
            if not bases:
                output.append(payload)
                continue
            base = max(bases, key=lambda item: len(item.objc_name))
            base_fields = [
                field for field in base.fields if field.name != "command"
            ]
            fields = [
                field for field in payload.fields if field.name != "command"
            ]
            if not base_fields or not fields:
                output.append(payload)
                continue
            matched: set[int] = set()
            changed = False
            for base_index, base_field in enumerate(base_fields):
                named_match = next(
                    (
                        index
                        for index, field in enumerate(fields)
                        if index not in matched
                        and field.name == base_field.name
                    ),
                    None,
                )
                covariant_match = next(
                    (
                        index
                        for index, field in enumerate(fields)
                        if index not in matched
                        and cls._covariant_cpp_type(
                            base_field.cpp_type, field.cpp_type
                        )
                    ),
                    None,
                )
                source_match = next(
                    (
                        index
                        for index, field in enumerate(fields)
                        if index not in matched
                        and cls._descriptor_identity(field.source_type)
                        == cls._descriptor_identity(base_field.source_type)
                    ),
                    None,
                )
                type_match = next(
                    (
                        index
                        for index, field in enumerate(fields)
                        if index not in matched
                        and field.cpp_type == base_field.cpp_type
                    ),
                    None,
                )
                existing = (
                    named_match
                    if named_match is not None
                    else covariant_match
                    if covariant_match is not None
                    else source_match
                    if source_match is not None
                    else type_match
                )
                if existing is not None:
                    field = fields[existing]
                    if (
                        covariant_match is not None
                        and existing == covariant_match
                        and named_match is None
                    ):
                        matched.add(existing)
                        continue
                    preserve_discriminator = (
                        field.name == base_field.name
                        and field.name in ("type", "dataType")
                    )
                    inherited = replace(
                        base_field,
                        offset=field.offset,
                        default=(
                            field.default
                            if preserve_discriminator
                            else base_field.default
                        ),
                        semantic_rules=(
                            field.semantic_rules
                            if preserve_discriminator
                            else base_field.semantic_rules
                        ),
                    )
                    fields[existing] = inherited
                    matched.add(existing)
                    changed = changed or field != inherited
                    continue
                insert_at = min(base_index, len(fields))
                fields.insert(insert_at, base_field)
                matched = {
                    index + 1 if index >= insert_at else index
                    for index in matched
                }
                matched.add(insert_at)
                changed = True
            if not changed:
                output.append(payload)
                continue
            command_field = next(
                (
                    field
                    for field in payload.fields
                    if field.name == "command"
                ),
                None,
            )
            combined = [*([command_field] if command_field else []), *fields]
            output.append(
                replace(
                    payload,
                    fields=tuple(cls._recalculate_offsets(combined)),
                    serialization=(
                        "external"
                        if any(
                            field.wire_kind in ("array", "string", "helper")
                            for field in combined
                        )
                        else "trivial"
                    ),
                    evidence=(
                        *payload.evidence,
                        f"inherited wire layout from {base.objc_name}",
                    ),
                )
            )
        return [
            cls._normalize_safe_listening_variant(
                cls._name_battery_threshold_fields(payload)
            )
            for payload in output
        ]

    @staticmethod
    def _covariant_cpp_type(base: str, derived: str) -> bool:
        if base == derived:
            return False
        for wrapper in ("MDRArray", "MDRPodArray"):
            prefix = f"{wrapper}<"
            if base.startswith(prefix) and derived.startswith(prefix):
                base_inner = base[len(prefix) : -1]
                derived_inner = derived[len(prefix) : -1]
                return derived_inner.startswith(base_inner)
        return derived.startswith(base)

    @staticmethod
    def _normalize_safe_listening_variant(
        payload: PayloadDecl,
    ) -> PayloadDecl:
        if not payload.cpp_name.startswith("SafeListening"):
            return payload
        discriminator = next(
            (
                (suffix, member)
                for suffix, member in (
                    ("Hbs1", "SAFE_LISTENING_HBS_1"),
                    ("Hbs2", "SAFE_LISTENING_HBS_2"),
                    ("Tws1", "SAFE_LISTENING_TWS_1"),
                    ("Tws2", "SAFE_LISTENING_TWS_2"),
                )
                if payload.cpp_name.endswith(suffix)
            ),
            None,
        )
        fields = [
            replace(
                field,
                name="inquiredType",
                default=(
                    f"SafeListeningInquiredType::{discriminator[1]}"
                    if discriminator is not None
                    else field.default
                ),
            )
            if field.cpp_type == "SafeListeningInquiredType"
            else field
            for field in payload.fields
        ]
        return replace(payload, fields=tuple(fields))

    @staticmethod
    def _name_battery_threshold_fields(
        payload: PayloadDecl,
    ) -> PayloadDecl:
        if "BatteryThreshold" not in payload.cpp_name:
            return payload
        numeric = [
            index
            for index, field in enumerate(payload.fields)
            if field.cpp_type == "UInt8"
        ]
        expected = (
            (
                "leftBatteryLevel",
                "rightBatteryLevel",
                "leftBatteryThreshold",
                "rightBatteryThreshold",
            )
            if "LeftRight" in payload.cpp_name
            else (
                ("batteryLevel", "batteryThreshold")
                if payload.cpp_name.endswith("Base")
                else ("value1", "batteryThreshold")
            )
        )
        if len(numeric) != len(expected):
            return payload
        fields = list(payload.fields)
        for index, name in zip(numeric, expected):
            fields[index] = replace(fields[index], name=name)
        return replace(payload, fields=tuple(fields))

    @staticmethod
    def _descriptor_identity(descriptor: str | None) -> str | None:
        if descriptor is None:
            return None
        descriptor = descriptor.lstrip("+-")
        if not descriptor.startswith("L") or not descriptor.endswith(";"):
            return descriptor
        return descriptor[1:-1].rsplit("/", 1)[-1]

    def _descriptors_match(self, left: str | None, right: str) -> bool:
        if left == right:
            return True
        if (
            left is None
            or not left.startswith("L")
            or not right.startswith("L")
        ):
            return False
        left_class = self.database.objc_class_for_descriptor_if_present(left)
        right_class = self.database.objc_class_for_descriptor_if_present(right)
        if (
            left_class is not None
            and right_class is not None
            and left_class == right_class
        ):
            return True

        def flattened(descriptor: str) -> str:
            body = descriptor[1:-1]
            return "".join(
                part[:1].upper() + part[1:]
                for part in body.split("/")
            ).replace("$", "_")

        return flattened(left) == flattened(right)

    def _is_abstract_factory_base(self, class_name: str) -> bool:
        """Identify erased-generic wire facades that require an injected factory."""
        factory_class = class_name + "_Factory"
        factory_methods = self.database.methods(factory_class)
        if not factory_methods:
            return False
        metadata = self.metadata(class_name)
        has_erased_type = any(
            "TT;" in (method.generic_signature or "")
            for method in metadata.methods
        )
        if not has_erased_type:
            return False
        factory_metadata = self.metadata(factory_class)
        has_injected_factory = any(
            method.java_name == "<init>"
            and any(
                descriptor.endswith("Factory;")
                for descriptor in parse_descriptors(
                    method.parameter_types or ""
                )
            )
            for method in factory_metadata.methods
        )
        if not has_injected_factory:
            raise ExtractionError(
                f"{class_name} has erased generic wire fields without an "
                "injected concrete factory"
            )
        return True

    @staticmethod
    def _correct_container_kinds(
        payloads: list[PayloadDecl],
    ) -> list[PayloadDecl]:
        nontrivial = {
            payload.cpp_name
            for payload in payloads
            if payload.serialization != "trivial"
        }
        output: list[PayloadDecl] = []
        for payload in payloads:
            if (
                payload.parent in nontrivial
                and all(
                    field.name
                    in {"command", payload.discriminator_field}
                    for field in payload.fields
                )
            ):
                continue
            changed = (
                payload.parent is not None
                and payload.parent in nontrivial
            )
            fields: list[FieldDecl] = []
            for field in payload.fields:
                match = re.fullmatch(
                    r"MDRPodArray<([A-Za-z_][A-Za-z0-9_]*)>",
                    field.cpp_type,
                )
                if match and match.group(1) in nontrivial:
                    fields.append(
                        replace(
                            field,
                            cpp_type=(
                                f"MDRArray<{match.group(1)}>"
                            ),
                            wire_kind="array",
                        )
                    )
                    changed = True
                else:
                    fields.append(field)
            output.append(
                replace(
                    payload,
                    fields=tuple(fields),
                    serialization=(
                        "external" if changed else payload.serialization
                    ),
                )
            )
        return output

    def _split_contextual_field_helpers(
        self,
        payloads: list[PayloadDecl],
        enums: list[EnumDecl],
    ) -> list[PayloadDecl]:
        """Split helpers when a parent serializer omits logical fields.

        J2ObjC factory signatures describe the Java object model rather than
        necessarily describing every byte written for that object.  Some
        parents therefore accept the same helper class but serialize only a
        subset of its getters.  Preserve the complete helper as the canonical
        layout and create a wire-specific helper for each proven subset.
        """
        helpers = {
            payload.cpp_name: payload
            for payload in payloads
            if payload.classification == "field_helper"
        }
        contexts: dict[
            tuple[str, str],
            tuple[tuple[str, ...], MethodSymbol],
        ] = {}
        layouts: dict[str, set[tuple[str, ...]]] = {}
        for payload in payloads:
            if payload.classification == "field_helper":
                continue
            for field in payload.fields:
                match = re.fullmatch(
                    r"MDR(?:Pod)?Array<([A-Za-z_][A-Za-z0-9_]*)>",
                    field.cpp_type,
                )
                helper = helpers.get(match.group(1)) if match else None
                if helper is None:
                    continue
                subset = self._serialized_helper_field_subset(payload, helper)
                if subset is None:
                    continue
                contexts[(payload.cpp_name, field.name)] = subset
                layouts.setdefault(helper.cpp_name, set()).add(subset[0])

        existing_names = {payload.cpp_name for payload in payloads}
        variants: dict[
            tuple[str, tuple[str, ...]], PayloadDecl
        ] = {}
        output: list[PayloadDecl] = []

        for payload in payloads:
            if payload.classification == "field_helper":
                output.append(payload)
                continue

            fields: list[FieldDecl] = []
            contextual_evidence: list[str] = []
            contextual_family = False
            for field in payload.fields:
                match = re.fullmatch(
                    r"(MDR(?:Pod)?Array)<([A-Za-z_][A-Za-z0-9_]*)>",
                    field.cpp_type,
                )
                helper = helpers.get(match.group(2)) if match else None
                if helper is None:
                    fields.append(field)
                    continue

                subset = contexts.get((payload.cpp_name, field.name))
                if (
                    subset is None
                    or len(layouts.get(helper.cpp_name, ())) < 2
                ):
                    fields.append(field)
                    continue
                contextual_family = True
                selected_names, serializer = subset
                if len(selected_names) == len(helper.fields):
                    fields.append(field)
                    continue

                selected = [
                    helper_field
                    for helper_field in helper.fields
                    if helper_field.name in selected_names
                ]
                omitted = [
                    helper_field
                    for helper_field in helper.fields
                    if helper_field.name not in selected_names
                ]
                omitted_suffix = "And".join(
                    item.name[:1].upper() + item.name[1:]
                    for item in omitted
                )
                variant_name = (
                    f"{helper.cpp_name}Without{omitted_suffix}"
                )
                key = (helper.cpp_name, selected_names)
                source = SourceRef(serializer.address, serializer.symbol)
                evidence = (
                    f"{serializer.symbol} serializes "
                    f"{', '.join(selected_names)} and omits "
                    f"{', '.join(item.name for item in omitted)}"
                )
                variant = variants.get(key)
                if variant is None:
                    if variant_name in existing_names:
                        raise ExtractionError(
                            f"contextual helper name collision: {variant_name}"
                        )
                    variants[key] = replace(
                        helper,
                        cpp_name=variant_name,
                        fields=tuple(self._recalculate_offsets(selected)),
                        sources=tuple(dict.fromkeys((*helper.sources, source))),
                        evidence=tuple(
                            dict.fromkeys((*helper.evidence, evidence))
                        ),
                    )
                    existing_names.add(variant_name)
                else:
                    variants[key] = replace(
                        variant,
                        sources=tuple(
                            dict.fromkeys((*variant.sources, source))
                        ),
                        evidence=tuple(
                            dict.fromkeys((*variant.evidence, evidence))
                        ),
                    )

                fields.append(
                    replace(
                        field,
                        cpp_type=f"{match.group(1)}<{variant_name}>",
                    )
                )
                contextual_evidence.append(
                    f"{field.name} uses contextual wire helper "
                    f"{variant_name}"
                )

            updated = replace(
                payload,
                fields=tuple(fields),
                evidence=tuple(
                    dict.fromkeys(
                        (*payload.evidence, *contextual_evidence)
                    )
                ),
            )
            if contextual_family:
                updated = self._correct_contextual_discriminator(
                    updated, enums
                )
            output.append(updated)

        output.extend(variants.values())
        return output

    @classmethod
    def _correct_contextual_discriminator(
        cls,
        payload: PayloadDecl,
        enums: list[EnumDecl],
    ) -> PayloadDecl:
        enum_names = {enum.cpp_name for enum in enums}
        reset_fields = tuple(
            replace(
                field,
                default=f"{field.cpp_type}::OUT_OF_RANGE",
                semantic_rules=(
                    f"EnumRange {field.cpp_type}::OUT_OF_RANGE",
                ),
            )
            if (
                field.name in {"inquiredType", "type"}
                and field.cpp_type in enum_names
            )
            else field
            for field in payload.fields
        )
        inferred = cls._infer_discriminator_defaults(
            [replace(payload, fields=reset_fields)], enums
        )[0]
        fields = tuple(
            original
            if candidate.default == original.default
            else candidate
            for original, candidate in zip(
                payload.fields, inferred.fields
            )
        )
        return replace(payload, fields=fields)

    def _serialized_helper_field_subset(
        self,
        payload: PayloadDecl,
        helper: PayloadDecl,
    ) -> tuple[tuple[str, ...], MethodSymbol] | None:
        serializer_sources = [
            source
            for source in payload.sources
            if re.search(
                r" (?:valueOf|toStream|getCommandStream)[^]]*\]$",
                source.symbol,
            )
        ]
        if len(serializer_sources) != 1:
            return None
        source = serializer_sources[0]
        serializer = MethodSymbol(
            source.address,
            "",
            payload.objc_name,
            "",
            source.symbol,
        )
        try:
            text = self._expanded_method_text(serializer)
        except ExtractionError:
            return None

        positions = {
            field.name: text.find(
                f'"get{field.name[:1].upper()}{field.name[1:]}"'
            )
            for field in helper.fields
        }
        present = {
            name: position
            for name, position in positions.items()
            if position >= 0
        }
        # One getter is not enough to prove that an object is serialized
        # field-by-field; it may only be incidental validation or logging.
        if len(present) < 2:
            return None
        observed = tuple(
            name
            for name, _ in sorted(
                present.items(), key=lambda item: item[1]
            )
        )
        canonical = tuple(
            field.name
            for field in helper.fields
            if field.name in present
        )
        if observed != canonical:
            raise ExtractionError(
                f"contextual serializer field order for {payload.objc_name} "
                f"does not match {helper.objc_name}: {observed}"
            )
        return observed, serializer

    @classmethod
    def _infer_missing_discriminators(
        cls,
        payloads: list[PayloadDecl],
        enums: list[EnumDecl],
        version: int,
    ) -> list[PayloadDecl]:
        if version != 2:
            return payloads
        discriminator_enums = [
            enum
            for enum in enums
            if enum.cpp_name.endswith("InquiredType")
            or enum.cpp_name == "DeviceInfoType"
        ]
        output: list[PayloadDecl] = []
        for payload in payloads:
            if payload.command is None or not payload.fields:
                output.append(payload)
                continue
            discriminator_names = {
                enum.cpp_name for enum in discriminator_enums
            }
            if any(
                field.cpp_type in discriminator_names
                for field in payload.fields[1:]
            ):
                output.append(payload)
                continue
            name_tokens = set(cls._camel_tokens(payload.cpp_name))
            candidates: list[tuple[int, str, str]] = []
            for enum in discriminator_enums:
                if any(
                    field.cpp_type == enum.cpp_name
                    for field in payload.fields
                ):
                    continue
                enum_prefix = enum.cpp_name.removesuffix("InquiredType")
                prefix_tokens = set(cls._camel_tokens(enum_prefix))
                if not prefix_tokens or not (
                    prefix_tokens & name_tokens
                ):
                    continue
                valid_values = [
                    value.name
                    for value in enum.values
                    if value.name
                    not in ("NO_USE", "OUT_OF_RANGE", "UNKNOWN")
                ]
                for member in valid_values:
                    member_tokens = set(cls._camel_tokens(member))
                    score = len(member_tokens & name_tokens)
                    if score:
                        candidates.append(
                            (score, enum.cpp_name, member)
                        )
                if len(valid_values) == 1:
                    candidates.append(
                        (1, enum.cpp_name, valid_values[0])
                    )
            if not candidates:
                output.append(payload)
                continue
            best_score = max(score for score, _, _ in candidates)
            best = {
                (enum_name, member)
                for score, enum_name, member in candidates
                if score == best_score
            }
            if len(best) != 1:
                shortest = min(
                    len(member.split("_"))
                    for _, member in best
                )
                best = {
                    (enum_name, member)
                    for enum_name, member in best
                    if len(member.split("_")) == shortest
                }
                if len(best) != 1:
                    output.append(payload)
                    continue
            enum_name, member = next(iter(best))
            default = f"{enum_name}::{member}"
            discriminator = FieldDecl(
                name="type",
                cpp_type=enum_name,
                wire_kind="pod",
                offset=1,
                default=default,
                source_type=f"inferred from {payload.objc_name}",
                semantic_rules=(f"EnumRange {default}",),
            )
            fields = cls._recalculate_offsets(
                [
                    payload.fields[0],
                    discriminator,
                    *payload.fields[1:],
                ]
            )
            output.append(replace(payload, fields=tuple(fields)))
        return output

    @classmethod
    def _infer_discriminator_defaults(
        cls,
        payloads: list[PayloadDecl],
        enums: list[EnumDecl],
    ) -> list[PayloadDecl]:
        enum_values = {
            enum.cpp_name: [
                value.name
                for value in enum.values
                if value.name
                not in ("NO_USE", "OUT_OF_RANGE", "UNKNOWN")
            ]
            for enum in enums
        }
        output: list[PayloadDecl] = []
        for payload in payloads:
            name_tokens = (
                set(cls._camel_tokens(payload.cpp_name))
                - _COMMAND_WORDS
                - {"NOTIFY"}
                - _GENERIC_NAME_TOKENS
            )
            inherited_layout = any(
                evidence.startswith("inherited wire layout from ")
                for evidence in payload.evidence
            )
            fields: list[FieldDecl] = []
            for field in payload.fields:
                if not (
                    field.name
                    in {"type", "dataType", "inquiredType", "command"}
                    or field.name.endswith("Type")
                    or field.name.endswith("InquiredType")
                    or field.cpp_type.endswith("InquiredType")
                ):
                    fields.append(field)
                    continue
                current_member = (
                    field.default.split("::", 1)[1]
                    if field.default and "::" in field.default
                    else None
                )
                current_tokens = (
                    set(cls._camel_tokens(current_member))
                    if current_member is not None
                    else set()
                )
                if (
                    not field.default
                    or (
                        not field.default.endswith(
                            ("::NO_USE", "::OUT_OF_RANGE", "::UNKNOWN")
                        )
                        and not (
                            inherited_layout
                            and current_tokens
                            and not (
                                current_tokens - _GENERIC_NAME_TOKENS
                            )
                            & name_tokens
                        )
                    )
                    or field.cpp_type not in enum_values
                ):
                    fields.append(field)
                    continue
                scores = []
                for member in enum_values[field.cpp_type]:
                    member_tokens = (
                        set(cls._camel_tokens(member)) - _GENERIC_NAME_TOKENS
                    )
                    if not member_tokens or not (
                        member_tokens <= name_tokens
                    ):
                        continue
                    scores.append((len(member_tokens), member))
                if not scores:
                    fields.append(field)
                    continue
                best = max(score for score, _ in scores)
                members = {
                    member for score, member in scores if score == best
                }
                if len(members) != 1:
                    shortest = min(
                        len(member.split("_")) for member in members
                    )
                    members = {
                        member
                        for member in members
                        if len(member.split("_")) == shortest
                    }
                    if len(members) != 1:
                        fields.append(field)
                        continue
                member = next(iter(members))
                default = f"{field.cpp_type}::{member}"
                semantic_rules = field.semantic_rules
                if not semantic_rules or all(
                    rule.startswith("EnumRange ")
                    and rule.endswith(
                        ("::NO_USE", "::OUT_OF_RANGE", "::UNKNOWN")
                    )
                    for rule in semantic_rules
                ):
                    semantic_rules = (f"EnumRange {default}",)
                fields.append(
                    replace(
                        field,
                        default=default,
                        semantic_rules=semantic_rules,
                    )
                )
            output.append(replace(payload, fields=tuple(fields)))
        return output

    @staticmethod
    def _link_nested_variants(
        payloads: list[PayloadDecl],
    ) -> list[PayloadDecl]:
        by_objc = {payload.objc_name: payload for payload in payloads}
        groups: dict[str, list[PayloadDecl]] = {}
        for payload in payloads:
            if "_" not in payload.objc_name:
                continue
            outer = payload.objc_name.split("_", 1)[0]
            groups.setdefault(outer, []).append(payload)
        replacements: dict[str, PayloadDecl] = {}
        for outer, variants in groups.items():
            outer_payload = by_objc.get(outer)
            bases = [
                payload
                for payload in variants
                if payload.objc_name.rsplit("_", 1)[-1].endswith("Base")
                and any(field.name == "type" for field in payload.fields)
            ]
            if outer_payload is not None and any(
                field.name == "type" for field in outer_payload.fields
            ) and not any(
                variant.cpp_name in field.cpp_type
                for variant in variants
                for field in outer_payload.fields
            ):
                base = outer_payload
            elif len(bases) == 1:
                base = bases[0]
            else:
                continue
            for variant in variants:
                if variant.objc_name == base.objc_name or variant.parent:
                    continue
                discriminator = next(
                    (
                        field
                        for field in variant.fields
                        if field.name == "type"
                    ),
                    None,
                )
                if discriminator is None:
                    continue
                replacements[variant.cpp_name] = replace(
                    variant,
                    parent=base.cpp_name,
                    discriminator_field="type",
                    discriminator_value=discriminator.default,
                )
        return [
            replacements.get(payload.cpp_name, payload)
            for payload in payloads
        ]

    @staticmethod
    def _fail_on_unresolved_types(result: TableIR) -> None:
        known = {
            enum.cpp_name for enum in result.enums
        } | {
            payload.cpp_name for payload in result.payloads
        } | {
            "Command",
            "MDRPodArray",
            "MDRArray",
            "MDRFixedArray",
            "MDRPrefixedString",
            "Array",
            "SupportFunction",
            "EnableDisable",
            "OnOffSettingValue",
            "Range",
            "mdr",
            "v2",
            "t1",
            "t2",
            "Preset",
        }
        referenced = {
            name
            for payload in result.payloads
            for field in payload.fields
            for name in re.findall(
                r"[A-Za-z_][A-Za-z0-9_]*", field.cpp_type
            )
            if not name.startswith(("UInt", "Int", "MDR"))
        }
        unresolved = sorted(referenced - known)
        if unresolved:
            raise ExtractionError(
                f"V{result.version}T{result.table} has unresolved polymorphic "
                f"field types: {unresolved}. Add discriminator-aware variant "
                "mappings; opaque byte fallbacks are forbidden."
            )

    @staticmethod
    def _unknown_field_types(
        payloads: list[PayloadDecl], enums: set[str]
    ) -> set[str]:
        known = enums | {payload.cpp_name for payload in payloads} | {
            "Command",
            "MDRPodArray",
            "MDRArray",
            "MDRFixedArray",
            "MDRPrefixedString",
            "Array",
        }
        return {
            name
            for payload in payloads
            for field in payload.fields
            for name in re.findall(
                r"[A-Za-z_][A-Za-z0-9_]*", field.cpp_type
            )
            if not name.startswith(("UInt", "Int", "MDR"))
            and name not in known
        }

    @classmethod
    def _expand_polymorphic_payloads(
        cls,
        payloads: list[PayloadDecl],
        enums: list[EnumDecl],
        allocation_dispatch: dict[str, frozenset[str]],
    ) -> list[PayloadDecl]:
        enum_names = {
            field.cpp_type
            for payload in payloads
            for field in payload.fields
            if field.default
            and "::" in field.default
            and not field.cpp_type.startswith("Command")
        }
        enum_defaults: dict[str, set[str]] = {}
        for enum in enums:
            enum_defaults[enum.cpp_name] = {
                f"{enum.cpp_name}::{value.name}"
                for value in enum.values
                if value.name not in ("NO_USE", "OUT_OF_RANGE", "UNKNOWN")
            }
        for declaration in payloads:
            for field in declaration.fields:
                if (
                    field.default
                    and "::" in field.default
                    and not field.default.endswith(
                        ("::NO_USE", "::OUT_OF_RANGE", "::UNKNOWN")
                    )
                ):
                    enum_defaults.setdefault(field.cpp_type, set()).add(
                        field.default
                    )
        output = list(payloads)
        while True:
            unresolved = cls._unknown_field_types(output, enum_names)
            if not unresolved:
                return output
            changed = False
            expanded: list[PayloadDecl] = []
            for payload in output:
                unresolved_fields = [
                    (index, field)
                    for index, field in enumerate(payload.fields)
                    if field.cpp_type in unresolved
                    and (
                        set(cls._camel_tokens(field.cpp_type))
                        & {"BASE", "DATA", "INFO", "PARAM", "PAYLOAD"}
                        or any(
                            (
                                set(cls._camel_tokens(field.cpp_type))
                                - {"BASE"}
                            )
                            <= set(cls._camel_tokens(candidate.cpp_name))
                            for candidate in output
                        )
                    )
                ]
                if not unresolved_fields:
                    expanded.append(payload)
                    continue
                if len(unresolved_fields) != 1:
                    expanded.append(payload)
                    continue
                field_index, interface_field = unresolved_fields[0]
                tag_fields = [
                    field
                    for field in reversed(payload.fields[:field_index])
                    if field.name != "command"
                    and field.cpp_type in enum_defaults
                ]
                tag_field = tag_fields[0] if tag_fields else None
                cases: list[
                    tuple[PayloadDecl, FieldDecl, FieldDecl | None]
                ] = []
                interface_tokens = set(
                    cls._camel_tokens(interface_field.cpp_type)
                )
                payload_tokens = set(cls._camel_tokens(payload.cpp_name))
                concrete_allocations = allocation_dispatch.get(
                    payload.objc_name, frozenset()
                ) & {candidate.objc_name for candidate in output}
                for candidate in output:
                    if candidate.classification not in (
                        "field_helper",
                        "nested_variant",
                    ):
                        continue
                    if any(
                        field.cpp_type in unresolved
                        for field in candidate.fields
                    ):
                        continue
                    candidate_fields = [
                        field
                        for field in candidate.fields
                        if field.name != "command"
                    ]
                    if not candidate_fields:
                        continue
                    candidate_tag = candidate_fields[0]
                    invalid_candidate_tag = (
                        not candidate_tag.default
                        or "::" not in candidate_tag.default
                        or candidate_tag.default.endswith(
                            ("::OUT_OF_RANGE", "::NO_USE", "::UNKNOWN")
                        )
                    )
                    exact_tag = (
                        not invalid_candidate_tag
                        and tag_field is not None
                        and
                        candidate_tag.cpp_type == tag_field.cpp_type
                    )
                    if tag_field is None and not (
                        interface_tokens
                        & set(cls._camel_tokens(candidate_tag.cpp_type))
                    ):
                        continue
                    if (
                        "ExtendedInfo" in interface_field.cpp_type
                        and "ExtendedInfo" not in candidate.cpp_name
                    ):
                        continue
                    if (
                        "ExtendedInfo" not in interface_field.cpp_type
                        and interface_field.cpp_type.endswith("Param")
                        and "ExtendedInfo" in candidate.cpp_name
                    ):
                        continue
                    if exact_tag:
                        cases.append((candidate, candidate_tag, candidate_tag))
                        continue
                    if (
                        tag_field is None
                        and not invalid_candidate_tag
                    ):
                        cases.append((candidate, candidate_tag, None))
                        continue
                    candidate_tokens = set(
                        cls._camel_tokens(candidate.cpp_name)
                    )
                    expected_kind = (
                        {"DATA"}
                        if "DATA" in interface_tokens
                        else payload_tokens
                        & {"PARAM", "STATUS", "CAPABILITY", "INFO"}
                    )
                    if "VALUE" in candidate_tokens:
                        expected_kind = set()
                    if expected_kind and not (
                        expected_kind & candidate_tokens
                    ):
                        continue
                    if ("EX" in payload_tokens) != ("EX" in candidate_tokens):
                        continue
                    inferred_tags: list[tuple[FieldDecl, str]] = []
                    for possible_tag in tag_fields:
                        for default in enum_defaults.get(
                            possible_tag.cpp_type, ()
                        ):
                            member = default.split("::", 1)[1]
                            member_tokens = set(cls._camel_tokens(member))
                            if member_tokens and member_tokens <= candidate_tokens:
                                inferred_tags.append((possible_tag, default))
                    unique_tags = {
                        (field.name, default): (field, default)
                        for field, default in inferred_tags
                    }
                    if len(unique_tags) != 1:
                        continue
                    tag_field, default = next(iter(unique_tags.values()))
                    cases.append(
                        (
                            candidate,
                            replace(candidate_tag, default=default),
                            (
                                candidate_tag
                                if candidate_tag.cpp_type
                                == tag_field.cpp_type
                                else None
                            ),
                        )
                    )
                unique_cases = {
                    (candidate.cpp_name, tag.default): (
                        candidate,
                        tag,
                        embedded_tag,
                    )
                    for candidate, tag, embedded_tag in cases
                }
                cases = [
                    unique_cases[key] for key in sorted(unique_cases)
                ]
                if not cases:
                    expanded.append(payload)
                    continue
                for candidate, candidate_tag, embedded_tag in cases:
                    prefix_fields = list(payload.fields[:field_index])
                    if tag_field is not None:
                        prefix_fields = [
                            replace(
                                field,
                                default=candidate_tag.default,
                                semantic_rules=(
                                    f"EnumRange {candidate_tag.default}",
                                ),
                            )
                            if field is tag_field
                            else field
                            for field in prefix_fields
                        ]
                    suffix_fields = [
                        field
                        for field in candidate.fields
                        if field.name != "command"
                        and (
                            embedded_tag is None
                            or field is not embedded_tag
                        )
                    ]
                    combined = cls._recalculate_offsets(
                        [*prefix_fields, *suffix_fields]
                    )
                    case_suffix = candidate.cpp_name
                    common_prefix = cls._common_name_prefix(
                        payload.cpp_name, case_suffix
                    )
                    if common_prefix:
                        case_suffix = case_suffix[len(common_prefix) :]
                    if not case_suffix:
                        case_suffix = candidate.cpp_name
                    expanded.append(
                        replace(
                            payload,
                            cpp_name=payload.cpp_name + case_suffix,
                            fields=tuple(combined),
                            serialization=(
                                "external"
                                if any(
                                    field.wire_kind
                                    in ("array", "string", "helper")
                                    for field in combined
                                )
                                else "trivial"
                            ),
                            evidence=(
                                *payload.evidence,
                                "discriminator-aware expansion of "
                                f"{interface_field.cpp_type} to "
                                f"{candidate.cpp_name}",
                                *(
                                    (
                                        "parser helper allocation targets "
                                        f"{candidate.objc_name}",
                                    )
                                    if candidate.objc_name
                                    in concrete_allocations
                                    else ()
                                ),
                            ),
                        )
                    )
                changed = True
            output = expanded
            if not changed:
                return output

    @staticmethod
    def _common_name_prefix(left: str, right: str) -> str:
        length = 0
        for left_char, right_char in zip(left, right):
            if left_char != right_char:
                break
            length += 1
        return left[:length]

    @staticmethod
    def _deduplicate_constructor_variants(
        payloads: list[PayloadDecl],
    ) -> list[PayloadDecl]:
        """Prefer the most complete flattening of constructor/parser variants."""
        selected = {id(payload) for payload in payloads}
        replacements: dict[int, PayloadDecl] = {}

        def select_winner(
            variants: list[PayloadDecl], canonical_name: str | None = None
        ) -> None:
            winner = max(
                variants,
                key=lambda payload: (
                    len(payload.fields),
                    -len(payload.cpp_name),
                    payload.cpp_name,
                ),
            )
            selected.difference_update(id(payload) for payload in variants)
            selected.add(id(winner))
            replacements[id(winner)] = replace(
                winner,
                cpp_name=canonical_name or winner.cpp_name,
                evidence=(
                    *winner.evidence,
                    "deduplicated equivalent constructor/parser variants",
                ),
            )

        normalized_groups: dict[
            tuple[str, str | None, str], list[PayloadDecl]
        ] = {}
        for payload in payloads:
            normalized_groups.setdefault(
                (
                    payload.objc_name,
                    payload.command,
                    payload.cpp_name.replace("ChildPayload", ""),
                ),
                [],
            ).append(payload)
        for (_, _, canonical_name), variants in normalized_groups.items():
            if len(variants) > 1:
                select_winner(variants, canonical_name)

        groups: dict[
            tuple[str, str | None, tuple[str, ...]], list[PayloadDecl]
        ] = {}
        for payload in payloads:
            if id(payload) not in selected:
                continue
            discriminator_values = {
                field.default
                for field in payload.fields
                if field.cpp_type != "Command"
                and field.default is not None
                and "::" in field.default
                and not field.default.endswith(
                    ("::NO_USE", "::OUT_OF_RANGE", "::UNKNOWN")
                )
            }
            discriminators = tuple(sorted(discriminator_values))
            groups.setdefault(
                (payload.objc_name, payload.command, discriminators), []
            ).append(payload)

        for variants in groups.values():
            if len(variants) < 2:
                continue
            token_sets = {
                frozenset(
                    set(ProtocolExtractor._camel_tokens(payload.cpp_name))
                    - {"CHILD", "PAYLOAD", "NTFY"}
                )
                for payload in variants
            }
            if len(token_sets) == 1:
                select_winner(variants)
        return [
            replacements.get(id(payload), payload)
            for payload in payloads
            if id(payload) in selected
        ]

    @staticmethod
    def _recalculate_offsets(fields: list[FieldDecl]) -> list[FieldDecl]:
        offset = 0
        dynamic = False
        output: list[FieldDecl] = []
        used_names: set[str] = set()
        for field in fields:
            name = field.name
            if name in used_names:
                type_name = re.findall(
                    r"[A-Za-z_][A-Za-z0-9_]*", field.cpp_type
                )[-1]
                name = type_name[:1].lower() + type_name[1:]
                suffix = 2
                while name in used_names:
                    name = (
                        type_name[:1].lower()
                        + type_name[1:]
                        + str(suffix)
                    )
                    suffix += 1
                field = replace(field, name=name)
            used_names.add(name)
            if field.cpp_type == "Command":
                size = 1
            elif field.cpp_type in _FIXED_SIZES:
                size = _FIXED_SIZES[field.cpp_type]
            elif (
                not field.cpp_type.startswith("MDR")
                and field.wire_kind == "pod"
            ):
                size = 1
            else:
                size = None
            output.append(
                replace(field, offset=None if dynamic else offset)
            )
            if size is None:
                dynamic = True
            elif not dynamic:
                offset += size
        return output

    def extract_payload(
        self,
        class_name: str,
        command_enum: EnumDecl,
        *,
        constructor_override: JavaMethod | None = None,
        cpp_name_override: str | None = None,
    ) -> tuple[PayloadDecl, set[str]]:
        version, table, simple_name = protocol_coordinates(class_name)
        metadata = self.metadata(class_name)
        cpp_name = cpp_name_override or self._clean_type_name(class_name)
        factory_class = class_name + "_Factory"
        factory_methods = self.database.methods(factory_class)
        references: set[str] = set()

        if factory_methods:
            value_methods = [
                method
                for method in self.metadata(factory_class).methods
                if method.java_name == "valueOf"
            ]
            if not value_methods:
                value_methods = [
                    method
                    for method in self.metadata(factory_class).methods
                    if method.java_name == "toStream"
                ]
            if len(value_methods) > 1:
                value_methods = [
                    self._select_value_method(factory_class, value_methods)
                ]
            value_method = value_methods[0] if value_methods else None
            descriptors = (
                parse_descriptors(value_method.parameter_types or "")
                if value_method
                else ()
            )
            generic_parameters = self._generic_parameters(
                value_method.generic_signature if value_method else None
            )
            serializer_method = (
                self._method_symbol(factory_class, value_method)
                if value_method
                else None
            )
            if value_method is not None:
                stream_methods = [
                    method
                    for method in self.metadata(factory_class).methods
                    if method.java_name == "toStream"
                    and method.parameter_types == value_method.parameter_types
                ]
                if len(stream_methods) > 1:
                    raise ExtractionError(
                        f"multiple matching toStream serializers for "
                        f"{factory_class}: "
                        f"{[method.selector for method in stream_methods]}"
                    )
                if stream_methods:
                    serializer_method = self._method_symbol(
                        factory_class, stream_methods[0]
                    )
            parser_methods = [
                method
                for method in self.metadata(factory_class).methods
                if method.java_name in ("parseBytes", "isValid")
            ]
        else:
            if constructor_override is not None:
                constructor = constructor_override
            else:
                constructors = [
                    method
                    for method in metadata.methods
                    if method.java_name == "<init>"
                    and method.parameter_types not in ("[B", None)
                ]
                if len(constructors) > 1:
                    constructors = self._select_constructor(constructors)
                constructor = constructors[0] if constructors else None
            descriptors = (
                parse_descriptors(constructor.parameter_types or "")
                if constructor
                else ()
            )
            generic_parameters = self._generic_parameters(
                constructor.generic_signature if constructor else None
            )
            serializers = self.database.find_methods(
                class_name, selector_prefix="getCommandStream"
            )
            serializer_method = serializers[0] if len(serializers) == 1 else None
            parser_methods = [
                method
                for method in metadata.methods
                if method.java_name.startswith(("restore", "parse"))
            ]

        has_command_stream = bool(
            self.database.find_methods(
                class_name, selector_prefix="getCommandStream"
            )
        )
        inferred_command = (
            self._infer_command(
                simple_name, metadata.package_name, command_enum
            )
            if has_command_stream or factory_methods
            else None
        )
        getter_methods = self._payload_getters(metadata)
        raw_fields: list[FieldDecl] = []
        used_names: set[str] = set()
        getter_offsets: dict[str, int] = {}
        field_evidence: list[str] = []
        dynamic_seen = False
        offset = 0
        wire_prefix = 1 if inferred_command is not None else 0
        serializer_text = (
            self._decompile_text(serializer_method)
            if serializer_method is not None
            else ""
        )
        parameter_specs = [
            (
                descriptor,
                (
                    generic_parameters[index]
                    if index < len(generic_parameters)
                    else None
                ),
            )
            for index, descriptor in enumerate(descriptors)
        ]
        parameter_ranges = self._serializer_parameter_ranges(
            serializer_method, len(parameter_specs)
        )
        if "_" in class_name and any(
            field.name == "this$0_" for field in metadata.fields
        ):
            outer = class_name.split("_", 1)[0]
            parameter_specs = [
                (descriptor, generic)
                for descriptor, generic in parameter_specs
                if not (
                    descriptor == f"L{outer};"
                    or descriptor == "[B"
                )
            ]
        for index, (descriptor, generic) in enumerate(parameter_specs):
            try:
                cpp_type, wire_kind, referenced = self._cpp_type(
                    descriptor, generic
                )
            except ExtractionError as error:
                raise ExtractionError(
                    f"{class_name} field {index} "
                    f"({descriptor!r}, {generic!r}): {error}"
                ) from error
            references.update(referenced)
            getter = self._match_getter(
                getter_methods,
                descriptor,
                generic,
                class_name=class_name,
                expected_offset=(
                    offset + wire_prefix
                    if not dynamic_seen
                    else None
                ),
            )
            name = (
                self._getter_field_name(getter)
                if getter is not None
                else self._derived_field_name(
                    descriptor, generic, index, len(parameter_specs)
                )
            )
            name = self._unique_name(name, used_names)
            if getter is not None:
                span = (
                    self._getter_span(class_name, getter)
                )
                expected_offset = (
                    offset + wire_prefix
                    if not dynamic_seen
                    else None
                )
                if (
                    span is not None
                    and expected_offset is not None
                    and span[0] > expected_offset
                    and span[0] - expected_offset <= 2
                    and not getter_offsets
                ):
                    wire_prefix = span[0] - offset
                    expected_offset = span[0]
                if (
                    span is not None
                    and (
                        expected_offset is None
                        or span[0] != expected_offset
                    )
                ):
                    span = None
                if span is not None:
                    getter_offsets[name] = span[0]
                    field_evidence.append(
                        f"0x{getter.source_address:X} {getter.selector} "
                        f"reads bytes {span[0]}..{span[0] + span[1] - 1}"
                    )
                    sized = self._sized_primitive(descriptor, span[1])
                    if sized is not None:
                        cpp_type = sized
                        wire_kind = "pod"
                    elif (
                        cpp_type == "MDRPrefixedString"
                        and 6 <= span[1] <= 64
                        and not (
                            serializer_text is not None
                            and self._string_field_is_length_prefixed(
                                serializer_text, getter.selector
                            )
                        )
                    ):
                        cpp_type = f"Array<UInt8, {span[1]}>"
                        wire_kind = "helper"
                        field_evidence.append(
                            f"0x{getter.source_address:X} {getter.selector} "
                            f"reads fixed {span[1]}-byte slice "
                            f"{span[0]}..{span[0] + span[1] - 1}"
                        )
                fixed_count = self._fixed_count_from_getter(getter)
                if (
                    fixed_count is not None
                    and cpp_type.startswith(("MDRArray<", "MDRPodArray<"))
                ):
                    element = cpp_type.split("<", 1)[1].rsplit(">", 1)[0]
                    cpp_type = f"MDRFixedArray<{element}, {fixed_count}>"
                    wire_kind = "helper"
                    field_evidence.append(
                        f"0x{getter.source_address:X} {getter.selector} "
                        f"requires {fixed_count} elements"
                    )
            if (
                descriptor == "Z"
                and "ComSonySongpalTandemfamilyMessageMdrV2EnableDisable"
                in serializer_text
            ):
                cpp_type = self._clean_type_name(
                    "ComSonySongpalTandemfamilyMessageMdrV2EnableDisable"
                )
                wire_kind = "pod"
                references.add(
                    "ComSonySongpalTandemfamilyMessageMdrV2EnableDisable"
                )
                field_evidence.append(
                    f"0x{serializer_method.address:X} "
                    "serializer maps boolean to EnableDisable"
                )
            if (
                cpp_type
                == "MDRPodArray<FunctionType>"
            ):
                if "Table1" in name:
                    function_type_table = 1
                elif "Table2" in name:
                    function_type_table = 2
                else:
                    function_type_table = None
                if function_type_table is not None:
                    function_type = "FunctionType"
                    if self._current_coordinates != (
                        2,
                        function_type_table,
                    ):
                        function_type = (
                            f"mdr::v2::t{function_type_table}::"
                            "FunctionType"
                        )
                    cpp_type = f"MDRPodArray<{function_type}>"
            size = _FIXED_SIZES.get(cpp_type)
            if (
                size is None
                and wire_kind == "pod"
                and len(referenced) == 1
                and self.is_wire_enum(next(iter(referenced)))
            ):
                size = 1
            field_offset = None if dynamic_seen else offset
            if size is None:
                dynamic_seen = True
            elif not dynamic_seen:
                offset += size
            default = self._default_value(cpp_type)
            if (
                index in parameter_ranges
                and parameter_ranges[index][0] < 0
                and cpp_type == "UInt8"
            ):
                cpp_type = "Int8"
                default = self._default_value(cpp_type)
            if len(referenced) == 1 and wire_kind == "pod":
                enum_class = next(iter(referenced))
                if self.is_wire_enum(enum_class):
                    enum_decl = self.extract_enum(enum_class)
                    valid_values = [
                        value
                        for value in enum_decl.values
                        if value.name not in ("OUT_OF_RANGE", "UNKNOWN")
                    ]
                    if valid_values:
                        default = (
                            f"{cpp_type}::{valid_values[0].name}"
                        )
            raw_fields.append(
                FieldDecl(
                    name=name,
                    cpp_type=cpp_type,
                    wire_kind=wire_kind,
                    offset=field_offset,
                    default=default,
                    source_type=generic or descriptor,
                    semantic_rules=(
                        (
                            f"Range {parameter_ranges[index][0]} "
                            f"{parameter_ranges[index][1]}"
                        ),
                    )
                    if index in parameter_ranges
                    else (),
                )
            )
        serializer_order = self._serializer_parameter_order(
            serializer_method, len(raw_fields)
        )
        if serializer_order is not None:
            raw_fields = self._recalculate_offsets(
                [raw_fields[index] for index in serializer_order]
            )
        descriptor_field_count = len(raw_fields)

        implicit_type_getters = [
            getter
            for getter in getter_methods
            if (
                getter.selector == "getType"
                or (
                    getter.selector == "getDataType"
                    and not has_command_stream
                    and not factory_methods
                )
            )
            and getter.return_type
            and getter.return_type.startswith("L")
        ]
        if len(implicit_type_getters) > 1:
            raise ExtractionError(
                f"multiple unmatched discriminator getters in {class_name}"
            )
        if implicit_type_getters:
            getter = implicit_type_getters[0]
            getter_methods.remove(getter)
            cpp_type, wire_kind, referenced = self._cpp_type(
                getter.return_type or "", getter.generic_signature
            )
            references.update(referenced)
            default = self._default_value(cpp_type)
            rules: tuple[str, ...] = ()
            if len(referenced) == 1 and wire_kind == "pod":
                enum_class = next(iter(referenced))
                member = self._returned_enum_member(getter, enum_class)
                if member is None:
                    enum_decl = self.extract_enum(enum_class)
                    valid_values = [
                        value
                        for value in enum_decl.values
                        if value.name not in ("OUT_OF_RANGE", "UNKNOWN")
                    ]
                    if valid_values:
                        member = valid_values[0].name
                if member is not None:
                    default = f"{cpp_type}::{member}"
                    if getter.source_address != metadata.metadata_method_address:
                        rules = (f"EnumRange {cpp_type}::{member}",)
            raw_fields.insert(
                0,
                FieldDecl(
                    name=(
                        "type"
                        if getter.selector == "getType"
                        else "dataType"
                    ),
                    cpp_type=cpp_type,
                    wire_kind=wire_kind,
                    offset=0,
                    default=default,
                    source_type=getter.return_type,
                    semantic_rules=rules,
                ),
            )
            span = self._getter_span(class_name, getter)
            if span is not None:
                getter_offsets[
                    "type"
                    if getter.selector == "getType"
                    else "dataType"
                ] = span[0]
                field_evidence.append(
                    f"0x{getter.source_address:X} {getter.selector} "
                    f"reads bytes {span[0]}..{span[0] + span[1] - 1}"
                )
            shifted: list[FieldDecl] = [raw_fields[0]]
            for field in raw_fields[1:]:
                shifted.append(
                    FieldDecl(
                        name=field.name,
                        cpp_type=field.cpp_type,
                        wire_kind=field.wire_kind,
                        offset=(
                            field.offset + 1
                            if field.offset is not None
                            else None
                        ),
                        default=field.default,
                        source_type=field.source_type,
                        semantic_rules=field.semantic_rules,
                    )
                )
            raw_fields = shifted

        if descriptor_field_count == 0:
            metadata_fields: list[FieldDecl] = []
            used_names.update(field.name for field in raw_fields)
            for java_field in metadata.fields:
                if (
                    not java_field.name.startswith("m")
                    or not java_field.name.endswith("_")
                    or java_field.name in (
                        "mRawPayloadBytes_",
                        "mByteArray_",
                    )
                    or java_field.type_descriptor is None
                ):
                    continue
                name = java_field.name[1:-1]
                name = name[:1].lower() + name[1:]
                if name in used_names:
                    continue
                try:
                    cpp_type, wire_kind, referenced = self._cpp_type(
                        java_field.type_descriptor,
                        java_field.generic_signature,
                    )
                except ExtractionError:
                    continue
                references.update(referenced)
                metadata_fields.append(
                    FieldDecl(
                        name=self._unique_name(name, used_names),
                        cpp_type=cpp_type,
                        wire_kind=wire_kind,
                        default=self._default_value(cpp_type),
                        source_type=java_field.type_descriptor,
                    )
                )
            raw_fields = self._recalculate_offsets(
                [*raw_fields, *metadata_fields]
            )

        if (
            (not has_command_stream and not factory_methods)
            or (descriptor_field_count == 0 and not raw_fields)
        ):
            for getter in sorted(
                getter_methods, key=lambda method: method.source_address
            ):
                if (
                    not getter.return_type
                    or getter.return_type == "V"
                    or getter.selector.startswith("is")
                ):
                    continue
                try:
                    cpp_type, wire_kind, referenced = self._cpp_type(
                        getter.return_type, getter.generic_signature
                    )
                except ExtractionError:
                    continue
                references.update(referenced)
                name = self._unique_name(
                    self._getter_field_name(getter), used_names
                )
                span = self._getter_span(class_name, getter)
                if span is not None:
                    getter_offsets[name] = span[0]
                    field_evidence.append(
                        f"0x{getter.source_address:X} {getter.selector} "
                        f"reads bytes {span[0]}..{span[0] + span[1] - 1}"
                    )
                    sized = self._sized_primitive(
                        getter.return_type or "", span[1]
                    )
                    if sized is not None:
                        cpp_type = sized
                        wire_kind = "pod"
                    elif (
                        cpp_type == "MDRPrefixedString"
                        and 6 <= span[1] <= 64
                        and not (
                            serializer_text is not None
                            and self._string_field_is_length_prefixed(
                                serializer_text, getter.selector
                            )
                        )
                    ):
                        cpp_type = f"Array<UInt8, {span[1]}>"
                        wire_kind = "helper"
                        field_evidence.append(
                            f"0x{getter.source_address:X} {getter.selector} "
                            f"reads fixed {span[1]}-byte slice "
                            f"{span[0]}..{span[0] + span[1] - 1}"
                        )
                fixed_count = self._fixed_count_from_getter(getter)
                if (
                    fixed_count is not None
                    and cpp_type.startswith(("MDRArray<", "MDRPodArray<"))
                ):
                    element = cpp_type.split("<", 1)[1].rsplit(">", 1)[0]
                    cpp_type = f"MDRFixedArray<{element}, {fixed_count}>"
                    wire_kind = "helper"
                    field_evidence.append(
                        f"0x{getter.source_address:X} {getter.selector} "
                        f"requires {fixed_count} elements"
                    )
                default = self._default_value(cpp_type)
                if len(referenced) == 1 and wire_kind == "pod":
                    enum_class = next(iter(referenced))
                    if self.is_wire_enum(enum_class):
                        member = self._returned_enum_member(
                            getter, enum_class
                        )
                        enum_decl = self.extract_enum(enum_class)
                        if member is None:
                            valid_values = [
                                value
                                for value in enum_decl.values
                                if value.name
                                not in ("OUT_OF_RANGE", "UNKNOWN")
                            ]
                            if valid_values:
                                member = valid_values[0].name
                        if member is not None:
                            default = f"{cpp_type}::{member}"
                raw_fields.append(
                    FieldDecl(
                        name=name,
                        cpp_type=cpp_type,
                        wire_kind=wire_kind,
                        default=default,
                        source_type=getter.return_type,
                    )
                )
            raw_fields = self._recalculate_offsets(raw_fields)

        raw_fields, static_offsets, static_evidence = (
            self._static_field_evidence(class_name, raw_fields)
        )
        for name, static_offset in static_offsets.items():
            getter_offset = getter_offsets.get(name)
            if getter_offset is not None and getter_offset != static_offset:
                raise ExtractionError(
                    f"contradictory offsets for {class_name}.{name}: "
                    f"getter={getter_offset}, metadata={static_offset}"
                )
            getter_offsets[name] = static_offset
        field_evidence.extend(static_evidence)

        raw_fields, primitive_offsets, primitive_evidence = (
            self._primitive_span_evidence(
                class_name, serializer_method, parser_methods, raw_fields
            )
        )
        for name, primitive_offset in primitive_offsets.items():
            known_offset = getter_offsets.get(name)
            if known_offset is not None and known_offset != primitive_offset:
                raise ExtractionError(
                    f"contradictory offsets for {class_name}.{name}: "
                    f"inferred={known_offset}, parser={primitive_offset}"
                )
            getter_offsets[name] = primitive_offset
        field_evidence.extend(primitive_evidence)

        if (
            serializer_method is None
            and not has_command_stream
            and len(raw_fields) >= 3
        ):
            external = self._external_field_evidence(
                class_name, [field.name for field in raw_fields]
            )
            if external is not None:
                requested_order, external_text = external
                by_name = {field.name: field for field in raw_fields}
                ordered_fields: list[FieldDecl] = []
                cursor_constants = sorted(
                    {
                        int(value)
                        for value in re.findall(
                            r"\+\s*(\d+)", external_text
                        )
                        if int(value) <= 64
                    }
                )
                cursor_groups: dict[str, set[int]] = {}
                for variable, value in re.findall(
                    r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\+\s*(\d+)",
                    external_text,
                ):
                    cursor_groups.setdefault(variable, set()).add(int(value))
                fixed_address_length: int | None = None
                for name in requested_order:
                    field = by_name[name]
                    lowered = name.lower()
                    if field.cpp_type == "MDRPrefixedString":
                        # Evidence-driven, fail-closed: only rewrite to a
                        # fixed-width byte array when the serializer text
                        # proves exactly one fixed width (6..64 bytes). Any
                        # ambiguity or absence keeps the dynamic string.
                        lengths = {
                            int(value, 0)
                            for value in re.findall(
                                r"\bsub_[0-9A-Fa-f]+\("
                                r"[^,\n]+,\s*[^,\n]+,\s*"
                                r"(0x[0-9A-Fa-f]+|\d+)[uUlL]*\s*\)",
                                external_text,
                            )
                            if 6 <= int(value, 0) <= 64
                        }
                        if len(lengths) == 1:
                            length = next(iter(lengths))
                            # Per-field check: the same serializer/processor
                            # text aggregates many fields, so the recovered
                            # fixed width may belong to a different (e.g.
                            # fixed-address) field. A string that is written
                            # with a length prefix in that text is variable and
                            # must stay dynamic even when a fixed width is seen.
                            selector = "get" + name[:1].upper() + name[1:]
                            if not self._string_field_is_length_prefixed(
                                external_text, selector
                            ):
                                fixed_address_length = length
                                field = replace(
                                    field,
                                    cpp_type=f"Array<UInt8, {length}>",
                                    wire_kind="helper",
                                )
                    elif (
                        field.source_type == "I"
                        and lowered.endswith("classofdevice")
                    ):
                        start = (
                            fixed_address_length + 1
                            if fixed_address_length is not None
                            else None
                        )
                        widths = {
                            min(
                                value
                                for value in values
                                if value > start
                            )
                            - start
                            for values in cursor_groups.values()
                            if start is not None
                            and start in values
                            and start - 1 in values
                            and any(
                                start < value <= start + 8
                                for value in values
                            )
                        }
                        widths = {
                            width for width in widths if 1 < width <= 8
                        }
                        if len(widths) != 1:
                            raise ExtractionError(
                                f"cannot prove class-of-device width for "
                                f"{class_name}.{name}: "
                                f"{sorted(widths)} from {cursor_groups}"
                            )
                        width = next(iter(widths))
                        sized = self._sized_primitive("I", width)
                        if sized is None:
                            raise ExtractionError(
                                f"unsupported inferred width {width} for "
                                f"{class_name}.{name}"
                            )
                        field = replace(
                            field, cpp_type=sized, wire_kind="pod"
                        )
                    ordered_fields.append(field)
                raw_fields = self._recalculate_offsets(ordered_fields)

        command = inferred_command
        if (
            command is None
            and has_command_stream
            and self._looks_like_payload(simple_name)
        ):
            raise ExtractionError(
                f"cannot infer command for command-stream class {class_name}"
            )
        classification = (
            "nested_variant"
            if "_" in simple_name and command is not None
            else "payload"
            if command is not None
            else "field_helper"
        )
        if serializer_method is None and len(getter_offsets) >= 2:
            original_order = {
                field.name: index for index, field in enumerate(raw_fields)
            }
            command_width = 1 if command is not None else 0
            raw_fields.sort(
                key=lambda field: (
                    getter_offsets.get(
                        field.name,
                        (
                            field.offset + command_width
                            if field.offset is not None
                            else 1 << 30
                        ),
                    ),
                    original_order[field.name],
                )
            )
            raw_fields = self._recalculate_offsets(raw_fields)

        fields: list[FieldDecl] = []
        if command is not None:
            fields.append(
                FieldDecl(
                    name="command",
                    cpp_type="Command",
                    wire_kind="pod",
                    offset=0,
                    default=f"Command::{command}",
                    source_type=f"THMSGV{version}T{table}Command",
                    semantic_rules=(f"EnumRange Command::{command}",),
                )
            )
            raw_fields = [
                FieldDecl(
                    name=field.name,
                    cpp_type=field.cpp_type,
                    wire_kind=field.wire_kind,
                    offset=(
                        field.offset + 1
                        if field.offset is not None
                        else None
                    ),
                    default=field.default,
                    source_type=field.source_type,
                    semantic_rules=field.semantic_rules,
                )
                for field in raw_fields
            ]
        fields.extend(raw_fields)
        fields = self._apply_parser_enum_rules(
            parser_methods, fields, references
        )

        serialization = (
            "trivial"
            if all(field.cpp_type in _FIXED_SIZES or field.cpp_type == "Command"
                   or self._is_enum_cpp_type(field.cpp_type, references, version, table)
                   for field in fields)
            else "external"
        )
        if any(field.wire_kind in ("array", "string", "helper") for field in fields):
            serialization = "external"

        evidence: tuple[str, ...] = tuple(field_evidence)
        sources: list[SourceRef] = [
            SourceRef(metadata.metadata_method_address, f"+[{class_name} __metadata]")
        ]
        if serializer_method is not None:
            sources.append(
                SourceRef(serializer_method.address, serializer_method.symbol)
            )
            evidence = (
                *evidence,
                *self._serialization_evidence(serializer_method),
            )
        for parser in parser_methods:
            sources.append(
                SourceRef(
                    parser.source_address,
                    f"-[{factory_class if factory_methods else class_name} "
                    f"{parser.selector}]",
                )
            )
        return (
            PayloadDecl(
                objc_name=class_name,
                cpp_name=cpp_name,
                classification=classification,
                command=command,
                fields=tuple(fields),
                serialization=serialization,
                parser=(
                    parser_methods[0].selector if parser_methods else None
                ),
                factory_signature=(
                    value_method.generic_signature
                    or value_method.parameter_types
                    if factory_methods and value_method
                    else None
                ),
                discriminator_field=None,
                discriminator_value=None,
                parent=None,
                sources=tuple(sources),
                evidence=evidence,
            ),
            references,
        )

    def _cpp_type(
        self, descriptor: str, generic: str | None
    ) -> tuple[str, str, set[str]]:
        descriptor = descriptor.lstrip("+-")
        primitives = {
            "B": ("UInt8", "pod"),
            "Z": ("UInt8", "pod"),
            "C": ("UInt16BE", "pod"),
            "S": ("Int16BE", "pod"),
            "I": ("UInt8", "pod"),
            "J": ("UInt64BE", "pod"),
            "F": ("UInt32BE", "pod"),
            "D": ("UInt64BE", "pod"),
        }
        if descriptor in primitives:
            cpp_type, kind = primitives[descriptor]
            return cpp_type, kind, set()
        if descriptor.startswith("["):
            element_type, _, references = self._cpp_type(
                descriptor[1:], None
            )
            container = (
                "MDRPodArray"
                if self._is_pod_container_element(element_type)
                else "MDRArray"
            )
            return f"{container}<{element_type}>", "array", references
        descriptor_body = descriptor[1:-1] if descriptor.startswith("L") else ""
        resolved_class = None
        if descriptor_body.startswith(("com/sony/", "ComSony")):
            resolved_class = self.database.objc_class_for_descriptor(
                descriptor
            )
        elif descriptor_body.startswith(("THMSGV1T", "THMSGV2T")):
            resolved_class = (
                self.database.objc_class_for_descriptor_if_present(descriptor)
            )
        simple = self._descriptor_simple_name(descriptor)
        shared_v2 = re.fullmatch(
            r"Lcom/sony/songpal/tandemfamily/message/mdr/v2/"
            r"(?P<name>[A-Za-z_][A-Za-z0-9_]*);",
            descriptor,
        )
        if shared_v2 is not None:
            name = shared_v2.group("name")
            if resolved_class is None:
                raise ExtractionError(
                    f"shared V2 descriptor lacks metadata class: {descriptor}"
                )
            cpp_type = self.metadata(resolved_class).java_name
            references: set[str] = set()
            if name == "FunctionType":
                # The Java enum multiplexes two wire tables with overlapping
                # byte codes. Fields are qualified to Table1/Table2 below.
                return cpp_type, "pod", references
            enum_class = resolved_class or (
                "ComSonySongpalTandemfamilyMessageMdrV2" + name
            )
            if self.is_wire_enum(enum_class):
                references.add(enum_class)
            return cpp_type, "pod", references
        flat_shared_v2 = re.fullmatch(
            r"L(?P<class>ComSonySongpalTandemfamilyMessageMdrV2"
            r"[A-Za-z_][A-Za-z0-9_]*);",
            descriptor,
        )
        if flat_shared_v2 is not None:
            class_name = resolved_class or flat_shared_v2.group("class")
            cpp_type = self._clean_type_name(class_name)
            references = {class_name} if self.is_wire_enum(class_name) else set()
            return cpp_type, "pod", references
        table_type = re.fullmatch(
            r"Lcom/sony/songpal/tandemfamily/message/mdr/v(?P<version>[12])/"
            r"table(?P<table>[12])/.+/(?P<name>[A-Za-z_][A-Za-z0-9_]*);",
            descriptor,
        )
        if table_type is not None:
            source = (
                int(table_type.group("version")),
                int(table_type.group("table")),
            )
            cpp_type = self._sanitize_identifier(table_type.group("name"))
            if (
                self._current_coordinates is not None
                and source != self._current_coordinates
            ):
                cpp_type = (
                    f"mdr::v{source[0]}::t{source[1]}::{cpp_type}"
                )
            objc_name = resolved_class or (
                f"THMSGV{source[0]}T{source[1]}"
                f"{table_type.group('name')}"
            )
            resolved_coordinates = protocol_coordinates(objc_name)[:2]
            if resolved_coordinates != source:
                raise ExtractionError(
                    f"descriptor table disagrees with metadata for "
                    f"{descriptor}: path={source}, "
                    f"metadata={resolved_coordinates}"
                )
            references = {objc_name} if self.is_wire_enum(objc_name) else set()
            return cpp_type, "pod" if references else "helper", references
        if resolved_class is not None and re.fullmatch(
            r"THMSGV[12]T[12].+", resolved_class
        ):
            source = protocol_coordinates(resolved_class)[:2]
            cpp_type = self._clean_type_name(resolved_class)
            if (
                self._current_coordinates is not None
                and source != self._current_coordinates
            ):
                cpp_type = (
                    f"mdr::v{source[0]}::t{source[1]}::{cpp_type}"
                )
            references = (
                {resolved_class} if self.is_wire_enum(resolved_class) else set()
            )
            return cpp_type, "pod" if references else "helper", references
        if simple in ("NSString", "String", "JavaLangString"):
            return "MDRPrefixedString", "string", set()
        if simple in (
            "Byte",
            "JavaLangByte",
            "Integer",
            "JavaLangInteger",
        ):
            return "UInt8", "pod", set()
        if simple in ("JavaUtilList", "JavaUtilCollection"):
            element_descriptor = self._generic_element_descriptor(generic)
            if element_descriptor is None:
                raise ExtractionError(
                    f"list descriptor lacks element type: {generic!r}"
                )
            element_type, _, references = self._cpp_type(
                element_descriptor, None
            )
            container = (
                "MDRPodArray"
                if self._is_pod_container_element(element_type)
                else "MDRArray"
            )
            return f"{container}<{element_type}>", "array", references
        if simple in ("JavaUtilMap", "JavaUtilLinkedHashMap"):
            generic_content = self._generic_element_descriptor(generic)
            if generic_content is None:
                raise ExtractionError(
                    f"map descriptor lacks key/value types: {generic!r}"
                )
            descriptors = parse_descriptors(generic_content)
            if len(descriptors) != 2:
                raise ExtractionError(
                    f"map descriptor does not have two types: {generic!r}"
                )
            key_type, _, key_references = self._cpp_type(
                descriptors[0], None
            )
            value_type, _, value_references = self._cpp_type(
                descriptors[1], None
            )
            return (
                f"MDRMap<{key_type}, {value_type}>",
                "array",
                key_references | value_references,
            )
        if simple.endswith("Array"):
            element_objc = simple.removesuffix("Array")
            element_cpp = self._clean_type_name(element_objc)
            references = (
                {element_objc} if self.is_wire_enum(element_objc) else set()
            )
            container = (
                "MDRPodArray"
                if references or element_cpp in _FIXED_SIZES
                else "MDRArray"
            )
            return f"{container}<{element_cpp}>", "array", references
        objc_name = resolved_class or simple
        cpp_type = self._clean_type_name(objc_name)
        references = {objc_name} if self.is_wire_enum(objc_name) else set()
        return cpp_type, "pod" if references else "helper", references

    def _clean_type_name(self, objc_name: str) -> str:
        match = re.fullmatch(r"THMSGV[12]T[12](.+)", objc_name)
        fallback = match.group(1) if match else objc_name
        if match is None and any(
            method.kind == "+" and method.selector == "__metadata"
            for method in self.database.methods(objc_name)
        ):
            fallback = self.metadata(objc_name).java_name
        fallback = self._sanitize_identifier(fallback)
        return fallback

    @staticmethod
    def _sanitize_identifier(value: str) -> str:
        return re.sub(r"[^A-Za-z0-9_]", "_", value)

    def _is_enum_cpp_type(
        self,
        cpp_type: str,
        references: set[str],
        version: int,
        table: int,
    ) -> bool:
        if cpp_type.startswith(("MDR", "UInt", "Int")):
            return False
        prefix = f"THMSGV{version}T{table}"
        return any(
            self._clean_type_name(reference) == cpp_type
            for reference in references
        ) or self.is_wire_enum(prefix + cpp_type)

    @staticmethod
    def _is_pod_container_element(cpp_type: str) -> bool:
        return cpp_type in _FIXED_SIZES or (
            "<" not in cpp_type and cpp_type != "MDRPrefixedString"
        )

    @staticmethod
    def _descriptor_simple_name(descriptor: str) -> str:
        descriptor = descriptor.lstrip("+-")
        while descriptor.startswith("["):
            descriptor = descriptor[1:]
        if not descriptor.startswith("L") or not descriptor.endswith(";"):
            raise ExtractionError(f"not an object descriptor: {descriptor!r}")
        return descriptor[1:-1].rsplit("/", 1)[-1]

    @staticmethod
    def _generic_parameters(signature: str | None) -> tuple[str, ...]:
        if not signature or not signature.startswith("("):
            return ()
        end = signature.find(")")
        if end < 0:
            raise ExtractionError(f"invalid generic method signature {signature!r}")
        content = signature[1:end]
        output: list[str] = []
        index = 0
        while index < len(content):
            start = index
            while index < len(content) and content[index] == "[":
                index += 1
            if content[index] == "L":
                depth = 0
                while index < len(content):
                    character = content[index]
                    if character == "<":
                        depth += 1
                    elif character == ">":
                        depth -= 1
                    elif character == ";" and depth == 0:
                        index += 1
                        break
                    index += 1
            else:
                index += 1
            output.append(content[start:index])
        return tuple(output)

    @staticmethod
    def _generic_element_descriptor(generic: str | None) -> str | None:
        if not generic:
            return None
        start = generic.find("<")
        end = generic.rfind(">")
        if start < 0 or end <= start:
            return None
        return generic[start + 1 : end]

    @staticmethod
    def _select_constructor(
        constructors: list[JavaMethod],
    ) -> list[JavaMethod]:
        non_empty = [
            method for method in constructors if method.parameter_types
        ]
        if len(non_empty) == 1:
            return non_empty
        ranked = sorted(
            non_empty,
            key=lambda method: (
                len(parse_descriptors(method.parameter_types or "")),
                sum(
                    1
                    for descriptor in parse_descriptors(
                        method.parameter_types or ""
                    )
                    if descriptor.endswith(("ChildPayload;", "Payload;"))
                ),
            ),
            reverse=True,
        )
        if ranked:
            top_key = (
                len(parse_descriptors(ranked[0].parameter_types or "")),
                sum(
                    descriptor.endswith(("ChildPayload;", "Payload;"))
                    for descriptor in parse_descriptors(
                        ranked[0].parameter_types or ""
                    )
                ),
            )
            tied = [
                method
                for method in ranked
                if (
                    len(parse_descriptors(method.parameter_types or "")),
                    sum(
                        descriptor.endswith(("ChildPayload;", "Payload;"))
                        for descriptor in parse_descriptors(
                            method.parameter_types or ""
                        )
                    ),
                )
                == top_key
            ]
            if len(tied) == 1:
                return tied
        raise ExtractionError(
            "cannot select one payload constructor from "
            f"{[item.selector for item in constructors]}"
        )

    def _constructor_variants(
        self, class_name: str
    ) -> tuple[JavaMethod, ...]:
        if self.database.methods(class_name + "_Factory"):
            return ()
        constructors = [
            method
            for method in self.metadata(class_name).methods
            if method.java_name == "<init>"
            and method.parameter_types not in ("[B", None)
        ]
        constructors = list(
            {
                (method.selector, method.parameter_types): method
                for method in constructors
            }.values()
        )
        if len(constructors) <= 1:
            return tuple(constructors)
        if not self.database.find_methods(
            class_name, selector_prefix="getCommandStream"
        ):
            try:
                return tuple(self._select_constructor(constructors))
            except ExtractionError:
                ranked = sorted(
                    constructors,
                    key=lambda method: len(
                        parse_descriptors(method.parameter_types or "")
                    ),
                    reverse=True,
                )
                return (ranked[0],)
        # Command-stream overloads describe distinct wire variants. Keep both
        # concrete helpers and abstract ChildPayload constructors so the later
        # polymorphic expansion and structural deduplication passes can see the
        # complete variant set.
        return tuple(sorted(constructors, key=lambda method: method.selector))

    @staticmethod
    def _select_value_method(
        factory_class: str, methods: list[JavaMethod]
    ) -> JavaMethod:
        ranked = sorted(
            methods,
            key=lambda method: len(
                parse_descriptors(method.parameter_types or "")
            ),
            reverse=True,
        )
        winner = ranked[0]
        winner_types = parse_descriptors(winner.parameter_types or "")
        if len(ranked) > 1 and len(winner_types) == len(
            parse_descriptors(ranked[1].parameter_types or "")
        ):
            raise ExtractionError(
                f"ambiguous valueOf serializers for {factory_class}: "
                f"{[item.selector for item in methods]}"
            )
        for method in ranked[1:]:
            types = parse_descriptors(method.parameter_types or "")
            if not any(
                winner_types[index : index + len(types)] == types
                for index in range(len(winner_types) - len(types) + 1)
            ):
                raise ExtractionError(
                    f"valueOf overload {method.selector} is not a field subset "
                    f"of {winner.selector} in {factory_class}"
                )
        return winner

    @staticmethod
    def _payload_getters(
        metadata: JavaClassMetadata,
    ) -> list[JavaMethod]:
        ignored = {
            "getCommandStream",
            "hashCode",
            "toString",
            "getClass",
        }
        return [
            method
            for method in metadata.methods
            if _GETTER_RE.fullmatch(method.selector.split(":", 1)[0])
            and method.selector not in ignored
            and not method.parameter_types
        ]

    def _match_getter(
        self,
        getters: list[JavaMethod],
        descriptor: str,
        generic: str | None,
        *,
        class_name: str,
        expected_offset: int | None,
    ) -> JavaMethod | None:
        exact = [
            method
            for method in getters
            if self._descriptors_match(method.return_type, descriptor)
            and (
                generic is None
                or method.generic_signature == f"(){generic}"
                or method.generic_signature == generic
            )
        ]
        candidates = exact or [
            method
            for method in getters
            if self._descriptors_match(method.return_type, descriptor)
        ]
        if not candidates:
            return None
        candidates.sort(key=lambda method: method.source_address)
        if expected_offset is not None:
            positional = [
                method
                for method in candidates
                if (
                    span := self._getter_span(class_name, method)
                ) is not None
                and span[0] == expected_offset
            ]
            if positional:
                candidates = positional
            elif all(
                self._getter_span(class_name, method) is not None
                for method in candidates
            ):
                candidates.sort(
                    key=lambda method: (
                        self._getter_span(class_name, method) or (1 << 30, 0)
                    )[0]
                )
        getter = candidates[0]
        getters.remove(getter)
        return getter

    def _serializer_parameter_ranges(
        self,
        serializer: MethodSymbol | None,
        parameter_count: int,
    ) -> dict[int, tuple[int, int]]:
        if serializer is None:
            return {}
        text = self._decompile_text(serializer)
        assignments = re.findall(
            r"\b([A-Za-z_][A-Za-z0-9_]*)\s*=\s*"
            r"([A-Za-z_][A-Za-z0-9_]*)\s*;",
            text,
        )
        output: dict[int, tuple[int, int]] = {}
        for index in range(parameter_count):
            aliases = {f"a{index + 3}"}
            changed = True
            while changed:
                changed = False
                for target, source in assignments:
                    if source in aliases and target not in aliases:
                        aliases.add(target)
                        changed = True
            alias_pattern = "(?:" + "|".join(
                re.escape(alias) for alias in sorted(aliases)
            ) + ")"
            number = r"-?(?:0x[0-9A-Fa-f]+|\d+)[uUlL]*"
            lower = {
                int(value.rstrip("uUlL"), 0)
                for value in re.findall(
                    rf"\b{alias_pattern}\s*<\s*({number})", text
                )
            }
            upper = {
                int(value.rstrip("uUlL"), 0)
                for value in re.findall(
                    rf"\b{alias_pattern}\s*>\s*({number})", text
                )
            }
            if len(lower) == 1 and len(upper) == 1:
                minimum = next(iter(lower))
                maximum = next(iter(upper))
                if minimum <= maximum:
                    output[index] = (minimum, maximum)
        return output

    def _serializer_parameter_order(
        self,
        serializer: MethodSymbol | None,
        parameter_count: int,
    ) -> tuple[int, ...] | None:
        if serializer is None or parameter_count < 2:
            return None
        text = self._decompile_text(serializer)
        anchor = text.rfind("toStreamWith")
        if anchor < 0:
            return None
        body = text[anchor:]
        assignments = re.findall(
            r"\b([A-Za-z_][A-Za-z0-9_]*)\s*=\s*"
            r"([A-Za-z_][A-Za-z0-9_]*)\s*;",
            text,
        )
        positions: list[tuple[int, int]] = []
        for index in range(parameter_count):
            aliases = {f"a{index + 3}"}
            changed = True
            while changed:
                changed = False
                for target, source in assignments:
                    if source in aliases and target not in aliases:
                        aliases.add(target)
                        changed = True
            occurrences = [
                match.start()
                for alias in aliases
                if (
                    match := re.search(
                        rf"\b{re.escape(alias)}\b", body
                    )
                )
                is not None
            ]
            if not occurrences:
                return None
            positions.append((min(occurrences), index))
        if len({position for position, _ in positions}) != parameter_count:
            return None
        return tuple(index for _, index in sorted(positions))

    @staticmethod
    def _valid_ordinals_from_invalid_condition(
        variable: str, condition: str
    ) -> tuple[int, ...]:
        """Invert simple parser rejection guards expressed in enum ordinals."""
        number = r"(0x[0-9A-Fa-f]+|\d+)[uUlL]*"
        escaped = re.escape(variable)
        range_match = re.search(
            rf"\b{escaped}\s*-\s*{number}\s*>\s*{number}\b",
            condition,
        )
        if range_match is not None and "&&" in condition:
            minimum = int(range_match.group(1), 0)
            width = int(range_match.group(2), 0)
            valid = set(range(minimum, minimum + width + 1))
            valid.update(
                int(value, 0)
                for value in re.findall(
                    rf"\b{escaped}\s*!=\s*{number}", condition
                )
            )
            return tuple(sorted(valid))

        exact_match = re.fullmatch(
            rf"\s*{escaped}\s*!=\s*{number}\s*", condition
        )
        if exact_match is not None:
            return (int(exact_match.group(1), 0),)
        return ()

    def _apply_parser_enum_rules(
        self,
        parsers: list[JavaMethod],
        fields: list[FieldDecl],
        references: set[str],
    ) -> list[FieldDecl]:
        """Map parser ordinal checks back to source-order enum members."""
        enums_by_cpp: dict[str, EnumDecl] = {}
        lookup_helpers_by_cpp: dict[str, set[str]] = {}
        for reference in references:
            if not self.is_wire_enum(reference):
                continue
            enum = self.extract_enum(reference)
            enums_by_cpp[enum.cpp_name] = enum
            lookup_helpers: set[str] = set()
            for method in self.database.find_methods(
                reference, selector_prefix="fromByteCode", kind="+"
            ):
                lookup_helpers.update(
                    re.findall(
                        r"\breturn\s+(sub_[0-9A-Fa-f]+)\s*\(",
                        self._decompile_text(method),
                    )
                )
            lookup_helpers_by_cpp[enum.cpp_name] = lookup_helpers

        accepted_by_lookup: dict[tuple[int, str], tuple[int, ...]] = {}
        assignment = re.compile(
            r"\b(?P<variable>v\d+)\s*=\s*objc_msgSend\("
            r"(?P<expression>[^;\r\n]+),\s*\"ordinal\"\s*\)"
        )
        for parser in parsers:
            if parser.java_name != "parseBytes":
                continue
            text = self._decompile_text(parser)
            for match in assignment.finditer(text):
                offsets = re.findall(
                    r"\+\s*(\d+)\s*\)", match.group("expression")
                )
                if not offsets:
                    continue
                offset = int(offsets[-1])
                lookup_helpers = re.findall(
                    r"\b(sub_[0-9A-Fa-f]+)\s*\(",
                    match.group("expression"),
                )
                if len(lookup_helpers) != 1:
                    continue
                lookup_helper = lookup_helpers[0]
                variable = match.group("variable")
                guards = re.finditer(
                    rf"if\s*\(\s*([^\r\n]*\b{re.escape(variable)}"
                    rf"\b[^\r\n]*)\s*\)",
                    text[match.end() :],
                )
                for guard in guards:
                    tail = text[
                        match.end() + guard.end() :
                        match.end() + guard.end() + 512
                    ]
                    if (
                        "wrong" not in tail
                        and "objc_exception_throw" not in tail
                    ):
                        continue
                    ordinals = self._valid_ordinals_from_invalid_condition(
                        variable, guard.group(1)
                    )
                    if ordinals:
                        accepted_by_lookup[
                            (offset, lookup_helper)
                        ] = ordinals
                        break

        output: list[FieldDecl] = []
        for field in fields:
            if (
                field.name in ("type", "dataType", "inquiredType")
                or field.name.endswith("Type")
                or field.cpp_type.endswith("InquiredType")
            ):
                output.append(field)
                continue
            enum = enums_by_cpp.get(field.cpp_type)
            matching_rules = {
                ordinals
                for helper in lookup_helpers_by_cpp.get(
                    field.cpp_type, set()
                )
                if field.offset is not None
                and (
                    ordinals := accepted_by_lookup.get(
                        (field.offset, helper)
                    )
                )
            }
            ordinals = (
                next(iter(matching_rules))
                if len(matching_rules) == 1
                else None
            )
            if enum is None or not ordinals:
                output.append(field)
                continue
            if any(ordinal >= len(enum.values) for ordinal in ordinals):
                output.append(field)
                continue
            members = tuple(enum.values[ordinal].name for ordinal in ordinals)
            rule = "EnumRange " + " ".join(
                f"{field.cpp_type}::{member}" for member in members
            )
            output.append(replace(field, semantic_rules=(rule,)))
        return output

    @staticmethod
    def _getter_field_name(method: JavaMethod) -> str:
        match = _GETTER_RE.fullmatch(method.selector)
        if match is None:
            raise ExtractionError(f"not a getter: {method.selector}")
        name = match.group("name")
        return name[0].lower() + name[1:]

    def _derived_field_name(
        self,
        descriptor: str,
        generic: str | None,
        index: int,
        count: int,
    ) -> str:
        if descriptor.startswith("L"):
            simple = self._descriptor_simple_name(descriptor)
            if simple in ("JavaUtilList", "JavaUtilCollection"):
                element = self._generic_element_descriptor(generic)
                if element:
                    base = self._descriptor_simple_name(element)
                    base = self._clean_type_name(base)
                    return self._lower_camel(base) + "s"
                return "items"
            if simple == "NSString":
                return "value" if count == 1 else f"value{index + 1}"
            return self._lower_camel(self._clean_type_name(simple))
        return "value" if count == 1 else f"value{index + 1}"

    @staticmethod
    def _unique_name(name: str, used: set[str]) -> str:
        candidate = name
        suffix = 2
        while candidate in used or candidate == "command":
            candidate = f"{name}{suffix}"
            suffix += 1
        used.add(candidate)
        return candidate

    @staticmethod
    def _default_value(cpp_type: str) -> str | None:
        if cpp_type in _FIXED_SIZES:
            return "{}"
        if cpp_type.startswith(("MDR",)):
            return None
        return "{}"

    @staticmethod
    def _lower_camel(value: str) -> str:
        return value[:1].lower() + value[1:]

    @staticmethod
    def _looks_like_payload(simple_name: str) -> bool:
        base = simple_name.split("_", 1)[0]
        tokens = ProtocolExtractor._camel_tokens(base)
        return bool(
            set(tokens) & {"GET", "RET", "SET", "NOTIFY", "NTFY", "RETURN"}
        )

    @staticmethod
    def _camel_tokens(value: str) -> list[str]:
        value = value.replace("_", " ")
        words = re.findall(
            r"[A-Z]+(?=[A-Z][a-z]|\d|\b)|[A-Z]?[a-z]+|\d+", value
        )
        aliases = {
            "NOTIFY": "NTFY",
            "NOTIFICATION": "NTFY",
            "RETURN": "RET",
        }
        tokens = [aliases.get(word.upper(), word.upper()) for word in words]
        expanded: list[str] = []
        for token in tokens:
            if token == "GS":
                expanded.extend(("GENERAL", "SETTING"))
            elif token == "LR":
                expanded.extend(("LEFT", "RIGHT"))
            else:
                expanded.append(token)
        merged: list[str] = []
        index = 0
        while index < len(expanded):
            pair = tuple(expanded[index : index + 2])
            if pair in (("NC", "ASM"), ("EQ", "EBB")):
                merged.append("".join(pair))
                index += 2
            else:
                merged.append(expanded[index])
                index += 1
        return merged

    def _infer_command(
        self,
        simple_name: str,
        package_name: str,
        command_enum: EnumDecl,
    ) -> str | None:
        base_name = simple_name.split("_", 1)[0]
        tokens = self._camel_tokens(base_name)
        category = self._camel_tokens(package_name.rsplit(".", 1)[-1])
        candidates = [tokens]
        command_aliases = {
            ("PERIPHERAL",): ("PERI",),
            ("SAFE", "LISTENING"): ("SL",),
            ("VOICE", "GUIDANCE"): ("VG",),
        }
        for source, replacement in command_aliases.items():
            for index in range(len(tokens) - len(source) + 1):
                if tuple(tokens[index : index + len(source)]) == source:
                    candidates.append(
                        [
                            *tokens[:index],
                            *replacement,
                            *tokens[index + len(source) :],
                        ]
                    )
        if tokens and tokens[0] in _COMMAND_WORDS:
            candidates.append(category + tokens)
            for category_length in range(1, min(3, len(tokens) - 1) + 1):
                candidates.append(
                    [
                        *tokens[1 : 1 + category_length],
                        tokens[0],
                        *tokens[1 + category_length :],
                    ]
                )
            if len(tokens) > 1 and tokens[1 : 1 + len(category)] == category:
                candidates.append(category + tokens[:1] + tokens[1 + len(category) :])
        else:
            candidates.append(category + tokens)
        candidates.extend(
            [
                replacement if token == source else token
                for token in candidate
            ]
            for candidate in list(candidates)
            for source, replacement in (
                ("NTFY", "NTNY"),
                ("NTNY", "NTFY"),
            )
            if source in candidate
        )

        scored: list[tuple[int, str]] = []
        for value in command_enum.values:
            command_tokens = value.name.split("_")
            for candidate in candidates:
                if candidate[: len(command_tokens)] == command_tokens:
                    scored.append((len(command_tokens), value.name))
                elif (
                    len(command_tokens) > 1
                    and candidate[: len(command_tokens) - 1]
                    == command_tokens[1:]
                ):
                    scored.append((len(command_tokens) - 1, value.name))
                elif self._is_subsequence(command_tokens, candidate):
                    scored.append((len(command_tokens) - 1, value.name))
        if not scored:
            return None
        best_score = max(score for score, _ in scored)
        names = sorted({name for score, name in scored if score == best_score})
        if len(names) != 1:
            raise ExtractionError(
                f"ambiguous command for {simple_name}: {names}"
            )
        return names[0]

    @staticmethod
    def _is_subsequence(needle: list[str], haystack: list[str]) -> bool:
        iterator = iter(haystack)
        return all(any(value == item for item in iterator) for value in needle)

    def _serialization_evidence(
        self, method: MethodSymbol
    ) -> tuple[str, ...]:
        evidence: list[str] = []
        visited: set[int] = set()

        def collect(current: MethodSymbol, depth: int) -> None:
            if current.address in visited:
                return
            visited.add(current.address)
            function = self.database.decompile(current)
            visitor = _EvidenceVisitor()
            visitor.apply_to(function.body, None)
            calls = sorted(set(visitor.calls), key=lambda item: item[0])
            for _, name in calls:
                evidence.append(name)
                if depth >= 8 or not re.match(
                    r"^[+-]\[THMSGV[12]T[12][^ ]+ "
                    r"(?:getCommandStream|toStream|write)",
                    name,
                ):
                    continue
                address = ida_name.get_name_ea(idaapi.BADADDR, name)
                if (
                    address == idaapi.BADADDR
                    or ida_funcs.get_func(address) is None
                ):
                    continue
                collect(
                    MethodSymbol(address, "", "", name, name),
                    depth + 1,
                )

        collect(method, 0)
        return tuple(dict.fromkeys(evidence))

    def _returned_enum_member(
        self, getter: JavaMethod, enum_class: str
    ) -> str | None:
        if getter.source_address == 0:
            return None
        function = ida_funcs.get_func(getter.source_address)
        if function is None:
            return None

        class ObjectVisitor(ida_hexrays.ctree_visitor_t):
            def __init__(self) -> None:
                super().__init__(ida_hexrays.CV_FAST)
                self.addresses: set[int] = set()
                self.targets: set[int] = set()

            def visit_expr(self, expression) -> int:
                if expression.op == ida_hexrays.cot_obj:
                    self.addresses.add(expression.obj_ea)
                if expression.op == ida_hexrays.cot_call:
                    target = _unwrap(expression.x)
                    if target.op == ida_hexrays.cot_obj:
                        self.targets.add(target.obj_ea)
                return 0

        method = MethodSymbol(
            getter.source_address,
            getter.kind,
            enum_class,
            getter.selector,
            getter.selector,
        )
        try:
            decompiled = self.database.decompile(method)
        except ExtractionError:
            return None
        visitor = ObjectVisitor()
        visitor.apply_to(decompiled.body, None)
        pattern = re.compile(
            rf"^\+\[{re.escape(enum_class)} (?P<member>[A-Z][A-Z0-9_]*)\]$"
        )
        members: set[str] = set()
        for address in visitor.targets:
            match = pattern.fullmatch(idc.get_func_name(address))
            if match:
                members.add(match.group("member"))
        global_members = self._enum_global_members(enum_class)
        members.update(
            global_members[address]
            for address in visitor.addresses
            if address in global_members
        )
        return next(iter(members)) if len(members) == 1 else None

    def _enum_global_members(self, enum_class: str) -> dict[int, str]:
        cached = self._enum_global_cache.get(enum_class)
        if cached is not None:
            return cached

        class ObjectVisitor(ida_hexrays.ctree_visitor_t):
            def __init__(self) -> None:
                super().__init__(ida_hexrays.CV_FAST)
                self.addresses: set[int] = set()

            def visit_expr(self, expression) -> int:
                if expression.op == ida_hexrays.cot_obj:
                    self.addresses.add(expression.obj_ea)
                return 0

        candidates: dict[int, set[str]] = {}
        for method in self.database.methods(enum_class):
            if method.kind != "+" or not _ENUM_MEMBER_RE.fullmatch(
                method.selector
            ):
                continue
            try:
                function = self.database.decompile(method)
            except ExtractionError:
                continue
            visitor = ObjectVisitor()
            visitor.apply_to(function.body, None)
            for address in visitor.addresses:
                name = ida_name.get_name(address)
                if name.startswith(("qword_", "off_", "unk_")):
                    candidates.setdefault(address, set()).add(method.selector)
        output = {
            address: next(iter(members))
            for address, members in candidates.items()
            if len(members) == 1
        }
        self._enum_global_cache[enum_class] = output
        return output

    @staticmethod
    def _method_symbol(
        class_name: str, method: JavaMethod
    ) -> MethodSymbol:
        return MethodSymbol(
            address=method.source_address,
            kind=method.kind,
            class_name=class_name,
            selector=method.selector,
            symbol=f"{method.kind}[{class_name} {method.selector}]",
        )
