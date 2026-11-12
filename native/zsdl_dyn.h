// Loads an optional SDL satellite library (SDL2_image, SDL2_mixer) at run time with SDL_LoadObject,
// so a program links only against SDL2 and needs the satellites only when it uses them.
#ifndef ZSDL_DYN_H
#define ZSDL_DYN_H

// Opens the first library that loads. Order: the path in the environment variable `envvar` (if set),
// then `explicit_path` (if not NULL or empty; it is the only candidate after the environment variable),
// otherwise each of `names` (NULL-terminated).
// Returns NULL and sets the SDL error when none could be opened.
void *zsdl_dyn_open(const char *envvar, const char *explicit_path, const char *const *names);

// Looks up `name` in `lib`. Returns NULL and sets the SDL error when it is missing.
void *zsdl_dyn_sym(void *lib, const char *name);

void zsdl_dyn_close(void *lib);

#endif
