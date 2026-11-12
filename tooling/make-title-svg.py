"""Generate the README title SVG with an embedded bitmap icon and outlined text."""

import argparse
import base64
import math
import mimetypes
from pathlib import Path
from xml.sax.saxutils import escape

from fontTools.pens.boundsPen import BoundsPen
from fontTools.pens.svgPathPen import SVGPathPen
from fontTools.ttLib import TTFont


ROOT = Path(__file__).resolve().parent.parent
DEFAULT_ICON = ROOT / "client" / "Platform" / "Emscripten" / "icon-512.png"
DEFAULT_FONT = ROOT / "client" / "Fonts" / "PlexSansIcon.ttf"
DEFAULT_OUTPUT = ROOT / "docs" / "readme_title.svg"


def kerning(font: TTFont) -> dict[tuple[str, str], int]:
    if "kern" not in font:
        return {}

    pairs: dict[tuple[str, str], int] = {}
    for table in font["kern"].kernTables:
        if table.version == 0 and table.coverage & 1:
            for pair, value in table.kernTable.items():
                pairs[pair] = pairs.get(pair, 0) + value
    return pairs


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--title", default="SonyHeadphonesClient")
    parser.add_argument("--icon", type=Path, default=DEFAULT_ICON)
    parser.add_argument("--font", type=Path, default=DEFAULT_FONT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--height", type=int, default=40)
    parser.add_argument("--icon-size", type=int, default=32)
    parser.add_argument("--font-size", type=int, default=30)
    parser.add_argument("--gap", type=int, default=10)
    args = parser.parse_args()

    if args.height <= 0 or args.icon_size <= 0 or args.font_size <= 0 or args.gap < 0:
        parser.error("sizes must be positive and gap must be non-negative")
    if args.icon_size > args.height:
        parser.error("icon size must not exceed the SVG height")

    icon = args.icon.resolve()
    font_path = args.font.resolve()
    output = args.output.resolve()
    mime_type, _ = mimetypes.guess_type(icon.name)
    if not mime_type or not mime_type.startswith("image/"):
        parser.error(f"cannot determine an image MIME type for {icon}")

    font = TTFont(font_path)
    glyph_set = font.getGlyphSet()
    cmap = font.getBestCmap()
    metrics = font["hmtx"].metrics
    kern = kerning(font)
    units_per_em = font["head"].unitsPerEm
    scale = args.font_size / units_per_em

    glyphs: list[tuple[str, float, str]] = []
    cursor = 0.0
    bounds: tuple[float, float, float, float] | None = None
    previous: str | None = None

    for character in args.title:
        glyph_name = cmap.get(ord(character))
        if glyph_name is None:
            parser.error(f"font has no glyph for {character!r}")
        if previous is not None:
            cursor += kern.get((previous, glyph_name), 0)

        glyph = glyph_set[glyph_name]
        path_pen = SVGPathPen(glyph_set)
        glyph.draw(path_pen)
        path = path_pen.getCommands()
        glyphs.append((glyph_name, cursor, path))

        bounds_pen = BoundsPen(glyph_set)
        glyph.draw(bounds_pen)
        if bounds_pen.bounds is not None:
            x_min, y_min, x_max, y_max = bounds_pen.bounds
            positioned = (x_min + cursor, y_min, x_max + cursor, y_max)
            if bounds is None:
                bounds = positioned
            else:
                bounds = (
                    min(bounds[0], positioned[0]),
                    min(bounds[1], positioned[1]),
                    max(bounds[2], positioned[2]),
                    max(bounds[3], positioned[3]),
                )

        cursor += metrics[glyph_name][0]
        previous = glyph_name

    if bounds is None:
        parser.error("title must contain at least one visible glyph")

    x_min, y_min, x_max, y_max = bounds
    text_x = args.icon_size + args.gap
    x_offset = text_x - x_min * scale
    baseline = args.height / 2 + (y_min + y_max) * scale / 2
    width = math.ceil(x_offset + x_max * scale)
    icon_y = (args.height - args.icon_size) / 2
    icon_data = base64.b64encode(icon.read_bytes()).decode("ascii")

    paths = []
    for _, position, path in glyphs:
        if not path:
            continue
        transform = (
            f"translate({x_offset + position * scale:.3f} {baseline:.3f}) "
            f"scale({scale:.6f} {-scale:.6f})"
        )
        paths.append(f'  <path class="title" d="{path}" transform="{transform}"/>')

    svg = "\n".join(
        [
            f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{args.height}" viewBox="0 0 {width} {args.height}" role="img" aria-labelledby="title">',
            f"  <title id=\"title\">{escape(args.title)}</title>",
            "  <style>",
            "    .title { fill: #1f2328; }",
            "    @media (prefers-color-scheme: dark) { .title { fill: #f0f6fc; } }",
            "  </style>",
            f'  <image x="0" y="{icon_y:g}" width="{args.icon_size}" height="{args.icon_size}" href="data:{mime_type};base64,{icon_data}"/>',
            *paths,
            "</svg>",
            "",
        ]
    )
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(svg, encoding="utf-8", newline="\n")
    print(output)


if __name__ == "__main__":
    main()
