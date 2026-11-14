# pong

Two paddles and a ball. W / S or the arrow keys move the left paddle (`KeyHeld` with the `Scan*` constants); the right paddle is the computer, and so is the left one until you press a key. The ball speeds up with every hit and bounces at an angle that depends on where it hits the paddle. First to 7 wins; Space starts a new game.

```
cd examples/pong
zevpm run
```

Needs the SDL2 development files; see the main README.
