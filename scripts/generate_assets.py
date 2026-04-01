#!/usr/bin/env python3
import math
import os
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUT_BMP_DIR = ROOT / "assets" / "pomodoro_frames"
OUT_ICO_PATH = ROOT / "main.ico"


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def create_canvas(w: int, h: int, color=(255, 255, 255, 255)):
    return [[list(color) for _ in range(w)] for _ in range(h)]


def set_px(canvas, x: int, y: int, color):
    h = len(canvas)
    w = len(canvas[0])
    if 0 <= x < w and 0 <= y < h:
        canvas[y][x] = list(color)


def fill_circle(canvas, cx: float, cy: float, r: float, color):
    x0 = max(0, int(cx - r - 1))
    x1 = min(len(canvas[0]) - 1, int(cx + r + 1))
    y0 = max(0, int(cy - r - 1))
    y1 = min(len(canvas) - 1, int(cy + r + 1))
    rr = r * r
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            dx = x - cx
            dy = y - cy
            if dx * dx + dy * dy <= rr:
                set_px(canvas, x, y, color)


def fill_ellipse(canvas, cx: float, cy: float, rx: float, ry: float, color):
    x0 = max(0, int(cx - rx - 1))
    x1 = min(len(canvas[0]) - 1, int(cx + rx + 1))
    y0 = max(0, int(cy - ry - 1))
    y1 = min(len(canvas) - 1, int(cy + ry + 1))
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            dx = (x - cx) / max(rx, 1e-6)
            dy = (y - cy) / max(ry, 1e-6)
            if dx * dx + dy * dy <= 1.0:
                set_px(canvas, x, y, color)


def draw_tomato(canvas, phase: float):
    h = len(canvas)
    w = len(canvas[0])
    cx, cy = w / 2, h / 2 + 6

    # 背景
    for y in range(h):
        for x in range(w):
            canvas[y][x] = [248, 249, 255, 255]

    scale = 1.0 + 0.06 * math.sin(phase * 2.0 * math.pi)
    body_rx = 38 * scale
    body_ry = 34 * (1.0 - 0.03 * math.sin(phase * 2.0 * math.pi))

    # 阴影
    fill_ellipse(canvas, cx, cy + 42, 34 * scale, 8, (215, 218, 230, 255))
    # 番茄主体
    fill_ellipse(canvas, cx, cy, body_rx, body_ry, (228, 55, 52, 255))
    # 高光
    fill_ellipse(canvas, cx - 14, cy - 12, 10, 8, (248, 122, 116, 255))
    fill_ellipse(canvas, cx - 9, cy - 9, 5, 4, (255, 180, 174, 255))
    # 叶片
    leaf_angle = phase * 2.0 * math.pi
    for i in range(5):
        a = leaf_angle + i * (2.0 * math.pi / 5.0)
        lx = cx + 11 * math.cos(a)
        ly = cy - body_ry + 8 + 8 * math.sin(a)
        fill_ellipse(canvas, lx, ly, 6, 3, (66, 155, 70, 255))
    # 茄蒂
    fill_ellipse(canvas, cx, cy - body_ry + 10, 7, 4, (72, 130, 63, 255))


