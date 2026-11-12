// SDL2_ttf, loaded at run time (see zsdl_dyn.h). Text is rendered to 0xAARRGGBB pixels that Zevic turns into textures.
#include "zsdl_dyn.h"
#include "zsdl_sdl.h"
#include <string.h>

typedef int (*ttf_init_fn)(void);
typedef void (*ttf_void_fn)(void);
typedef void *(*ttf_open_fn)(const char *, int);
typedef void (*ttf_close_fn)(void *);
typedef SDL_Surface *(*ttf_render_fn)(void *, const char *, SDL_Color);
typedef SDL_Surface *(*ttf_render_wrapped_fn)(void *, const char *, SDL_Color, Uint32);
typedef int (*ttf_size_fn)(void *, const char *, int *, int *);
typedef int (*ttf_metric_fn)(const void *);
typedef void (*ttf_set_style_fn)(void *, int);
typedef int (*ttf_set_size_fn)(void *, int);
typedef const char *(*ttf_family_fn)(const void *);
typedef const SDL_version *(*ttf_version_fn)(void);

static struct {
    void *lib;
    ttf_init_fn init;
    ttf_void_fn quit;
    ttf_open_fn open_font;
    ttf_close_fn close_font;
    ttf_render_fn render;
    ttf_render_wrapped_fn render_wrapped;
    ttf_size_fn size_utf8;
    ttf_metric_fn height, ascent, descent, line_skip, get_style;
    ttf_set_style_fn set_style;
    ttf_set_size_fn set_size;
    ttf_family_fn family;
    ttf_version_fn version;
} tf;

static const char *const ttf_names[] = {
#if defined(_WIN32)
    "SDL2_ttf.dll",
#elif defined(__APPLE__)
    "libSDL2_ttf-2.0.0.dylib", "libSDL2_ttf.dylib",
    "/opt/homebrew/lib/libSDL2_ttf-2.0.0.dylib", "/usr/local/lib/libSDL2_ttf-2.0.0.dylib",
    "SDL2_ttf.framework/SDL2_ttf",
#else
    "libSDL2_ttf-2.0.so.0", "libSDL2_ttf.so", "libSDL2_ttf-2.0.so",
#endif
    NULL};

#define RESOLVE(field, type, name)                                                       \
    do {                                                                                 \
        tf.field = (type)zsdl_dyn_sym(lib, name);                                        \
        if (!tf.field) { zsdl_dyn_close(lib); memset(&tf, 0, sizeof tf); return -1; }    \
    } while (0)

int zsdl_ttf_loaded(void) { return tf.lib != NULL; }

// Loads the library and starts it. 0 on success, -1 on failure (see zsdl_error).
int zsdl_ttf_open(const char *path) {
    if (tf.lib) return 0;
    void *lib = zsdl_dyn_open("ZEVIC_SDL2_TTF", path, ttf_names);
    if (!lib) return -1;
    memset(&tf, 0, sizeof tf);
    RESOLVE(init, ttf_init_fn, "TTF_Init");
    RESOLVE(quit, ttf_void_fn, "TTF_Quit");
    RESOLVE(open_font, ttf_open_fn, "TTF_OpenFont");
    RESOLVE(close_font, ttf_close_fn, "TTF_CloseFont");
    RESOLVE(render, ttf_render_fn, "TTF_RenderUTF8_Blended");
    RESOLVE(render_wrapped, ttf_render_wrapped_fn, "TTF_RenderUTF8_Blended_Wrapped");
    RESOLVE(size_utf8, ttf_size_fn, "TTF_SizeUTF8");
    RESOLVE(height, ttf_metric_fn, "TTF_FontHeight");
    RESOLVE(ascent, ttf_metric_fn, "TTF_FontAscent");
    RESOLVE(descent, ttf_metric_fn, "TTF_FontDescent");
    RESOLVE(line_skip, ttf_metric_fn, "TTF_FontLineSkip");
    RESOLVE(get_style, ttf_metric_fn, "TTF_GetFontStyle");
    RESOLVE(set_style, ttf_set_style_fn, "TTF_SetFontStyle");
    RESOLVE(set_size, ttf_set_size_fn, "TTF_SetFontSize");
    RESOLVE(family, ttf_family_fn, "TTF_FontFaceFamilyName");
    RESOLVE(version, ttf_version_fn, "TTF_Linked_Version");
    if (tf.init() != 0) {
        zsdl_dyn_close(lib);
        memset(&tf, 0, sizeof tf);
        return -1;
    }
    tf.lib = lib;
    return 0;
}

