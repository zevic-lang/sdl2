# Changelog

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
