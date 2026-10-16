"""Generate every platform's client icon from client/Platform/AppIcon.png. Requires Pillow."""
import argparse
import shutil
from pathlib import Path

from PIL import Image

PLATFORM = Path(__file__).resolve().parent.parent / "client" / "Platform"
WINDOWS_ICO_SIZES = [16, 20, 24, 32, 40, 48, 64, 96, 128, 256]


def resized(image: Image.Image, size: int) -> Image.Image:
    if image.width == size:
        return image.copy()
    return image.resize((size, size), Image.NEAREST if size > image.width else Image.LANCZOS)


def opaque(image: Image.Image) -> Image.Image:
    out = Image.new("RGB", image.size, (255, 255, 255))
    out.paste(image, mask=image.getchannel("A"))
    return out


def write_png(source: Path, image: Image.Image, size: int, dest: Path):
    if image.width == size:
        shutil.copyfile(source, dest)
    else:
        resized(image, size).save(dest, optimize=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", type=Path, default=PLATFORM / "AppIcon.png")
    args = parser.parse_args()

    source: Path = args.source.resolve()
    image = Image.open(source).convert("RGBA")
    if image.width != image.height:
        raise SystemExit(f"{source}: icon must be square, got {image.size}")

    pngs = {
        "Android/app/src/main/res/mipmap-nodpi/app_icon.png": 512,
        "OpenHarmony/AppScope/resources/base/media/app_icon.png": 512,
        "Emscripten/icon-192.png": 192,
        "Emscripten/icon-512.png": 512,
    }
    for rel, size in pngs.items():
        write_png(source, image, size, PLATFORM / rel)

    opaque(resized(image, 1024)).save(PLATFORM / "iOS/Assets.xcassets/AppIcon.appiconset/app_icon.png", optimize=True)
    resized(image, 1024).save(PLATFORM / "MacOS/AppIcon.icns", format="ICNS")
    resized(image, 256).save(PLATFORM / "Windows/app.ico", format="ICO", sizes=[(s, s) for s in WINDOWS_ICO_SIZES])

    for rel in [*pngs, "iOS/Assets.xcassets/AppIcon.appiconset/app_icon.png", "MacOS/AppIcon.icns", "Windows/app.ico"]:
        print(PLATFORM / rel)


if __name__ == "__main__":
    main()
