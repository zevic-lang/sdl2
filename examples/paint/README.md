# paint

A small paint program built on `Renderer.Canvas`: the strokes are drawn into a texture, so they stay from frame to frame.

| | |
| --- | --- |
| left button | paint |
| right button | erase (writes transparent pixels into the canvas) |
| wheel | brush size |
| click a swatch, or keys 1 to 8 | colour |
| C | clear |
| S | save the window as `paint.bmp` |
| Escape | quit |

```
cd examples/paint
zevpm run
```

Needs the SDL2 development files; see the main README.
