// SDL2_image, loaded at run time (see zsdl_dyn.h). Only the PNG/JPG/... loader is exposed.
#include "zsdl_dyn.h"
#include "zsdl_sdl.h"

typedef int (*img_init_fn)(int);
typedef void (*img_quit_fn)(void);
typedef SDL_Surface *(*img_load_fn)(const char *);
typedef const SDL_version *(*img_version_fn)(void);

static struct {
    void *lib;
    img_init_fn init;
    img_quit_fn quit;
    img_load_fn load;
    img_version_fn version;
    int flags;
} img;

static const char *const img_names[] = {
#if defined(_WIN32)
    "SDL2_image.dll",
#elif defined(__APPLE__)
    "libSDL2_image-2.0.0.dylib", "libSDL2_image.dylib",
    "/opt/homebrew/lib/libSDL2_image-2.0.0.dylib", "/usr/local/lib/libSDL2_image-2.0.0.dylib",
    "SDL2_image.framework/SDL2_image",
#else
    "libSDL2_image-2.0.so.0", "libSDL2_image.so", "libSDL2_image-2.0.so",
#endif
    NULL};

int zsdl_img_loaded(void) { return img.lib != NULL; }

// Loads the library and starts the PNG/JPG/TIF/WEBP decoders. 0 on success, -1 on failure (see zsdl_error).
int zsdl_img_open(const char *path) {
    if (img.lib) return 0;
    void *lib = zsdl_dyn_open("ZEVIC_SDL2_IMAGE", path, img_names);
    if (!lib) return -1;
    img_init_fn init = (img_init_fn)zsdl_dyn_sym(lib, "IMG_Init");
    img_quit_fn quit = (img_quit_fn)zsdl_dyn_sym(lib, "IMG_Quit");
    img_load_fn load = (img_load_fn)zsdl_dyn_sym(lib, "IMG_Load");
    img_version_fn ver = (img_version_fn)zsdl_dyn_sym(lib, "IMG_Linked_Version");
    if (!init || !quit || !load || !ver) {
        zsdl_dyn_close(lib);
        return -1;
    }
    img.lib = lib;
    img.init = init;
    img.quit = quit;
    img.load = load;
    img.version = ver;
    img.flags = init(0x1 | 0x2 | 0x4 | 0x8); /* JPG | PNG | TIF | WEBP */
    return 0;
}

void zsdl_img_close(void) {
    if (!img.lib) return;
    img.quit();
    zsdl_dyn_close(img.lib);
    img.lib = NULL;
}

// The decoders that started: 1 JPG, 2 PNG, 4 TIF, 8 WEBP.
int zsdl_img_flags(void) { return img.lib ? img.flags : 0; }

const char *zsdl_img_version(void) {
    static char buf[32];
    if (!img.lib) return "";
    const SDL_version *v = img.version();
    SDL_snprintf(buf, sizeof buf, "%d.%d.%d", v->major, v->minor, v->patch);
    return buf;
}

// Decodes an image file into a texture that blends by alpha.
void *zsdl_img_texture(void *renderer, const char *path) {
    if (!img.lib) {
        SDL_SetError("SDL2_image is not loaded: call Sdl2.Image.Init() first");
        return NULL;
    }
    SDL_Surface *s = img.load(path);
    if (!s) return NULL;
    SDL_Texture *t = SDL_CreateTextureFromSurface((SDL_Renderer *)renderer, s);
    SDL_FreeSurface(s);
    if (t) SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t;
}
