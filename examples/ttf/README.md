# ttf

Antialiased TrueType text through `Sdl2.Ttf`: sizes, bold / italic / underline, word wrap and Japanese.

```
cd examples/ttf
zevpm run
```

`assets/DejaVuSans.ttf` (free licence, see `assets/DejaVu-LICENSE.txt`) covers Latin, Greek and Cyrillic. Japanese needs a font with those glyphs, so the example tries common system fonts (Yu Gothic / Meiryo on Windows, Hiragino on macOS, Noto Sans CJK / IPA on Linux) and says so when it finds none. Use any `.ttf` or `.ttc` file by changing the path.

Needs SDL2_ttf (libsdl2-ttf-dev / `brew install sdl2_ttf`) in addition to SDL2; it is loaded at run time.
