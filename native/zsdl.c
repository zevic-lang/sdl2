// Thin C shim between Zevic and SDL2.
// Zevic passes only C scalars and opaque pointers; everything with a union,
// a by-value struct or a pointer-to-scalar lives here.
#include "zsdl_sdl.h"
#include <stdlib.h>
#include "font8x8.h"

// ---- lifecycle ----
// We have no SDL_main: tell SDL so (SDL's documented route on Windows; a no-op elsewhere).
int zsdl_init(int flags) {
    SDL_SetMainReady();
    return SDL_Init((Uint32)flags);
}
void zsdl_quit(void) { SDL_Quit(); }
const char *zsdl_error(void) { return SDL_GetError(); }

// ---- window / renderer ----
void *zsdl_window_new(const char *title, int x, int y, int w, int h, int flags) {
    return SDL_CreateWindow(title, x, y, w, h, (Uint32)flags);
}
void zsdl_window_free(void *w) { if (w) SDL_DestroyWindow((SDL_Window *)w); }
void zsdl_window_set_title(void *w, const char *t) { SDL_SetWindowTitle((SDL_Window *)w, t); }
int zsdl_window_w(void *w) { int a = 0, b = 0; SDL_GetWindowSize((SDL_Window *)w, &a, &b); return a; }
int zsdl_window_h(void *w) { int a = 0, b = 0; SDL_GetWindowSize((SDL_Window *)w, &a, &b); return b; }

void *zsdl_renderer_new(void *win, int flags) { return SDL_CreateRenderer((SDL_Window *)win, -1, (Uint32)flags); }
void zsdl_renderer_free(void *r) { if (r) SDL_DestroyRenderer((SDL_Renderer *)r); }
int zsdl_set_color(void *r, int cr, int cg, int cb, int ca) {
    return SDL_SetRenderDrawColor((SDL_Renderer *)r, (Uint8)cr, (Uint8)cg, (Uint8)cb, (Uint8)ca);
}
int zsdl_clear(void *r) { return SDL_RenderClear((SDL_Renderer *)r); }
void zsdl_present(void *r) { SDL_RenderPresent((SDL_Renderer *)r); }
int zsdl_draw_point(void *r, int x, int y) { return SDL_RenderDrawPoint((SDL_Renderer *)r, x, y); }
int zsdl_draw_line(void *r, int x1, int y1, int x2, int y2) {
    return SDL_RenderDrawLine((SDL_Renderer *)r, x1, y1, x2, y2);
}
int zsdl_draw_rect(void *r, int x, int y, int w, int h) {
    SDL_Rect rc = {x, y, w, h};
    return SDL_RenderDrawRect((SDL_Renderer *)r, &rc);
}
int zsdl_fill_rect(void *r, int x, int y, int w, int h) {
    SDL_Rect rc = {x, y, w, h};
    return SDL_RenderFillRect((SDL_Renderer *)r, &rc);
}

