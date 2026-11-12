#!/usr/bin/env bash
# Builds the test suite for 64-bit Windows from Linux and runs it under Wine.
#
# This checks the C shim and the Zevic code against the real Windows SDL2 DLLs (mingw ABI), with SDL's
# dummy video/audio drivers. It does not replace a run on a real Windows machine with the MSVC toolchain.
#
# Needs: python3 with pip, wine (wine64), curl, a built Zevic checkout.
#   ZEVIC_HOME   the Zevic checkout (default: ../zevic)
#   WIN_CACHE    where Windows SDL files are kept (default: ~/.cache/zevic-sdl2-windows)
set -euo pipefail

here="$(cd "$(dirname "$0")/.." && pwd)"
ZEVIC_HOME="${ZEVIC_HOME:-$here/../zevic}"
cache="${WIN_CACHE:-$HOME/.cache/zevic-sdl2-windows}"
zevic="$ZEVIC_HOME/build/zevic"

SDL=2.30.0 IMG=2.8.2 MIX=2.8.0 TTF=2.22.0
base=x86_64-w64-mingw32

mkdir -p "$cache"
fetch() { # url, tar name
    [ -d "$cache/$3" ] && return
    curl -fsSL -o "$cache/$2" "$1"
    tar xzf "$cache/$2" -C "$cache"
}
fetch "https://github.com/libsdl-org/SDL/releases/download/release-$SDL/SDL2-devel-$SDL-mingw.tar.gz" sdl.tgz "SDL2-$SDL"
fetch "https://github.com/libsdl-org/SDL_image/releases/download/release-$IMG/SDL2_image-devel-$IMG-mingw.tar.gz" img.tgz "SDL2_image-$IMG"
fetch "https://github.com/libsdl-org/SDL_mixer/releases/download/release-$MIX/SDL2_mixer-devel-$MIX-mingw.tar.gz" mix.tgz "SDL2_mixer-$MIX"
fetch "https://github.com/libsdl-org/SDL_ttf/releases/download/release-$TTF/SDL2_ttf-devel-$TTF-mingw.tar.gz" ttf.tgz "SDL2_ttf-$TTF"

sdl="$cache/SDL2-$SDL/$base"
# lld wants the import library under the name SDL2.lib, and must not pick the static libSDL2.a
mkdir -p "$cache/implib"
cp "$sdl/lib/libSDL2.dll.a" "$cache/implib/SDL2.lib"

# a "clang" that targets Windows: zig bundles clang, lld and the mingw-w64 headers and libraries
python3 -m ziglang version >/dev/null 2>&1 || pip install --break-system-packages -q ziglang
cat > "$cache/zcc" <<EOS
#!/usr/bin/env bash
exec python3 -m ziglang cc -target x86_64-windows-gnu -I"$sdl/include/SDL2" -I"$sdl/include" -L"$cache/implib" "\$@" -lws2_32 -ladvapi32 -lbcrypt
EOS
chmod +x "$cache/zcc"

work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
sources=("$here"/src/*.zev "$here"/tests/*.zev "$here"/native/*.c)
ZEVIC_HOME="$ZEVIC_HOME" ZEVIC_CLANG="$cache/zcc" "$zevic" --test "${sources[@]}" -lSDL2 -o "$work/tests.exe"

cp "$sdl/bin/SDL2.dll" "$cache/SDL2_image-$IMG/$base/bin/SDL2_image.dll" \
   "$cache/SDL2_mixer-$MIX/$base/bin/SDL2_mixer.dll" "$cache/SDL2_ttf-$TTF/$base/bin/SDL2_ttf.dll" "$work/"

# tests read tests/data/... relative to the package folder, so run from there with the DLLs on the exe's path
cp "$work"/*.dll "$work/tests.exe" "$here/"
trap 'rm -f "$here"/SDL2*.dll "$here/tests.exe"; rm -rf "$work"' EXIT
cd "$here"
# Wine reads Unix file names with the Unix locale; a UTF-8 locale makes it see names like 日本語.png the way
# Windows does (real Windows stores UTF-16 names and needs no such setting).
LANG=C.UTF-8 LC_ALL=C.UTF-8 WINEDEBUG=-all WINEPREFIX="${WINEPREFIX:-$cache/prefix}" \
    SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy wine ./tests.exe | tr -d '\r'
