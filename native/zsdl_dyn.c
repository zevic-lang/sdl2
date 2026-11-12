#include "zsdl_dyn.h"
#include "zsdl_sdl.h"

static void *try_open(const char *name) {
    if (!name || !*name) return NULL;
    return SDL_LoadObject(name);
}

void *zsdl_dyn_open(const char *envvar, const char *explicit_path, const char *const *names) {
    void *lib = NULL;
    const char *env = envvar ? SDL_getenv(envvar) : NULL;
    if (env && *env) {
        lib = try_open(env);
        if (lib) return lib;
    }
    if (explicit_path && *explicit_path) {
        /* an explicit path is a request for exactly that file: no fallback to the default names */
        lib = try_open(explicit_path);
        if (!lib) SDL_SetError("could not load %s", explicit_path);
        return lib;
    }
    for (int i = 0; names && names[i]; i++) {
        lib = try_open(names[i]);
        if (lib) return lib;
    }
    SDL_SetError("could not load the library (tried %s%s%s%s)",
                 env && *env ? env : "",
                 env && *env ? ", " : "",
                 explicit_path && *explicit_path ? explicit_path : "",
                 names && names[0] ? names[0] : "");
    return NULL;
}

void *zsdl_dyn_sym(void *lib, const char *name) {
    void *fn = SDL_LoadFunction(lib, name);
    if (!fn) SDL_SetError("missing function %s in the loaded library", name);
    return fn;
}

void zsdl_dyn_close(void *lib) {
    if (lib) SDL_UnloadObject(lib);
}
