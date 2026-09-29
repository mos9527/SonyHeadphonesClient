"""Small, fail-closed access layer over the IDA and J2ObjC metadata APIs."""

from __future__ import annotations

from dataclasses import dataclass
import hashlib
import os
import re

import ida_bytes
import ida_hexrays
import ida_ida
import ida_name
import ida_nalt
import idaapi
import idautils
import idc


PROTOCOL_CLASS_RE = re.compile(
    r"^THMSGV(?P<version>[12])T(?P<table>[12])"
    r"(?P<name>[A-Za-z_][A-Za-z0-9_$]*?)(?P<factory>_Factory)?$"
)
METHOD_RE = re.compile(r"^(?P<kind>[+-])\[(?P<class>[^ ]+) (?P<selector>.+)\]$")
OBJECT_DESCRIPTOR_RE = re.compile(r"^L(?P<body>[^;]+);$")
ALLOC_METHOD_RE = re.compile(
    r"^\+\[(?P<class>[^ ]+) (?:alloc|allocWithZone:|new)\]$"
)
OBJC_CLASS_RE = re.compile(
    r"^(?:_?OBJC_CLASS_\$_|_?OBJC_CLASS___|classRef_)(?P<class>.+)$"
)

_JAVA_STATIC = 0x0008
_JAVA_FINAL = 0x0010
_INTEGER_DESCRIPTORS = {
    "Z": (1, False),
    "B": (8, True),
    "C": (16, False),
    "S": (16, True),
    "I": (32, True),
    "J": (64, True),
}


class ExtractionError(RuntimeError):
    """Raised when binary evidence is missing or contradictory."""


@dataclass(frozen=True)
class MethodSymbol:
    address: int
    kind: str
    class_name: str
    selector: str
    symbol: str


@dataclass(frozen=True)
class JavaMethod:
    kind: str
    selector: str
    java_name: str
    return_type: str | None
    parameter_types: str | None
    generic_signature: str | None
    source_address: int


@dataclass(frozen=True)
class JavaField:
    name: str
    type_descriptor: str | None
    generic_signature: str | None


@dataclass(frozen=True)
class JavaClassMetadata:
    objc_name: str
    java_name: str
    package_name: str
    methods: tuple[JavaMethod, ...]
    fields: tuple[JavaField, ...]
    class_info_address: int
    metadata_method_address: int


@dataclass(frozen=True)
class JavaStaticInteger:
    name: str
    type_descriptor: str
    value: int
    modifiers: int
    metadata_address: int
    accessor_address: int | None


def _decode_string(address: int) -> str | None:
    if not address:
        return None
    value = ida_bytes.get_strlit_contents(
        address, -1, ida_nalt.STRTYPE_C
    )
    if value is None:
        return None
    return value.decode("utf-8", errors="strict")


def read_pointer(address: int) -> int:
    if ida_ida.inf_is_64bit():
        return ida_bytes.get_qword(address)
    return ida_bytes.get_dword(address)


def binary_fingerprint() -> str:
    try:
        digest = ida_nalt.retrieve_input_file_sha256()
        if digest:
            return bytes(digest).hex()
    except AttributeError:
        pass
    path = ida_nalt.get_input_file_path()
    hasher = hashlib.sha256()
    with open(path, "rb") as input_file:
        for block in iter(lambda: input_file.read(1024 * 1024), b""):
            hasher.update(block)
    return hasher.hexdigest()