// ---- streaming texture (ARGB8888 framebuffer) ----
void *zsdl_texture_new(void *r, int w, int h) {
    return SDL_CreateTexture((SDL_Renderer *)r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
}
void zsdl_texture_free(void *t) { if (t) SDL_DestroyTexture((SDL_Texture *)t); }
int zsdl_texture_update(void *t, const void *pixels, int pitch) {
    return SDL_UpdateTexture((SDL_Texture *)t, NULL, pixels, pitch);
}
int zsdl_texture_draw(void *r, void *t) { return SDL_RenderCopy((SDL_Renderer *)r, (SDL_Texture *)t, NULL, NULL); }

// ---- time ----
long long zsdl_ticks(void) { return (long long)SDL_GetTicks64(); }
void zsdl_delay(int ms) { SDL_Delay((Uint32)ms); }

// ---- events: SDL_Event is a union, so Zevic keeps an opaque buffer ----
void *zsdl_event_new(void) { return calloc(1, sizeof(SDL_Event)); }
void zsdl_event_free(void *e) { free(e); }
int zsdl_poll(void *e) { return SDL_PollEvent((SDL_Event *)e); }
int zsdl_ev_type(void *e) { return (int)((SDL_Event *)e)->type; }
int zsdl_ev_key(void *e) { return (int)((SDL_Event *)e)->key.keysym.sym; }
int zsdl_ev_scancode(void *e) { return (int)((SDL_Event *)e)->key.keysym.scancode; }
int zsdl_ev_repeat(void *e) { return (int)((SDL_Event *)e)->key.repeat; }
int zsdl_ev_mouse_x(void *e) { return ((SDL_Event *)e)->motion.x; }
int zsdl_ev_mouse_y(void *e) { return ((SDL_Event *)e)->motion.y; }
int zsdl_ev_button(void *e) { return (int)((SDL_Event *)e)->button.button; }
int zsdl_ev_wheel_y(void *e) { return ((SDL_Event *)e)->wheel.y; }
int zsdl_ev_window_event(void *e) { return (int)((SDL_Event *)e)->window.event; }

// ---- keyboard / mouse state ----
int zsdl_key_down(int scancode) {
    const Uint8 *st = SDL_GetKeyboardState(NULL);
    return st && st[scancode] ? 1 : 0;
}
int zsdl_mouse_x(void) { int x = 0, y = 0; SDL_GetMouseState(&x, &y); return x; }
int zsdl_mouse_y(void) { int x = 0, y = 0; SDL_GetMouseState(&x, &y); return y; }
int zsdl_mouse_buttons(void) { return (int)SDL_GetMouseState(NULL, NULL); }

// ---- constants (so Zevic need not copy header values) ----
int zsdl_const(int id) {
    switch (id) {
    case 0: return SDL_INIT_VIDEO;
    case 1: return SDL_INIT_AUDIO;
    case 2: return SDL_INIT_TIMER;
    case 3: return SDL_INIT_EVENTS;
    case 10: return SDL_WINDOW_SHOWN;
    case 11: return SDL_WINDOW_RESIZABLE;
    case 12: return SDL_WINDOW_FULLSCREEN_DESKTOP;
    case 13: return SDL_WINDOW_HIDDEN;
    case 20: return SDL_RENDERER_ACCELERATED;
    case 21: return SDL_RENDERER_PRESENTVSYNC;
    case 22: return SDL_RENDERER_SOFTWARE;
    case 30: return SDL_QUIT;
    case 31: return SDL_KEYDOWN;
    case 32: return SDL_KEYUP;
    case 33: return SDL_MOUSEMOTION;
    case 34: return SDL_MOUSEBUTTONDOWN;
    case 35: return SDL_MOUSEBUTTONUP;
    case 36: return SDL_MOUSEWHEEL;
    case 37: return SDL_WINDOWEVENT;
    case 40: return SDL_WINDOWEVENT_RESIZED;
    case 41: return SDL_WINDOWEVENT_CLOSE;
    case 50: return SDL_WINDOWPOS_CENTERED;
    case 51: return SDL_WINDOWPOS_UNDEFINED;
    case 60: return SDL_INIT_AUDIO;
    case 61: return SDL_INIT_GAMECONTROLLER;
    case 62: return SDL_WINDOW_FULLSCREEN;
    case 63: return SDL_WINDOW_BORDERLESS;
    case 70: return SDL_TEXTINPUT;
    case 71: return SDL_CONTROLLERBUTTONDOWN;
    case 72: return SDL_CONTROLLERBUTTONUP;
    case 73: return SDL_WINDOWEVENT_FOCUS_GAINED;
    case 74: return SDL_WINDOWEVENT_FOCUS_LOST;
    case 75: return SDL_CONTROLLERDEVICEADDED;
    case 76: return SDL_CONTROLLERDEVICEREMOVED;
    default: return -1;
    }
}

// Queue an SDL_QUIT event (lets a program end its own main loop).
int zsdl_push_quit(void) {
    SDL_Event e;
    SDL_zero(e);
    e.type = SDL_QUIT;
    return SDL_PushEvent(&e);
}

// =====================================================================
// v0.2 additions. Still portable SDL2 only: no OS-specific calls.
// =====================================================================

// ---- hints / headless ----
void zsdl_hint(const char *name, const char *value) { SDL_SetHint(name, value); }
int zsdl_init_sub(int flags) {
    SDL_SetMainReady();
    return SDL_InitSubSystem((Uint32)flags);
}
void zsdl_quit_sub(int flags) { SDL_QuitSubSystem((Uint32)flags); }
const char *zsdl_version(void) {
    static char buf[32];
    SDL_version v;
    SDL_GetVersion(&v);
    SDL_snprintf(buf, sizeof buf, "%d.%d.%d", v.major, v.minor, v.patch);
    return buf;
}

// ---- window extras ----
void zsdl_window_set_size(void *w, int cw, int ch) { SDL_SetWindowSize((SDL_Window *)w, cw, ch); }
void zsdl_window_set_pos(void *w, int x, int y) { SDL_SetWindowPosition((SDL_Window *)w, x, y); }
int zsdl_window_x(void *w) { int a = 0, b = 0; SDL_GetWindowPosition((SDL_Window *)w, &a, &b); return a; }
int zsdl_window_y(void *w) { int a = 0, b = 0; SDL_GetWindowPosition((SDL_Window *)w, &a, &b); return b; }
void zsdl_window_min_size(void *w, int cw, int ch) { SDL_SetWindowMinimumSize((SDL_Window *)w, cw, ch); }
int zsdl_window_fullscreen(void *w, int mode) { return SDL_SetWindowFullscreen((SDL_Window *)w, (Uint32)mode); }
void zsdl_window_show(void *w, int on) { if (on) SDL_ShowWindow((SDL_Window *)w); else SDL_HideWindow((SDL_Window *)w); }
void zsdl_window_raise(void *w) { SDL_RaiseWindow((SDL_Window *)w); }
void zsdl_window_resizable(void *w, int on) { SDL_SetWindowResizable((SDL_Window *)w, on ? SDL_TRUE : SDL_FALSE); }
int zsdl_display_w(int idx) { SDL_Rect r; if (SDL_GetDisplayBounds(idx, &r) != 0) return -1; return r.w; }
int zsdl_display_h(int idx) { SDL_Rect r; if (SDL_GetDisplayBounds(idx, &r) != 0) return -1; return r.h; }

// ---- renderer extras ----
int zsdl_set_blend(void *r, int mode) {
    SDL_BlendMode m = mode == 1 ? SDL_BLENDMODE_BLEND : mode == 2 ? SDL_BLENDMODE_ADD
                    : mode == 3 ? SDL_BLENDMODE_MOD : SDL_BLENDMODE_NONE;
    return SDL_SetRenderDrawBlendMode((SDL_Renderer *)r, m);
}
int zsdl_set_logical(void *r, int w, int h) { return SDL_RenderSetLogicalSize((SDL_Renderer *)r, w, h); }
int zsdl_set_clip(void *r, int x, int y, int w, int h) {
    SDL_Rect rc = {x, y, w, h};
    return SDL_RenderSetClipRect((SDL_Renderer *)r, &rc);
}
int zsdl_clear_clip(void *r) { return SDL_RenderSetClipRect((SDL_Renderer *)r, NULL); }
int zsdl_output_w(void *r) { int a = 0, b = 0; SDL_GetRendererOutputSize((SDL_Renderer *)r, &a, &b); return a; }
int zsdl_output_h(void *r) { int a = 0, b = 0; SDL_GetRendererOutputSize((SDL_Renderer *)r, &a, &b); return b; }
// Reads back the frame as ARGB8888 into `pixels` (w*h Uint32). Handy for tests and screenshots.
int zsdl_read_pixels(void *r, int x, int y, int w, int h, void *pixels) {
    SDL_Rect rc = {x, y, w, h};
    return SDL_RenderReadPixels((SDL_Renderer *)r, &rc, SDL_PIXELFORMAT_ARGB8888, pixels, w * 4);
}

// ---- textures (static, from ARGB pixels or a BMP file) ----
void *zsdl_texture_from_pixels(void *r, const void *pixels, int w, int h) {
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormatFrom((void *)pixels, w, h, 32, w * 4, SDL_PIXELFORMAT_ARGB8888);
    if (!s) return NULL;
    SDL_Texture *t = SDL_CreateTextureFromSurface((SDL_Renderer *)r, s);
    SDL_FreeSurface(s);
    if (t) SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t;
}
void *zsdl_texture_from_bmp(void *r, const char *path) {
    SDL_Surface *s = SDL_LoadBMP(path);
    if (!s) return NULL;
    SDL_Texture *t = SDL_CreateTextureFromSurface((SDL_Renderer *)r, s);
    SDL_FreeSurface(s);
    if (t) SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t;
}
int zsdl_texture_w(void *t) { int w = 0, h = 0; SDL_QueryTexture((SDL_Texture *)t, NULL, NULL, &w, &h); return w; }
int zsdl_texture_h(void *t) { int w = 0, h = 0; SDL_QueryTexture((SDL_Texture *)t, NULL, NULL, &w, &h); return h; }
int zsdl_texture_color(void *t, int r, int g, int b, int a) {
    int e = SDL_SetTextureColorMod((SDL_Texture *)t, (Uint8)r, (Uint8)g, (Uint8)b);
    return e != 0 ? e : SDL_SetTextureAlphaMod((SDL_Texture *)t, (Uint8)a);
}
// Copies the whole texture (or the sx,sy,sw,sh part when sw > 0) to dx,dy,dw,dh, rotated about its centre.
int zsdl_texture_copy(void *r, void *t, int sx, int sy, int sw, int sh,
                      int dx, int dy, int dw, int dh, double angle, int flip) {
    SDL_Rect src = {sx, sy, sw, sh};
    SDL_Rect dst = {dx, dy, dw, dh};
    int f = SDL_FLIP_NONE;
    if (flip & 1) f |= SDL_FLIP_HORIZONTAL;
    if (flip & 2) f |= SDL_FLIP_VERTICAL;
    return SDL_RenderCopyEx((SDL_Renderer *)r, (SDL_Texture *)t, sw > 0 ? &src : NULL, &dst, angle, NULL, (SDL_RendererFlip)f);
}

// ---- more event fields ----
int zsdl_ev_mod(void *e) { return (int)((SDL_Event *)e)->key.keysym.mod; }
const char *zsdl_ev_text(void *e) { return ((SDL_Event *)e)->text.text; }
int zsdl_ev_wheel_x(void *e) { return ((SDL_Event *)e)->wheel.x; }
int zsdl_ev_win_w(void *e) { return ((SDL_Event *)e)->window.data1; }
int zsdl_ev_win_h(void *e) { return ((SDL_Event *)e)->window.data2; }
int zsdl_ev_pad_button(void *e) { return (int)((SDL_Event *)e)->cbutton.button; }
int zsdl_ev_pad_which(void *e) { return (int)((SDL_Event *)e)->cbutton.which; }
void zsdl_text_input(int on) { if (on) SDL_StartTextInput(); else SDL_StopTextInput(); }
int zsdl_mod_state(void) { return (int)SDL_GetModState(); }

// ---- clipboard ----
static char *clip_buf = NULL;
const char *zsdl_clipboard_get(void) {
    if (clip_buf) { SDL_free(clip_buf); clip_buf = NULL; }
    clip_buf = SDL_GetClipboardText();
    return clip_buf ? clip_buf : "";
}
int zsdl_clipboard_set(const char *text) { return SDL_SetClipboardText(text); }

// ---- audio: a queue device (no callbacks), signed 16-bit samples, interleaved ----
int zsdl_audio_open(int freq, int channels) {
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = freq;
    want.format = AUDIO_S16SYS;
    want.channels = (Uint8)channels;
    want.samples = 1024;
    SDL_AudioDeviceID id = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (id == 0) return 0;
    SDL_PauseAudioDevice(id, 0);
    return (int)id;
}
void zsdl_audio_close(int dev) { SDL_CloseAudioDevice((SDL_AudioDeviceID)dev); }
int zsdl_audio_queue(int dev, const void *data, int bytes) { return SDL_QueueAudio((SDL_AudioDeviceID)dev, data, (Uint32)bytes); }
int zsdl_audio_queued(int dev) { return (int)SDL_GetQueuedAudioSize((SDL_AudioDeviceID)dev); }
void zsdl_audio_clear(int dev) { SDL_ClearQueuedAudio((SDL_AudioDeviceID)dev); }
void zsdl_audio_pause(int dev, int on) { SDL_PauseAudioDevice((SDL_AudioDeviceID)dev, on); }

// ---- game controllers ----
int zsdl_pad_count(void) { return SDL_NumJoysticks(); }
int zsdl_pad_is_controller(int idx) { return SDL_IsGameController(idx) ? 1 : 0; }
void *zsdl_pad_open(int idx) { return SDL_GameControllerOpen(idx); }
void zsdl_pad_close(void *p) { if (p) SDL_GameControllerClose((SDL_GameController *)p); }
const char *zsdl_pad_name(void *p) { const char *n = SDL_GameControllerName((SDL_GameController *)p); return n ? n : ""; }
int zsdl_pad_button(void *p, int b) { return SDL_GameControllerGetButton((SDL_GameController *)p, (SDL_GameControllerButton)b); }
int zsdl_pad_axis(void *p, int a) { return SDL_GameControllerGetAxis((SDL_GameController *)p, (SDL_GameControllerAxis)a); }
int zsdl_pad_id(void *p) {
    return (int)SDL_JoystickInstanceID(SDL_GameControllerGetJoystick((SDL_GameController *)p));
}

// ---- timing ----
long long zsdl_perf_counter(void) { return (long long)SDL_GetPerformanceCounter(); }
long long zsdl_perf_freq(void) { return (long long)SDL_GetPerformanceFrequency(); }

// ---- message box (portable SDL2 dialog) ----
int zsdl_message_box(void *win, int kind, const char *title, const char *msg) {
    Uint32 flags = kind == 2 ? SDL_MESSAGEBOX_ERROR : kind == 1 ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    return SDL_ShowSimpleMessageBox(flags, title, msg, (SDL_Window *)win);
}

// ---- bitmap text (8x8 font, ASCII) ----
// Draws `text` in the current draw colour. Each glyph cell is 8*scale pixels; '\n' starts a new line.
// Bytes outside ASCII draw as '?' (one per UTF-8 character).
static int text_glyph(int c) {
    if (c < 32 || c > 126) return '?';
    return c;
}
int zsdl_draw_text(void *r, int x, int y, const char *text, int scale) {
    if (scale < 1) scale = 1;
    int cx = x, cy = y;
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if (*p == '\n') { cx = x; cy += 8 * scale; continue; }
        if (*p >= 0x80 && *p < 0xC0) continue;       /* UTF-8 continuation byte */
        const unsigned char *g = font8x8_basic[text_glyph(*p)];
        for (int row = 0; row < 8; row++) {
            int bits = g[row];
            int col = 0;
            while (col < 8) {
                if (!(bits & (1 << col))) { col++; continue; }
                int start = col;
                while (col < 8 && (bits & (1 << col))) col++;
                SDL_Rect rc = {cx + start * scale, cy + row * scale, (col - start) * scale, scale};
                if (SDL_RenderFillRect((SDL_Renderer *)r, &rc) != 0) return -1;
            }
        }
        cx += 8 * scale;
    }
    return 0;
}
// Width in pixels of the widest line, and the height of all lines, for the same rules as above.
int zsdl_text_width(const char *text, int scale) {
    if (scale < 1) scale = 1;
    int best = 0, cur = 0;
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if (*p == '\n') { cur = 0; continue; }
        if (*p >= 0x80 && *p < 0xC0) continue;
        cur += 8 * scale;
        if (cur > best) best = cur;
    }
    return best;
}
int zsdl_text_height(const char *text, int scale) {
    if (scale < 1) scale = 1;
    int lines = 1;
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) if (*p == '\n') lines++;
    return lines * 8 * scale;
}

