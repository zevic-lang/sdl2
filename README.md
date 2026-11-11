# sdl2

SDL2 bindings for [Zevic](https://github.com/zevic-lang/zevic). A thin C shim (`native/zsdl.c`) plus Zevic modules that expose only Zevic types. Only portable SDL2 calls are used, so the same code builds on Linux, macOS and Windows.

## Install

Install the SDL2 development files for your system, then:

```
zevpm get zevic-lang/sdl2@v0.2.0
```

```
import Sdl2
import Sdl2.Audio as Audio        // optional
import Sdl2.Gamepad as Gamepad    // optional
```

`zevpm build` / `zevpm run` link `-lSDL2` for you (this package's `[native] libs`).

| OS | SDL2 development files |
| --- | --- |
| Ubuntu / Debian | `sudo apt install libsdl2-dev` |
| macOS | `brew install sdl2` |
| Windows | SDL2 development libraries (vcpkg `sdl2`, or the MinGW/VC zip from libsdl.org) |

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
- `Renderer`: `Color`, `Clear`, `Present`, `Point`, `Line`, `Rect`, `FillRect`, `Text`, `Blend`, `SetLogicalSize`, `SetClip`, `ClearClip`, `OutputWidth`, `OutputHeight`, `ReadPixels`, `TextureFromPixels`, `LoadBmp`, `Framebuffer`
- `Texture`: `Width`, `Height`, `SetColor`, `Draw`, `DrawScaled`, `DrawEx(angle, flip)`, `DrawPart`
- `Framebuffer.Draw(Array<U32>)` with 0xAARRGGBB pixels
- `EventPump.Poll()` gives `Option<Event>`: `Quit`, `KeyDown(key, mods)`, `KeyUp`, `TextInput(text)`, `MouseMove`, `MouseDown(button, x, y)`, `MouseUp`, `Wheel(dx, dy)`, `Resized(w, h)`, `Focus(on)`, `PadDown(id, button)`, `PadUp`, `PadAdded`, `PadRemoved`, `Other(type)`

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
- `Renderer.Text` uses a built-in 8x8 font (ASCII only), so text needs no SDL_ttf and no font files. Non-ASCII characters draw as `?`.
- Not covered yet: SDL_image / SDL_ttf / SDL_mixer, joysticks that are not game controllers, haptics, window icons.

## License

Apache-2.0. `native/font8x8.h` is the public-domain font8x8 table by Daniel Hepper (based on IBM VGA fonts).
