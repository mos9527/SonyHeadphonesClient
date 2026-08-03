# MDR protocol extraction

**DISCLAIMER:** LLM generated guide. Follow at your own discretion.

The files in `generated/` are checked-in build inputs and review artifacts.
They contain normalized protocol IR, extraction inventories, and candidate
classification manifests. They are generated from the Sound Connect iOS
binary and must not be edited by hand.

Raw application binaries, IPAs, and IDA databases are local inputs and must
not be committed.

## Prepare the binary

1. Obtain the Sound Connect iOS application through an account and device you
   are authorized to use.
2. Extract the decrypted ARM64 Mach-O containing the J2ObjC classes. An
   encrypted App Store executable cannot be analyzed usefully.
3. Keep the binary outside the repository, or under the ignored
   `tooling/ida/input/` directory.
4. Load the Mach-O in IDA with the ARM64 processor selected.
5. Allow auto-analysis to finish and ensure the Hex-Rays decompiler is
   available. Preserve the Objective-C/J2ObjC symbol names.

The extraction records the input file's SHA-256 fingerprint. A different app
release may require extractor updates.

## Dump protocol IR in IDA

Open the analyzed database, then choose **File > Script file** and run:

```text
tooling/ida/j2objc/extract_all.py
```

The default output is `tooling/ida/generated/`. The script extracts all four
V1/V2 tables atomically and writes:

- `inventory.json`: binary fingerprint and extraction totals;
- `v*_t*.json`: normalized enum and payload IR;
- `v*_t*.manifest.json`: every discovered candidate and its classification.

Extraction is evidence based and fail closed. It resolves JVM and flat
J2ObjC descriptors through class metadata, correlates static index/size
constants with getter and serializer byte access, and follows parser helper
allocations for polymorphic variants. Narrow primitive widths, synthesized
discriminators, fixed cardinalities, and field reordering require
corroborating metadata or Hex-Rays control/data flow; relevant symbols and
addresses are recorded in each declaration's `sources` and `evidence`.
C++ declaration names use the Java names recorded in J2ObjC metadata. There
is no cosmetic name map or protocol override file. Protocol-prefixed parser,
processor, interface, and anonymous classes that expose no serialized fields
remain classified as non-wire aliases in the manifest and do not emit empty
C++ declarations.

The extraction fails if the binary inventory differs from the expected
release, evidence contradicts itself, a descriptor is ambiguous, or a wire
layout cannot be proven. Use `--no-inventory-check` only while investigating
a new binary; do not use it to publish generated output. Fix or extend the
general extractor for newly observed evidence and rerun all four tables.

## Render production headers

Rendering (producing `Protocol` headers) does not require IDA. From the repository root, run:

```sh
python -m tooling.ida.j2objc.render
```

This writes all production headers directly:

```text
libmdr/include/mdr/ProtocolV1.hpp
libmdr/include/mdr/ProtocolV1T1.hpp
libmdr/include/mdr/ProtocolV1T2.hpp
libmdr/include/mdr/ProtocolV2.hpp
libmdr/include/mdr/ProtocolV2T1.hpp
libmdr/include/mdr/ProtocolV2T2.hpp
```

There is no staged-header or parity workflow: production protocol headers are
fully generated from the checked-in IR.

Configure and build normally afterward. CMake regenerates enum formatters,
traits, serialization, and validation sources from these headers.

## Verify and commit

1. Run extraction and rendering a second time; `git diff` should not change.
2. Build `mdr` and run `ctest --output-on-failure`.
3. Review inventory and manifest changes before reviewing generated C++.
4. Commit the IR, manifests, production headers, and generated C++ together.

Do not commit the source IPA/Mach-O, an IDA database, temporary extraction
files, or private payload captures.