// ---- screenshots ----
// Writes what the renderer shows (the current target, before Present) to a .bmp file. 0 on success.
int zsdl_save_bmp(void *r, const char *path) {
    int w = 0, h = 0;
    if (SDL_GetRendererOutputSize((SDL_Renderer *)r, &w, &h) != 0) return -1;
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!s) return -1;
    int rc = SDL_RenderReadPixels((SDL_Renderer *)r, NULL, SDL_PIXELFORMAT_ARGB8888, s->pixels, s->pitch);
    if (rc == 0) rc = SDL_SaveBMP(s, path);
    SDL_FreeSurface(s);
    return rc;
}

// =====================================================================
// v0.5 additions: shapes, mouse state, render targets, window extras, a smoke-test hook.
// =====================================================================

// ---- key names (the tests use them to check the scancode constants against SDL's own table) ----
int zsdl_scancode(const char *name) { return (int)SDL_GetScancodeFromName(name); }
const char *zsdl_key_name(int key) { return SDL_GetKeyName((SDL_Keycode)key); }

// ---- mouse ----
static int rel_dx = 0, rel_dy = 0;
// Movement since the last call; one call reads both numbers (zsdl_mouse_rel_y returns the second).
int zsdl_mouse_rel_x(void) { rel_dx = 0; rel_dy = 0; SDL_GetRelativeMouseState(&rel_dx, &rel_dy); return rel_dx; }
int zsdl_mouse_rel_y(void) { return rel_dy; }
int zsdl_relative_mouse(int on) { return SDL_SetRelativeMouseMode(on ? SDL_TRUE : SDL_FALSE); }
int zsdl_show_cursor(int on) { return SDL_ShowCursor(on ? SDL_ENABLE : SDL_DISABLE); }

