# mixer

Plays WAV notes and a looping track through `Sdl2.Mixer`.

- `1` `2` `3` play C, E, G
- `M` starts and stops the looping arpeggio
- `Up` `Down` change the volume
- `Escape` quits

```
cd examples/mixer
zevpm run
```

Needs SDL2_mixer (libsdl2-mixer-dev / `brew install sdl2_mixer`) in addition to SDL2; it is loaded at run time.
