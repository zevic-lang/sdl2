// One place that decides how SDL is included, for every file in native/.
#ifndef ZSDL_SDL_H
#define ZSDL_SDL_H
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#endif