void zsdl_ttf_close(void) {
    if (!tf.lib) return;
    tf.quit();
    zsdl_dyn_close(tf.lib);
    memset(&tf, 0, sizeof tf);
}

const char *zsdl_ttf_version(void) {
    static char buf[32];
    if (!tf.lib) return "";
    const SDL_version *v = tf.version();
    SDL_snprintf(buf, sizeof buf, "%d.%d.%d", v->major, v->minor, v->patch);
    return buf;
}

static int need_loaded(void) {
    if (tf.lib) return 1;
    SDL_SetError("SDL2_ttf is not loaded: call Sdl2.Ttf.Init() first");
    return 0;
}

void *zsdl_ttf_font_open(const char *path, int size) { return need_loaded() ? tf.open_font(path, size) : NULL; }
void zsdl_ttf_font_close(void *f) { if (tf.lib && f) tf.close_font(f); }
int zsdl_ttf_font_set_size(void *f, int size) { return need_loaded() ? tf.set_size(f, size) : -1; }
int zsdl_ttf_font_height(void *f) { return need_loaded() ? tf.height(f) : 0; }
int zsdl_ttf_font_ascent(void *f) { return need_loaded() ? tf.ascent(f) : 0; }
int zsdl_ttf_font_descent(void *f) { return need_loaded() ? tf.descent(f) : 0; }
int zsdl_ttf_font_line_skip(void *f) { return need_loaded() ? tf.line_skip(f) : 0; }
void zsdl_ttf_font_set_style(void *f, int style) { if (tf.lib) tf.set_style(f, style); }
int zsdl_ttf_font_style(void *f) { return tf.lib ? tf.get_style(f) : 0; }
const char *zsdl_ttf_font_family(void *f) {
    const char *n = tf.lib ? tf.family(f) : NULL;
    return n ? n : "";
}

// Size of `text` in pixels (UTF-8). Returns 0 and fills *w and *h, or -1.
int zsdl_ttf_text_w(void *f, const char *text) {
    int w = 0, h = 0;
    if (!need_loaded() || tf.size_utf8(f, text, &w, &h) != 0) return -1;
    return w;
}
int zsdl_ttf_text_h(void *f, const char *text) {
    int w = 0, h = 0;
    if (!need_loaded() || tf.size_utf8(f, text, &w, &h) != 0) return -1;
    return h;
}

// ---- rendering: a surface handle you read once and free ----

// Antialiased text as an ARGB8888 surface. wrap > 0 breaks lines at that pixel width (and at '\n').
void *zsdl_ttf_render(void *f, const char *text, int r, int g, int b, int a, int wrap) {
    if (!need_loaded()) return NULL;
    SDL_Color c = {(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
    SDL_Surface *s = wrap > 0 ? tf.render_wrapped(f, text, c, (Uint32)wrap) : tf.render(f, text, c);
    if (!s) return NULL;
    SDL_Surface *argb = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(s);
    return argb;
}
int zsdl_ttf_surf_w(void *s) { return ((SDL_Surface *)s)->w; }
int zsdl_ttf_surf_h(void *s) { return ((SDL_Surface *)s)->h; }
// Copies the pixels, row by row, into `out` (w*h Uint32) without the surface's row padding.
int zsdl_ttf_surf_copy(void *s, void *out) {
    SDL_Surface *sf = (SDL_Surface *)s;
    if (SDL_MUSTLOCK(sf) && SDL_LockSurface(sf) != 0) return -1;
    for (int y = 0; y < sf->h; y++)
        memcpy((Uint8 *)out + (size_t)y * sf->w * 4, (Uint8 *)sf->pixels + (size_t)y * sf->pitch, (size_t)sf->w * 4);
    if (SDL_MUSTLOCK(sf)) SDL_UnlockSurface(sf);
    return 0;
}
void zsdl_ttf_surf_free(void *s) { if (s) SDL_FreeSurface((SDL_Surface *)s); }