// ---- shapes ----
static int hline(SDL_Renderer *r, int x1, int x2, int y) { return SDL_RenderDrawLine(r, x1, y, x2, y); }

int zsdl_circle(void *rr, int cx, int cy, int rad, int fill) {
    SDL_Renderer *r = (SDL_Renderer *)rr;
    if (rad < 0) return -1;
    if (fill) {
        for (int dy = -rad; dy <= rad; dy++) {
            int dx = (int)SDL_sqrt((double)rad * rad - (double)dy * dy + 0.5);
            if (hline(r, cx - dx, cx + dx, cy + dy) != 0) return -1;
        }
        return 0;
    }
    // midpoint circle: eight symmetric points per step, each point drawn once
    int cap = 8 * (rad + 1);
    SDL_Point *pts = (SDL_Point *)malloc(sizeof(SDL_Point) * (size_t)cap);
    if (!pts) return -1;
    int n = 0, x = rad, y = 0, err = 1 - rad;
    while (x >= y) {
        int px[8] = {cx + x, cx - x, cx + x, cx - x, cx + y, cx - y, cx + y, cx - y};
        int py[8] = {cy + y, cy + y, cy - y, cy - y, cy + x, cy + x, cy - x, cy - x};
        for (int i = 0; i < 8; i++) {
            int dup = 0;                                     // on an axis or a diagonal some of the eight coincide
            for (int j = 0; j < i && !dup; j++) dup = px[i] == px[j] && py[i] == py[j];
            if (dup) continue;
            pts[n].x = px[i];
            pts[n].y = py[i];
            n++;
        }
        y++;
        if (err < 0) err += 2 * y + 1;
        else { x--; err += 2 * (y - x) + 1; }
    }
    int rc = n > 0 ? SDL_RenderDrawPoints(r, pts, n) : 0;
    free(pts);
    return rc;
}