def write_bmp(canvas, out_path: Path):
    h = len(canvas)
    w = len(canvas[0])
    row_size = ((24 * w + 31) // 32) * 4
    pixel_data_size = row_size * h
    file_size = 14 + 40 + pixel_data_size

    with out_path.open("wb") as f:
        # BITMAPFILEHEADER
        f.write(b"BM")
        f.write(file_size.to_bytes(4, "little"))
        f.write((0).to_bytes(2, "little"))
        f.write((0).to_bytes(2, "little"))
        f.write((14 + 40).to_bytes(4, "little"))
        # BITMAPINFOHEADER
        f.write((40).to_bytes(4, "little"))
        f.write(w.to_bytes(4, "little", signed=True))
        f.write(h.to_bytes(4, "little", signed=True))
        f.write((1).to_bytes(2, "little"))
        f.write((24).to_bytes(2, "little"))
        f.write((0).to_bytes(4, "little"))  # BI_RGB
        f.write(pixel_data_size.to_bytes(4, "little"))
        f.write((2835).to_bytes(4, "little", signed=True))
        f.write((2835).to_bytes(4, "little", signed=True))
        f.write((0).to_bytes(4, "little"))
        f.write((0).to_bytes(4, "little"))

        pad = b"\x00" * (row_size - w * 3)
        for y in range(h - 1, -1, -1):
            row = bytearray()
            for x in range(w):
                r, g, b, _ = canvas[y][x]
                row.extend([b, g, r])
            f.write(row)
            f.write(pad)


def build_icon_pixels(size: int):
    canvas = create_canvas(size, size, (0, 0, 0, 0))
    cx, cy = size / 2, size / 2 + 2
    fill_ellipse(canvas, cx, cy, size * 0.34, size * 0.30, (228, 55, 52, 255))
    fill_ellipse(canvas, cx - size * 0.12, cy - size * 0.10, size * 0.08, size * 0.06, (255, 170, 165, 255))
    for i in range(5):
        a = i * (2.0 * math.pi / 5.0)
        lx = cx + size * 0.09 * math.cos(a)
        ly = cy - size * 0.30 + size * 0.10 + size * 0.06 * math.sin(a)
        fill_ellipse(canvas, lx, ly, size * 0.05, size * 0.03, (66, 155, 70, 255))
    fill_ellipse(canvas, cx, cy - size * 0.23, size * 0.06, size * 0.03, (72, 130, 63, 255))
    return canvas


def write_ico(canvas, out_path: Path):
    h = len(canvas)
    w = len(canvas[0])
    # ICO uses DIB with height doubled (XOR + AND mask)
    bi_size = 40
    xor_size = w * h * 4
    and_row_size = ((w + 31) // 32) * 4
    and_size = and_row_size * h
    image_size = bi_size + xor_size + and_size

    with out_path.open("wb") as f:
        # ICONDIR
        f.write((0).to_bytes(2, "little"))  # reserved
        f.write((1).to_bytes(2, "little"))  # type icon
        f.write((1).to_bytes(2, "little"))  # count

        # ICONDIRENTRY
        f.write(bytes([w if w < 256 else 0]))
        f.write(bytes([h if h < 256 else 0]))
        f.write(bytes([0]))  # color count
        f.write(bytes([0]))  # reserved
        f.write((1).to_bytes(2, "little"))  # planes
        f.write((32).to_bytes(2, "little"))  # bit count
        f.write(image_size.to_bytes(4, "little"))
        f.write((6 + 16).to_bytes(4, "little"))  # image offset

        # BITMAPINFOHEADER
        f.write((40).to_bytes(4, "little"))
        f.write(w.to_bytes(4, "little", signed=True))
        f.write((h * 2).to_bytes(4, "little", signed=True))
        f.write((1).to_bytes(2, "little"))
        f.write((32).to_bytes(2, "little"))
        f.write((0).to_bytes(4, "little"))
        f.write((xor_size + and_size).to_bytes(4, "little"))
        f.write((2835).to_bytes(4, "little", signed=True))
        f.write((2835).to_bytes(4, "little", signed=True))
        f.write((0).to_bytes(4, "little"))
        f.write((0).to_bytes(4, "little"))

        # XOR bitmap (BGRA, bottom-up)
        for y in range(h - 1, -1, -1):
            for x in range(w):
                r, g, b, a = canvas[y][x]
                f.write(bytes([b, g, r, a]))

        # AND mask (all 0 = opaque/alpha-driven)
        f.write(b"\x00" * and_size)


def main():
    ensure_dir(OUT_BMP_DIR)
    frame_count = 12
    for i in range(frame_count):
        phase = i / frame_count
        canvas = create_canvas(128, 128, (248, 249, 255, 255))
        draw_tomato(canvas, phase)
        write_bmp(canvas, OUT_BMP_DIR / f"tomato_{i:02d}.bmp")

    icon = build_icon_pixels(32)
    write_ico(icon, OUT_ICO_PATH)

    print(f"Generated {frame_count} BMP frames in: {OUT_BMP_DIR}")
    print(f"Generated ICO file: {OUT_ICO_PATH}")


if __name__ == "__main__":
    main()
