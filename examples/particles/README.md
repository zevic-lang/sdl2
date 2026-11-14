# particles

A fountain of up to 600 sparks. Each spark is the same soft-edged texture, drawn with `Texture.SetBlend(2)` (additive blending), so overlapping sparks add up to white. `Texture.SetColor` tints each one by its age. The fountain runs in a loop until you move the mouse; then it follows the mouse. Space reverses gravity, Escape quits.

```
cd examples/particles
zevpm run
```

Needs the SDL2 development files; see the main README.