int zsdl_triangle(void *rr, int x1, int y1, int x2, int y2, int x3, int y3, int fill) {
    SDL_Renderer *r = (SDL_Renderer *)rr;
    if (!fill) {
        if (SDL_RenderDrawLine(r, x1, y1, x2, y2) != 0) return -1;
        if (SDL_RenderDrawLine(r, x2, y2, x3, y3) != 0) return -1;
        return SDL_RenderDrawLine(r, x3, y3, x1, y1);
    }
    // order the corners by y, then fill row by row between the long edge and the two short ones
    int t;
    if (y1 > y2) { t = x1; x1 = x2; x2 = t; t = y1; y1 = y2; y2 = t; }
    if (y2 > y3) { t = x2; x2 = x3; x3 = t; t = y2; y2 = y3; y3 = t; }
    if (y1 > y2) { t = x1; x1 = x2; x2 = t; t = y1; y1 = y2; y2 = t; }
    if (y1 == y3) {                                          // flat: one row
        int lo = x1 < x2 ? x1 : x2; lo = lo < x3 ? lo : x3;
        int hi = x1 > x2 ? x1 : x2; hi = hi > x3 ? hi : x3;
        return hline(r, lo, hi, y1);
    }
    for (int y = y1; y <= y3; y++) {
        long long xa = x1 + (long long)(x3 - x1) * (y - y1) / (y3 - y1);
        long long xb;
        if (y < y2) xb = x1 + (long long)(x2 - x1) * (y - y1) / (y2 - y1);
        else if (y3 == y2) xb = x2;
        else xb = x2 + (long long)(x3 - x2) * (y - y2) / (y3 - y2);
        int lo = (int)(xa < xb ? xa : xb), hi = (int)(xa < xb ? xb : xa);
        if (hline(r, lo, hi, y) != 0) return -1;
    }
    return 0;
}

