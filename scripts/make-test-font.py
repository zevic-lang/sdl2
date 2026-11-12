#!/usr/bin/env python3
"""Writes tests/data/box.ttf: a tiny TrueType font whose glyphs are plain boxes.

Rendering a box at a known size gives pixels the tests can check exactly, and the file is ours (no font licence).
Needs: pip install fonttools
"""
from fontTools.fontBuilder import FontBuilder
from fontTools.pens.ttGlyphPen import TTGlyphPen


def box(x0, y0, x1, y1):
    pen = TTGlyphPen(None)
    pen.moveTo((x0, y0))
    pen.lineTo((x0, y1))
    pen.lineTo((x1, y1))
    pen.lineTo((x1, y0))
    pen.closePath()
    return pen.glyph()


def empty():
    return TTGlyphPen(None).glyph()


# name: (code point, advance, glyph)   1000 units per em, baseline at 0, ascent 800, descent 200
glyphs = {
    ".notdef": (None, 600, box(100, 0, 500, 700)),
    "space": (0x20, 300, empty()),
    "A": (0x41, 600, box(100, 0, 500, 700)),
    "B": (0x42, 600, box(100, 0, 500, 700)),
    "uni3042": (0x3042, 1000, box(100, 0, 900, 800)),  # あ, full width
    "uni65E5": (0x65E5, 1000, box(100, 0, 900, 800)),  # 日, full width
}

fb = FontBuilder(1000, isTTF=True)
fb.setupGlyphOrder(list(glyphs))
fb.setupCharacterMap({code: name for name, (code, _, _) in glyphs.items() if code})
fb.setupGlyf({name: g for name, (_, _, g) in glyphs.items()})
fb.setupHorizontalMetrics({name: (adv, g.xMin if hasattr(g, "xMin") and g.numberOfContours else 0) for name, (_, adv, g) in glyphs.items()})
fb.setupHorizontalHeader(ascent=800, descent=-200)
fb.setupNameTable({"familyName": "ZevicBox", "styleName": "Regular"})
fb.setupOS2(sTypoAscender=800, sTypoDescender=-200, usWinAscent=800, usWinDescent=200)
fb.setupPost()
fb.save("tests/data/box.ttf")
print("wrote tests/data/box.ttf")
