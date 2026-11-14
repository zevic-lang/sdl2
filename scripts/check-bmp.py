"""Checks that a file is a BMP picture with more than three colours (so not a blank window).

    python3 check-bmp.py shot.bmp
"""
import sys

data = open(sys.argv[1], "rb").read()
assert data[:2] == b"BM", "%s is not a BMP" % sys.argv[1]
w = int.from_bytes(data[18:22], "little")
h = abs(int.from_bytes(data[22:26], "little", signed=True))
bpp = int.from_bytes(data[28:30], "little")
off = int.from_bytes(data[10:14], "little")
step = bpp // 8
colours = {data[i:i + step] for i in range(off, len(data) - step + 1, step)}
assert len(colours) > 3, "the picture has only %d colours" % len(colours)
print("  ok    screenshot %dx%d, %d colours" % (w, h, len(colours)))