// ---- render targets: a texture you can draw into, and back to the window ----
void *zsdl_canvas_new(void *rr, int w, int h) {
    SDL_Renderer *r = (SDL_Renderer *)rr;
    SDL_Texture *t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!t) return NULL;
    SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    // a new texture holds nothing defined: make it transparent
    SDL_Texture *old = SDL_GetRenderTarget(r);
    Uint8 cr, cg, cb, ca;
    SDL_BlendMode bm;
    SDL_GetRenderDrawColor(r, &cr, &cg, &cb, &ca);
    SDL_GetRenderDrawBlendMode(r, &bm);
    SDL_SetRenderTarget(r, t);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
    SDL_RenderClear(r);
    SDL_SetRenderDrawBlendMode(r, bm);
    SDL_SetRenderDrawColor(r, cr, cg, cb, ca);
    SDL_SetRenderTarget(r, old);
    return t;
}
int zsdl_set_target(void *r, void *t) { return SDL_SetRenderTarget((SDL_Renderer *)r, (SDL_Texture *)t); }
int zsdl_texture_blend(void *t, int mode) {
    SDL_BlendMode m = mode == 1 ? SDL_BLENDMODE_BLEND : mode == 2 ? SDL_BLENDMODE_ADD
                    : mode == 3 ? SDL_BLENDMODE_MOD : SDL_BLENDMODE_NONE;
    return SDL_SetTextureBlendMode((SDL_Texture *)t, m);
}

// ---- window extras ----
int zsdl_window_icon(void *w, const void *pixels, int iw, int ih) {
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormatFrom((void *)pixels, iw, ih, 32, iw * 4, SDL_PIXELFORMAT_ARGB8888);
    if (!s) return -1;
    SDL_SetWindowIcon((SDL_Window *)w, s);   // SDL copies the icon
    SDL_FreeSurface(s);
    return 0;
}
void zsdl_window_minimize(void *w) { SDL_MinimizeWindow((SDL_Window *)w); }
void zsdl_window_maximize(void *w) { SDL_MaximizeWindow((SDL_Window *)w); }
void zsdl_window_restore(void *w) { SDL_RestoreWindow((SDL_Window *)w); }
void zsdl_window_bordered(void *w, int on) { SDL_SetWindowBordered((SDL_Window *)w, on ? SDL_TRUE : SDL_FALSE); }

// ---- smoke-test hook for examples: ZEVIC_SDL2_FRAMES=N makes a program end after N frames ----
int zsdl_env_int(const char *name, int dflt) {
    const char *v = SDL_getenv(name);
    return v && *v ? atoi(v) : dflt;
}
const char *zsdl_env_str(const char *name) {
    const char *v = SDL_getenv(name);
    return v ? v : "";
}