class IDADatabase:
    def __init__(self) -> None:
        self._by_symbol: dict[str, MethodSymbol] = {}
        self._by_class: dict[str, list[MethodSymbol]] = {}
        self._metadata_cache: dict[str, JavaClassMetadata] = {}
        self._metadata_identity_cache: dict[str, tuple[str, str, int]] = {}
        self._descriptor_class_cache: dict[str, tuple[str, ...]] = {}
        self._static_integer_cache: dict[
            str, tuple[JavaStaticInteger, ...]
        ] = {}
        for address in idautils.Functions():
            symbol = idc.get_func_name(address)
            match = METHOD_RE.fullmatch(symbol)
            if match is None:
                continue
            method = MethodSymbol(
                address=address,
                kind=match.group("kind"),
                class_name=match.group("class"),
                selector=match.group("selector"),
                symbol=symbol,
            )
            self._by_symbol[symbol] = method
            self._by_class.setdefault(method.class_name, []).append(method)
        for methods in self._by_class.values():
            methods.sort(key=lambda item: item.address)

    @property
    def classes(self) -> tuple[str, ...]:
        return tuple(sorted(self._by_class))

    def protocol_classes(self) -> tuple[str, ...]:
        return tuple(
            name
            for name in self.classes
            if PROTOCOL_CLASS_RE.fullmatch(name)
        )

    def methods(self, class_name: str) -> tuple[MethodSymbol, ...]:
        return tuple(self._by_class.get(class_name, ()))

    def method(
        self, class_name: str, selector: str, kind: str = "-"
    ) -> MethodSymbol:
        symbol = f"{kind}[{class_name} {selector}]"
        try:
            return self._by_symbol[symbol]
        except KeyError as error:
            raise ExtractionError(f"function not found: {symbol}") from error

    def find_methods(
        self,
        class_name: str,
        *,
        selector_prefix: str,
        kind: str | None = None,
    ) -> tuple[MethodSymbol, ...]:
        return tuple(
            method
            for method in self.methods(class_name)
            if method.selector.startswith(selector_prefix)
            and (kind is None or method.kind == kind)
        )

    @staticmethod
    def decompile(method: MethodSymbol):
        try:
            function = ida_hexrays.decompile(method.address)
        except ida_hexrays.DecompilationFailure as error:
            raise ExtractionError(
                f"Hex-Rays could not decompile {method.symbol}"
            ) from error
        if function is None:
            raise ExtractionError(
                f"Hex-Rays returned no ctree for {method.symbol}"
            )
        return function

    def metadata(self, class_name: str) -> JavaClassMetadata:
        cached = self._metadata_cache.get(class_name)
        if cached is not None:
            return cached
        metadata_method = self.method(class_name, "__metadata", "+")
        java_name, package_name, class_info_address = self._metadata_identity(
            class_name
        )
        pointer_size = 8 if ida_ida.inf_is_64bit() else 4
        if pointer_size != 8:
            raise ExtractionError("the extractor currently requires a 64-bit IDB")

        pointer_table = read_pointer(class_info_address + 16)
        methods_address = read_pointer(class_info_address + 24)
        fields_address = read_pointer(class_info_address + 32)
        version = ida_bytes.get_word(class_info_address + 40)
        method_count = ida_bytes.get_word(class_info_address + 44)
        field_count = ida_bytes.get_word(class_info_address + 46)
        if version != 7:
            raise ExtractionError(
                f"invalid J2ObjC metadata at 0x{class_info_address:X}"
            )

        method_symbols = [
            method
            for method in self.methods(class_name)
            if method.selector not in ("__metadata", "initialize", "class")
        ]
        metadata_selectors = self._metadata_selectors(metadata_method)
        methods: list[JavaMethod] = []
        for index in range(method_count):
            address = methods_address + index * 32
            return_type = _decode_string(read_pointer(address + 8))
            modifiers = ida_bytes.get_word(address + 16)
            indices = [
                self._signed_word(address + 18 + slot * 2)
                for slot in range(6)
            ]
            pointers = [
                self._indexed_string(pointer_table, value)
                for value in indices
            ]
            java_method_name = pointers[0]
            selector = metadata_selectors.get(address)
            if selector is not None:
                expected_kind = "+" if modifiers & 0x0008 else "-"
                candidates = [
                    method
                    for method in method_symbols
                    if method.selector == selector
                    and method.kind == expected_kind
                ]
                if not candidates:
                    candidates = [
                        method
                        for method in method_symbols
                        if method.selector == selector
                    ]
            else:
                candidates = self._metadata_method_candidates(
                    method_symbols, java_method_name, pointers[1]
                )
            if not candidates and selector is not None:
                candidates = [
                    MethodSymbol(
                        address=metadata_method.address,
                        kind="+" if modifiers & 0x0008 else "-",
                        class_name=class_name,
                        selector=selector,
                        symbol=(
                            f"{'+' if modifiers & 0x0008 else '-'}"
                            f"[{class_name} {selector}] (abstract)"
                        ),
                    )
                ]
            if len(candidates) != 1:
                raise ExtractionError(
                    f"cannot associate metadata method {java_method_name!r} "
                    f"for {class_name}: {[item.selector for item in candidates]}"
                )
            method_symbol = candidates[0]
            if method_symbol in method_symbols:
                method_symbols.remove(method_symbol)
            effective_java_name = java_method_name
            if effective_java_name is None:
                effective_java_name = (
                    "<init>"
                    if method_symbol.selector.startswith("init")
                    else method_symbol.selector.split("With", 1)[0]
                )
            methods.append(
                JavaMethod(
                    kind=method_symbol.kind,
                    selector=method_symbol.selector,
                    java_name=effective_java_name,
                    return_type=return_type,
                    parameter_types=pointers[1],
                    generic_signature=pointers[3],
                    source_address=method_symbol.address,
                )
            )

        fields: list[JavaField] = []
        for index in range(field_count):
            address = fields_address + index * 40
            indices = [
                self._signed_word(address + 26 + slot * 2)
                for slot in range(4)
            ]
            fields.append(
                JavaField(
                    name=_decode_string(read_pointer(address)) or "",
                    type_descriptor=_decode_string(read_pointer(address + 8)),
                    generic_signature=self._indexed_string(
                        pointer_table, indices[2]
                    ),
                )
            )
        result = JavaClassMetadata(
            objc_name=class_name,
            java_name=java_name,
            package_name=package_name,
            methods=tuple(methods),
            fields=tuple(fields),
            class_info_address=class_info_address,
            metadata_method_address=metadata_method.address,
        )
        self._metadata_cache[class_name] = result
        return result

    def objc_class_for_descriptor(self, descriptor: str) -> str:
        """Resolve a JVM/J2ObjC object descriptor to exactly one ObjC class."""
        candidates = self._descriptor_classes(descriptor)
        if len(candidates) != 1:
            detail = (
                "no matching class"
                if not candidates
                else f"ambiguous classes {list(candidates)}"
            )
            raise ExtractionError(
                f"cannot resolve object descriptor {descriptor!r}: {detail}"
            )
        return candidates[0]

    def objc_class_for_descriptor_if_present(
        self, descriptor: str
    ) -> str | None:
        """Resolve a descriptor when metadata exists, rejecting ambiguity."""
        candidates = self._descriptor_classes(descriptor)
        if len(candidates) > 1:
            raise ExtractionError(
                f"cannot resolve object descriptor {descriptor!r}: "
                f"ambiguous classes {list(candidates)}"
            )
        return candidates[0] if candidates else None

    def metadata_for_descriptor(
        self, descriptor: str
    ) -> JavaClassMetadata:
        """Resolve an object descriptor and return its J2ObjC metadata."""
        return self.metadata(self.objc_class_for_descriptor(descriptor))

    def static_integer_constants(
        self, class_name: str
    ) -> tuple[JavaStaticInteger, ...]:
        """Recover static-final primitive integer constants from class info."""
        cached = self._static_integer_cache.get(class_name)
        if cached is not None:
            return cached

        _, _, class_info_address = self._metadata_identity(class_name)
        fields_address = read_pointer(class_info_address + 32)
        field_count = ida_bytes.get_word(class_info_address + 46)
        constants: list[JavaStaticInteger] = []
        for index in range(field_count):
            address = fields_address + index * 40
            name = _decode_string(read_pointer(address))
            descriptor = _decode_string(read_pointer(address + 8))
            modifiers = ida_bytes.get_word(address + 24)
            if (
                name is None
                or descriptor not in _INTEGER_DESCRIPTORS
                or modifiers & (_JAVA_STATIC | _JAVA_FINAL)
                != (_JAVA_STATIC | _JAVA_FINAL)
            ):
                continue

            value = self._normalize_integer(
                read_pointer(address + 16), descriptor
            )
            accessor = self._by_symbol.get(f"+[{class_name} {name}]")
            accessor_value = (
                self._constant_accessor_value(accessor, descriptor)
                if accessor is not None
                else None
            )
            if accessor_value is not None and accessor_value != value:
                raise ExtractionError(
                    f"constant {class_name}.{name} disagrees between "
                    f"metadata ({value}) and accessor ({accessor_value})"
                )
            constants.append(
                JavaStaticInteger(
                    name=name,
                    type_descriptor=descriptor,
                    value=value,
                    modifiers=modifiers,
                    metadata_address=address,
                    accessor_address=(
                        accessor.address if accessor is not None else None
                    ),
                )
            )

        result = tuple(constants)
        self._static_integer_cache[class_name] = result
        return result

    def allocation_target(
        self, method: MethodSymbol, *, max_depth: int = 12
    ) -> str:
        """Find the one concrete ObjC class allocated by a returned call chain."""
        if max_depth < 0:
            raise ValueError("max_depth must be non-negative")

        targets: set[str] = set()
        visited: set[int] = set()
        active: set[int] = set()

        def trace_function(address: int, symbol: str, depth: int) -> None:
            if address in visited or address in active:
                return
            if depth > max_depth:
                raise ExtractionError(
                    f"allocation helper depth exceeded in {method.symbol}"
                )
            function = self._decompile_address(address, symbol)
            active.add(address)
            assignments: dict[int, list[object]] = {}
            returns: list[object] = []

            class ReturnFlowVisitor(ida_hexrays.ctree_visitor_t):
                def __init__(self) -> None:
                    super().__init__(ida_hexrays.CV_FAST)

                def visit_expr(self, expression) -> int:
                    if expression.op != ida_hexrays.cot_asg:
                        return 0
                    target = self_outer._unwrap_ctree(expression.x)
                    if target.op == ida_hexrays.cot_var:
                        assignments.setdefault(target.v.idx, []).append(
                            expression.y
                        )
                    return 0

                def visit_insn(self, instruction) -> int:
                    if instruction.op == ida_hexrays.cit_return:
                        expression = instruction.creturn.expr
                        if expression is not None:
                            returns.append(expression)
                    return 0

            self_outer = self
            visitor = ReturnFlowVisitor()
            visitor.apply_to(function.body, None)
            traced_variables: set[int] = set()

            def trace_expression(expression, current_depth: int) -> bool:
                expression = self._unwrap_ctree(expression)
                if expression.op == ida_hexrays.cot_var:
                    index = expression.v.idx
                    if index in traced_variables:
                        return False
                    traced_variables.add(index)
                    found = False
                    for value in assignments.get(index, ()):
                        found = (
                            trace_expression(value, current_depth) or found
                        )
                    return found
                if expression.op == ida_hexrays.cot_call:
                    target = self._unwrap_ctree(expression.x)
                    target_address = (
                        target.obj_ea
                        if target.op == ida_hexrays.cot_obj
                        else idaapi.BADADDR
                    )
                    target_name = (
                        (
                            idc.get_func_name(target_address)
                            or ida_name.get_name(target_address)
                        )
                        if target_address != idaapi.BADADDR
                        else ""
                    )
                    allocation = ALLOC_METHOD_RE.fullmatch(target_name)
                    if allocation is not None:
                        targets.add(allocation.group("class"))
                        return True

                    runtime_classes = self._runtime_allocation_classes(
                        target_name, expression
                    )
                    if runtime_classes:
                        targets.update(runtime_classes)
                        return True

                    found = False
                    for argument in expression.a:
                        found = (
                            trace_expression(argument, current_depth) or found
                        )
                    if found:
                        return True
                    if (
                        target_address != idaapi.BADADDR
                        and idaapi.get_func(target_address) is not None
                    ):
                        trace_function(
                            target_address,
                            target_name or f"sub_{target_address:X}",
                            current_depth + 1,
                        )
                    return bool(targets)
                if expression.op == ida_hexrays.cot_asg:
                    return trace_expression(expression.y, current_depth)

                found = False
                for attribute in ("x", "y", "z"):
                    try:
                        child = getattr(expression, attribute)
                    except (AttributeError, RuntimeError):
                        continue
                    if child is not None:
                        found = (
                            trace_expression(child, current_depth) or found
                        )
                return found

            for expression in returns:
                trace_expression(expression, depth)
            active.remove(address)
            visited.add(address)

        trace_function(method.address, method.symbol, 0)
        if len(targets) != 1:
            raise ExtractionError(
                f"expected one concrete allocation target in {method.symbol}, "
                f"found {sorted(targets)}"
            )
        return next(iter(targets))

    def _metadata_identity(self, class_name: str) -> tuple[str, str, int]:
        cached = self._metadata_identity_cache.get(class_name)
        if cached is not None:
            return cached
        metadata_method = self.method(class_name, "__metadata", "+")
        class_info_address = self._class_info_address(metadata_method)
        java_name = _decode_string(read_pointer(class_info_address))
        package_name = _decode_string(read_pointer(class_info_address + 8))
        if (
            ida_bytes.get_word(class_info_address + 40) != 7
            or java_name is None
            or package_name is None
        ):
            raise ExtractionError(
                f"invalid J2ObjC metadata at 0x{class_info_address:X}"
            )
        result = (java_name, package_name, class_info_address)
        self._metadata_identity_cache[class_name] = result
        return result

    def _descriptor_classes(self, descriptor: str) -> tuple[str, ...]:
        cached = self._descriptor_class_cache.get(descriptor)
        if cached is not None:
            return cached
        match = OBJECT_DESCRIPTOR_RE.fullmatch(descriptor)
        if match is None:
            raise ExtractionError(
                f"not a non-array object descriptor: {descriptor!r}"
            )
        body = match.group("body")
        if "/" not in body:
            candidates = (
                (body,)
                if body in self._by_class
                and f"+[{body} __metadata]" in self._by_symbol
                else ()
            )
            self._descriptor_class_cache[descriptor] = candidates
            return candidates

        java_simple_name = body.rsplit("/", 1)[-1]
        objc_suffix = java_simple_name.replace("$", "_")
        java_package = (
            body.rsplit("/", 1)[0].replace("/", ".")
            if "/" in body
            else ""
        )
        inner_java_name = java_simple_name.rsplit("$", 1)[-1]
        matches: list[str] = []
        for class_name in self._by_class:
            if (
                not class_name.endswith(objc_suffix)
                or f"+[{class_name} __metadata]" not in self._by_symbol
            ):
                continue
            try:
                java_name, package_name, _ = self._metadata_identity(class_name)
            except ExtractionError:
                continue
            slash_name = (
                f"{package_name.replace('.', '/')}/{java_name}"
                if package_name
                else java_name
            )
            if slash_name == body or (
                "$" in java_simple_name
                and package_name == java_package
                and java_name == inner_java_name
            ):
                matches.append(class_name)
        result = tuple(sorted(set(matches)))
        self._descriptor_class_cache[descriptor] = result
        return result

    @staticmethod
    def _normalize_integer(value: int, descriptor: str) -> int:
        bits, signed = _INTEGER_DESCRIPTORS[descriptor]
        mask = (1 << bits) - 1
        value &= mask
        if signed and value & (1 << (bits - 1)):
            value -= 1 << bits
        return value

    def _constant_accessor_value(
        self, accessor: MethodSymbol, descriptor: str
    ) -> int | None:
        try:
            function = self.decompile(accessor)
        except ExtractionError:
            return None
        values: set[int] = set()

        class ReturnVisitor(ida_hexrays.ctree_visitor_t):
            def __init__(self) -> None:
                super().__init__(ida_hexrays.CV_FAST)

            def visit_insn(self, instruction) -> int:
                if instruction.op != ida_hexrays.cit_return:
                    return 0
                value = IDADatabase._ctree_integer(instruction.creturn.expr)
                if value is not None:
                    values.add(
                        IDADatabase._normalize_integer(value, descriptor)
                    )
                return 0

        visitor = ReturnVisitor()
        visitor.apply_to(function.body, None)
        if len(values) > 1:
            raise ExtractionError(
                f"ambiguous integer returns in {accessor.symbol}: "
                f"{sorted(values)}"
            )
        return next(iter(values)) if values else None

    @staticmethod
    def _ctree_integer(expression) -> int | None:
        if expression is None:
            return None
        expression = IDADatabase._unwrap_ctree(expression)
        if expression.op == ida_hexrays.cot_num:
            return expression.numval()
        unary = {
            getattr(ida_hexrays, "cot_neg", -1): lambda value: -value,
            getattr(ida_hexrays, "cot_bnot", -1): lambda value: ~value,
            getattr(ida_hexrays, "cot_lnot", -1): lambda value: int(not value),
        }
        operation = unary.get(expression.op)
        if operation is not None:
            value = IDADatabase._ctree_integer(expression.x)
            return operation(value) if value is not None else None
        return None

    @staticmethod
    def _unwrap_ctree(expression):
        while expression.op in (ida_hexrays.cot_cast, ida_hexrays.cot_ref):
            expression = expression.x
        return expression

    @staticmethod
    def _runtime_allocation_classes(
        target_name: str, call
    ) -> tuple[str, ...]:
        runtime_name = target_name.lstrip("_")
        selectors = {
            argument.string
            for argument in call.a
            if argument.op == ida_hexrays.cot_str and argument.string
        }
        if runtime_name not in {
            "objc_alloc",
            "objc_alloc_init",
            "objc_opt_new",
        } and not (
            runtime_name in {"objc_msgSend", "objc_msgSendSuper2"}
            and selectors.intersection({"alloc", "new"})
        ):
            return ()
        classes = {
            match.group("class")
            for argument in call.a
            if IDADatabase._unwrap_ctree(argument).op
            == ida_hexrays.cot_obj
            if (
                match := OBJC_CLASS_RE.fullmatch(
                    ida_name.get_name(
                        IDADatabase._unwrap_ctree(argument).obj_ea
                    )
                )
            )
            is not None
        }
        return tuple(sorted(classes))

    @staticmethod
    def _decompile_address(address: int, symbol: str):
        try:
            function = ida_hexrays.decompile(address)
        except ida_hexrays.DecompilationFailure as error:
            raise ExtractionError(
                f"Hex-Rays could not decompile {symbol}"
            ) from error
        if function is None:
            raise ExtractionError(f"Hex-Rays returned no ctree for {symbol}")
        return function

    def _metadata_selectors(
        self, metadata_method: MethodSymbol
    ) -> dict[int, str]:
        class AssignmentVisitor(ida_hexrays.ctree_visitor_t):
            def __init__(self) -> None:
                super().__init__(ida_hexrays.CV_FAST)
                self.assignments: dict[int, str] = {}

            def visit_expr(self, expression) -> int:
                if expression.op != ida_hexrays.cot_asg:
                    return 0
                target = expression.x
                while target.op in (ida_hexrays.cot_cast, ida_hexrays.cot_ref):
                    target = target.x
                value = expression.y
                while value.op == ida_hexrays.cot_cast:
                    value = value.x
                if (
                    target.op == ida_hexrays.cot_obj
                    and value.op
                    in (ida_hexrays.cot_str, ida_hexrays.cot_obj)
                ):
                    rendered = value.dstr()
                    if rendered.startswith('"') and rendered.endswith('"'):
                        self.assignments[target.obj_ea] = rendered[1:-1]
                return 0

        function = self.decompile(metadata_method)
        visitor = AssignmentVisitor()
        visitor.apply_to(function.body, None)
        return visitor.assignments

    @staticmethod
    def _signed_word(address: int) -> int:
        value = ida_bytes.get_word(address)
        return value - 0x10000 if value >= 0x8000 else value

    @staticmethod
    def _indexed_string(pointer_table: int, index: int) -> str | None:
        if index < 0:
            return None
        return _decode_string(read_pointer(pointer_table + index * 8))

    @staticmethod
    def _metadata_method_candidates(
        methods: list[MethodSymbol],
        java_name: str | None,
        parameter_types: str | None,
    ) -> list[MethodSymbol]:
        parameter_count = len(parse_descriptors(parameter_types or ""))
        if java_name is None:
            candidates = [
                method
                for method in methods
                if method.selector.count(":") == parameter_count
            ]
            return candidates[:1] if len(candidates) == 1 else candidates
        aliases = {java_name}
        if java_name == "<init>":
            aliases.add("init")
        return [
            method
            for method in methods
            if any(
                method.selector == alias
                or method.selector.startswith(alias + "With")
                for alias in aliases
            )
            and method.selector.count(":") == parameter_count
        ]

    def _class_info_address(self, metadata_method: MethodSymbol) -> int:
        class ObjectVisitor(ida_hexrays.ctree_visitor_t):
            def __init__(self) -> None:
                super().__init__(ida_hexrays.CV_FAST)
                self.addresses: set[int] = set()

            def visit_expr(self, expression) -> int:
                if expression.op == ida_hexrays.cot_obj:
                    self.addresses.add(expression.obj_ea)
                return 0

        function = self.decompile(metadata_method)
        visitor = ObjectVisitor()
        visitor.apply_to(function.body, None)
        matches = [
            address
            for address in visitor.addresses
            if ida_bytes.get_word(address + 40) == 7
        ]
        if len(matches) != 1:
            raise ExtractionError(
                f"expected one J2ObjC class-info object in "
                f"{metadata_method.symbol}, found {[hex(item) for item in matches]}"
            )
        return matches[0]


def parse_descriptors(descriptors: str) -> tuple[str, ...]:
    """Split concatenated JVM descriptors used by J2ObjC method metadata."""
    output: list[str] = []
    index = 0
    while index < len(descriptors):
        start = index
        while index < len(descriptors) and descriptors[index] == "[":
            index += 1
        if index >= len(descriptors):
            raise ExtractionError(f"truncated descriptor string {descriptors!r}")
        if descriptors[index] == "L":
            end = descriptors.find(";", index)
            if end < 0:
                raise ExtractionError(
                    f"unterminated object descriptor {descriptors!r}"
                )
            index = end + 1
        elif descriptors[index] in "ZBCSIJFDV":
            index += 1
        else:
            raise ExtractionError(
                f"unknown JVM descriptor at {descriptors[index:]!r}"
            )
        output.append(descriptors[start:index])
    return tuple(output)


def protocol_coordinates(class_name: str) -> tuple[int, int, str]:
    match = PROTOCOL_CLASS_RE.fullmatch(class_name)
    if match is None:
        raise ExtractionError(f"not a protocol class: {class_name}")
    return (
        int(match.group("version")),
        int(match.group("table")),
        match.group("name"),
    )
