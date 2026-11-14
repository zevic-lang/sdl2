# Changelog

## 0.5.0

- Shapes: `Renderer.Circle`, `FillCircle`, `Triangle`, `FillTriangle` (done in the C shim in one pass)
- Render targets: `Renderer.Canvas(w, h)`, `SetTarget(canvas)`, `ResetTarget()`: a texture you draw into, whose contents stay from frame to frame. `Texture.SetBlend(mode)` (none, blend, add, multiply)
- Input: `Scan*` constants for `KeyHeld` (letters, digits, F1 to F12, arrows, Space, Return, Escape, Tab, Backspace, Left Shift / Ctrl / Alt), so programs no longer need the raw numbers; `ScanFromName`, `KeyName`, `MouseHeld`, `MouseDelta`, `RelativeMouse`, `ShowCursor`
- Window: `SetIcon`, `Minimize`, `Maximize`, `Restore`, `SetBordered`
- `Clock`: holds a frame rate and returns the seconds each frame took
- `SmokeDone`: `ZEVIC_SDL2_FRAMES=N` ends a program after N frames and `ZEVIC_SDL2_SHOT=file.bmp` saves the last one, so examples can be run without a window and checked
- Six new examples: `shapes`, `plasma`, `paint`, `particles`, `pong`, `snake`
- 19 new tests (62 in total). The scancode constants are compared with the names SDL gives them, shapes are checked pixel by pixel
- Windows: CI now builds and tests on a real Windows machine with the official Visual C++ SDL2 packages, besides the mingw build run under Wine; `scripts/windows-check.sh` also runs the new examples and checks their screenshots
- CI: zevic is pinned to a release (v0.4.1) instead of the newest commit, `actions/checkout@v7`, and a push to `main` that passes everything releases the version in `zevic.toml` when it has no tag yet

## 0.4.0

- `Sdl2.Ttf`: TrueType / OpenType text through SDL2_ttf, loaded at run time like Image and Mixer. Sizes, bold / italic / underline / strikethrough, measuring, word wrap, UTF-8 (Japanese with a font that has the glyphs); text comes back as pixels or as a ready `Texture`
- Windows: the shim calls `SDL_SetMainReady()` (no SDL2main needed); `.gitattributes` keeps LF in sources and assets binary; non-ASCII file names are tested. `scripts/windows-check.sh` builds the tests and examples for 64-bit Windows from Linux and runs them under Wine against the official DLLs; CI runs it
- `Renderer.SaveBmp(path)` saves a screenshot
- New example: `ttf` (bundles DejaVu Sans, see its licence file)
- 13 new tests (43 in total), including a generated box font for exact pixel checks (`scripts/make-test-font.py`)

## 0.3.0

- `Sdl2.Image`: PNG, JPG, TIF and WEBP loading through SDL2_image; `Renderer.LoadImage(path)` returns a `Texture`
- `Sdl2.Mixer`: sound effects, looping music, channels and volume through SDL2_mixer
- Both satellites are loaded at run time with `SDL_LoadObject`, so programs that do not use them still need only SDL2. `ZEVIC_SDL2_IMAGE` / `ZEVIC_SDL2_MIXER` override the library path; a missing library is an error value with install instructions, not a crash
- New examples: `image`, `mixer`
- 9 new tests (30 in total), with tiny PNG, BMP and WAV assets in `tests/data`

## 0.2.0

- `Renderer.Text`, `Sdl2.TextWidth`, `Sdl2.TextHeight`: built-in 8x8 bitmap font (ASCII), no SDL_ttf needed
- Every example is now its own zevpm package under `examples/` (hello, framebuffer, sprites, beep, plus new text, events, gamepad)
- CI for Linux and macOS
- 3 new tests (16 in total)

## 0.1.0

First release.

- Core: `Init`, `Quit`, `Headless`, `PixelArt`, `Version`, `DisplaySize`, `Ticks`, `Seconds`, `Delay`, `MessageBox`, clipboard, text input, modifier keys
- `Window` (size, position, fullscreen, show/hide, resizable), `Renderer` (colours, blending, lines, rects, clip, logical size, `ReadPixels`)
- `Texture` (from pixels or BMP, tint/alpha, scale, rotate, flip, sprite-sheet parts) and `Framebuffer`
- `EventPump` with keyboard, text, mouse, wheel, resize, focus and controller events
- `Sdl2.Audio`: queue-based 16-bit output and a sine generator
- `Sdl2.Gamepad`: controller buttons, axes and dead-zone helper
- 13 tests that run on SDL's dummy drivers
