#!/usr/bin/env bash
# Builds every example and runs the ones that can run on their own for 20 frames, saving a screenshot of the
# last frame and checking that it is a real picture. Used by CI on Linux, macOS and Windows.
#
#   scripts/smoke-examples.sh zevpm        (the zevpm to build with: zevpm2 on Linux and macOS, zevpm on Windows)
#
# SDL_VIDEODRIVER=dummy and SDL_AUDIODRIVER=dummy are expected to be set: no window and no sound device are needed.
set -euo pipefail

pm="${1:?usage: smoke-examples.sh ZEVPM}"
here="$(cd "$(dirname "$0")/.." && pwd)"
py="$(command -v python3 || command -v python)"

for dir in "$here"/examples/*/; do
    name="$(basename "$dir")"
    echo "== $name"
    (cd "$dir" && "$pm" build)
    exe="$(ls "$dir"build | grep -v '\.pdb$' | grep -v '\.ilk$' | head -1)"
    case "$name" in
        hello|framebuffer|beep|shapes|plasma|paint|particles|pong|snake)
            rm -f "$dir/smoke.bmp"
            out="$(cd "$dir" && ZEVIC_SDL2_FRAMES=20 ZEVIC_SDL2_SHOT=smoke.bmp "./build/$exe" | tr -d '\r' | tail -1)"
            echo "  ran${out:+: $out}"
            # hello, framebuffer and beep end by themselves and do not take a screenshot
            case "$name" in
                hello|framebuffer|beep) ;;
                *) "$py" "$here/scripts/check-bmp.py" "$dir/smoke.bmp"; rm -f "$dir/smoke.bmp" ;;
            esac ;;
        *) echo "  built" ;;
    esac
done
