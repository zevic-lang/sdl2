# sdl2

SDL2 bindings for [Zevic](https://github.com/zevic-lang/zevic). A thin C shim (`native/zsdl.c`) plus Zevic modules that expose only Zevic types. Only portable SDL2 calls are used, so the same code builds on Linux, macOS and Windows.

## Install

Install the SDL2 development files for your system, then:

```
zevpm get zevic-lang/sdl2@v0.3.0
```

```
import Sdl2
import Sdl2.Audio as Audio        // optional
import Sdl2.Gamepad as Gamepad    // optional
import Sdl2.Image as Image        // optional, needs SDL2_image installed
import Sdl2.Mixer as Mixer        // optional, needs SDL2_mixer installed
import Sdl2.Ttf as Ttf            // optional, needs SDL2_ttf installed
```

`zevpm build` / `zevpm run` link `-lSDL2` for you (this package's `[native] libs`).

| OS | SDL2 development files |
| --- | --- |
| Ubuntu / Debian | `sudo apt install libsdl2-dev` (optional: `libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev`) |
| macOS | `brew install sdl2` (optional: `sdl2_image sdl2_mixer sdl2_ttf`) |
| Windows | SDL2 development libraries (vcpkg `sdl2`, or the MinGW/VC zip from libsdl.org); optional SDL2_image.dll and SDL2_mixer.dll next to the program |

## Windows

The library uses only portable SDL2 calls, so the same source builds on Windows. It does not use SDL2main: the shim calls `SDL_SetMainReady()` itself, and Zevic programs keep their normal `Main`.

1. Get the SDL2 development files and put the headers and `SDL2.lib` where clang can find them, for example with [vcpkg](https://vcpkg.io): `vcpkg install sdl2:x64-windows` and set the `INCLUDE` and `LIB` environment variables to `installed\x64-windows\include` and `installed\x64-windows\lib` (the shim includes `<SDL2/SDL.h>`).
2. Put `SDL2.dll` next to your program. For the optional modules also put `SDL2_image.dll`, `SDL2_mixer.dll` or `SDL2_ttf.dll` there; they are loaded when you call `Init()`.
3. `zevpm build` / `zevpm run` as usual. File names are UTF-8 in Zevic and SDL converts them, so paths such as `画像\ねこ.png` work.

What has been checked: `scripts/windows-check.sh` builds the tests and all examples for 64-bit Windows from Linux (zig's bundled clang, mingw-w64 ABI) and runs them under Wine against the official SDL2 / SDL2_image / SDL2_mixer / SDL2_ttf DLLs with SDL's dummy drivers. It does not replace a run on a real Windows machine with the MSVC toolchain, which these steps describe but have not been run.

## Quick start

```
module Main

import Sdl2

fn Main() {
    Sdl2.Init().Unwrap()
    defer Sdl2.Quit()
    let win: Sdl2.Window = Sdl2.Window.Open("hello", 640, 360).Unwrap()
    defer win.Close()
    let ren: Sdl2.Renderer = win.Renderer().Unwrap()
    defer ren.Close()
    let pump: Sdl2.EventPump = Sdl2.EventPump.New()
    defer pump.Close()

    mut running: Bool = true
    while (running) {
        while (true) {
            match (pump.Poll()) {
                Some(Sdl2.Event.Quit) => { running = false },
                Some(_) => {},
                None => break,
            }
        }
        ren.Color(20, 24, 40, 255)
        ren.Clear()
        ren.Color(255, 200, 0, 255)
        ren.FillRect(100, 100, 80, 80)
        ren.Present()
    }
}
```

Close what you open with `defer x.Close()`; there are no finalizers.

## API

**Sdl2**
- Lifecycle and info: `Init`, `Quit`, `Headless`, `PixelArt`, `Version`, `DisplaySize`
- Text size: `TextWidth`, `TextHeight` (for the built-in 8x8 font used by `Renderer.Text`)
- Time: `Ticks`, `Seconds`, `Delay`
- Input state: `KeyHeld(scancode)`, `MouseX`, `MouseY`, `ModState`, `HasMod`, `TextInput(on)`; constants `Key*`, `Mod*`, `Button*`
- Other: `ClipboardText`, `SetClipboardText`, `MessageBox`, `RequestQuit`
- `Window.Open(title, w, h)`: `Close`, `SetTitle`, `Width`, `Height`, `SetSize`, `SetMinSize`, `SetPosition`, `X`, `Y`, `SetResizable`, `SetFullscreen`, `Show`, `Hide`, `Raise`, `Renderer()`
- `Renderer`: `Color`, `Clear`, `Present`, `Point`, `Line`, `Rect`, `FillRect`, `Text`, `SaveBmp` (screenshot), `Blend`, `SetLogicalSize`, `SetClip`, `ClearClip`, `OutputWidth`, `OutputHeight`, `ReadPixels`, `TextureFromPixels`, `LoadBmp`, `Framebuffer`
- `Texture`: `Width`, `Height`, `SetColor`, `Draw`, `DrawScaled`, `DrawEx(angle, flip)`, `DrawPart`
- `Framebuffer.Draw(Array<U32>)` with 0xAARRGGBB pixels
- `EventPump.Poll()` gives `Option<Event>`: `Quit`, `KeyDown(key, mods)`, `KeyUp`, `TextInput(text)`, `MouseMove`, `MouseDown(button, x, y)`, `MouseUp`, `Wheel(dx, dy)`, `Resized(w, h)`, `Focus(on)`, `PadDown(id, button)`, `PadUp`, `PadAdded`, `PadRemoved`, `Other(type)`

**Sdl2.Image** (optional, needs SDL2_image): `Init`, `InitFrom(path)`, `Quit`, `IsLoaded`, `Version`, `Supports(Image.PNG)`; then `Renderer.LoadImage(path)` gives a `Texture` (PNG, JPG, TIF, WEBP with alpha)

**Sdl2.Mixer** (optional, needs SDL2_mixer): `Init`, `InitWith(rate, channels, buffer)`, `InitFrom(path)`, `Quit`, `IsLoaded`, `Version`, `SetChannels`, `Volume`, `Playing`, `Halt`, `MusicVolume`, `MusicPlaying`, `MusicPaused`, `PauseMusic`, `ResumeMusic`, `StopMusic`; `Sound.Load(path)` with `Play`, `PlayRepeat`, `Loop`, `SetVolume`, `Close`; `Music.Load(path)` with `Play`, `PlayTimes`, `Loop`, `Close`

**Sdl2.Ttf** (optional, needs SDL2_ttf): `Init`, `InitFrom(path)`, `Quit`, `IsLoaded`, `Version`; `Font.Open(path, size)` with `Close`, `SetSize`, `Height`, `Ascent`, `Descent`, `LineSkip`, `Family`, `SetStyle(Ttf.Bold + Ttf.Italic ...)`, `Measure(text)`, `Image` / `ImageWrapped` (0xAARRGGBB pixels), `Texture(ren, text, r, g, b, a)` and `TextureWrapped(..., width)`. Text is UTF-8, so Japanese works with a font that has the glyphs.

**Sdl2.Audio**: `Init`, `Device.Open(rate, channels)`, `Queue(Array<I16>)`, `Queued`, `Clear`, `Pause`, `Close`, `Sine(freq, seconds, rate, volume)`

**Sdl2.Gamepad**: `Init`, `Count`, `IsPad`, `Pad.Open(index)`, `Button`, `Axis`, `Name`, `Id`, `Close`, `Stick(raw, dead)`; constants `Button*`, `Axis*`

## Examples

Every example in `examples/` is its own zevpm package that depends on this one by path:

| Example | Shows |
| --- | --- |
| `hello` | window, renderer, event loop |
| `framebuffer` | writing pixels from code |
| `sprites` | textures, rotation, keyboard state, logical size |
| `text` | the built-in bitmap font |
| `events` | every event type, text input |
| `gamepad` | controllers |
| `beep` | audio queue |
| `image` | PNG with alpha through SDL2_image |
| `mixer` | sound effects and music through SDL2_mixer |
| `ttf` | TrueType text, styles, wrapping and Japanese through SDL2_ttf |

```
cd examples/sprites
zevpm run
```

## Tests

```
zevpm test
```

The tests call `Sdl2.Headless()`, which switches SDL to its dummy video and audio drivers, so they need no display or sound card.

## Design notes

- `SDL_Event` is a union and Zevic has no unions, so events live in an opaque buffer read through accessor functions in the shim.
- SDL constants are read from the real headers through `zsdl_const`, never copied.
- Audio uses SDL's queue API instead of callbacks (Zevic passes only non-capturing `fn`).
- `Renderer.Text` uses a built-in 8x8 font (ASCII only), so quick text needs no SDL_ttf and no font files; use `Sdl2.Ttf` for real fonts and non-ASCII text. Non-ASCII characters draw as `?`.
- SDL2_image, SDL2_mixer and SDL2_ttf are loaded at run time with `SDL_LoadObject`, not linked. A program that never calls `Image.Init()`, `Mixer.Init()` or `Ttf.Init()` needs only SDL2. Set `ZEVIC_SDL2_IMAGE` / `ZEVIC_SDL2_MIXER` / `ZEVIC_SDL2_TTF` to a library path if yours is somewhere unusual.
- Not covered yet: joysticks that are not game controllers, haptics, window icons.

## License

Apache-2.0. `native/font8x8.h` is the public-domain font8x8 table by Daniel Hepper (based on IBM VGA fonts).
