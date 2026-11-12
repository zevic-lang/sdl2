// SDL2_mixer, loaded at run time (see zsdl_dyn.h). Sound effects (chunks) and one music stream.
#include "zsdl_dyn.h"
#include "zsdl_sdl.h"
#include <string.h>

typedef int (*mix_init_fn)(int);
typedef void (*mix_void_fn)(void);
typedef int (*mix_open_fn)(int, Uint16, int, int);
typedef void *(*mix_load_wav_fn)(SDL_RWops *, int);
typedef void *(*mix_load_mus_fn)(const char *);
typedef void (*mix_free_fn)(void *);
typedef int (*mix_play_fn)(int, void *, int, int);
typedef int (*mix_play_mus_fn)(void *, int);
typedef int (*mix_vol_chunk_fn)(void *, int);
typedef int (*mix_int_fn)(int);
typedef int (*mix_int_void_fn)(void);
typedef int (*mix_vol_chan_fn)(int, int);
typedef const SDL_version *(*mix_version_fn)(void);

static struct {
    void *lib;
    mix_init_fn init;
    mix_void_fn quit, close_audio, pause_music, resume_music;
    mix_open_fn open_audio;
    mix_load_wav_fn load_wav;
    mix_load_mus_fn load_mus;
    mix_free_fn free_chunk, free_music;
    mix_play_fn play;
    mix_play_mus_fn play_music;
    mix_vol_chunk_fn vol_chunk;
    mix_int_fn volume_music, playing, halt_channel, allocate;
    mix_int_void_fn halt_music, playing_music, paused_music;
    mix_vol_chan_fn volume;
    mix_version_fn version;
    int audio_open;
} mx;

static const char *const mix_names[] = {
#if defined(_WIN32)
    "SDL2_mixer.dll",
#elif defined(__APPLE__)
    "libSDL2_mixer-2.0.0.dylib", "libSDL2_mixer.dylib",
    "/opt/homebrew/lib/libSDL2_mixer-2.0.0.dylib", "/usr/local/lib/libSDL2_mixer-2.0.0.dylib",
    "SDL2_mixer.framework/SDL2_mixer",
#else
    "libSDL2_mixer-2.0.so.0", "libSDL2_mixer.so", "libSDL2_mixer-2.0.so",
#endif
    NULL};

#define RESOLVE(field, type, name)                                  \
    do {                                                            \
        mx.field = (type)zsdl_dyn_sym(lib, name);                   \
        if (!mx.field) { zsdl_dyn_close(lib); memset(&mx, 0, sizeof mx); return -1; } \
    } while (0)

int zsdl_mix_loaded(void) { return mx.lib != NULL; }

// Loads the library and opens the audio device. 0 on success, -1 on failure (see zsdl_error).
int zsdl_mix_open(const char *path, int freq, int channels, int chunk) {
    if (mx.lib) return 0;
    void *lib = zsdl_dyn_open("ZEVIC_SDL2_MIXER", path, mix_names);
    if (!lib) return -1;
    memset(&mx, 0, sizeof mx);
    RESOLVE(init, mix_init_fn, "Mix_Init");
    RESOLVE(quit, mix_void_fn, "Mix_Quit");
    RESOLVE(open_audio, mix_open_fn, "Mix_OpenAudio");
    RESOLVE(close_audio, mix_void_fn, "Mix_CloseAudio");
    RESOLVE(load_wav, mix_load_wav_fn, "Mix_LoadWAV_RW");
    RESOLVE(load_mus, mix_load_mus_fn, "Mix_LoadMUS");
    RESOLVE(free_chunk, mix_free_fn, "Mix_FreeChunk");
    RESOLVE(free_music, mix_free_fn, "Mix_FreeMusic");
    RESOLVE(play, mix_play_fn, "Mix_PlayChannelTimed");
    RESOLVE(play_music, mix_play_mus_fn, "Mix_PlayMusic");
    RESOLVE(vol_chunk, mix_vol_chunk_fn, "Mix_VolumeChunk");
    RESOLVE(volume_music, mix_int_fn, "Mix_VolumeMusic");
    RESOLVE(pause_music, mix_void_fn, "Mix_PauseMusic");
    RESOLVE(resume_music, mix_void_fn, "Mix_ResumeMusic");
    RESOLVE(halt_music, mix_int_void_fn, "Mix_HaltMusic");
    RESOLVE(playing_music, mix_int_void_fn, "Mix_PlayingMusic");
    RESOLVE(paused_music, mix_int_void_fn, "Mix_PausedMusic");
    RESOLVE(playing, mix_int_fn, "Mix_Playing");
    RESOLVE(halt_channel, mix_int_fn, "Mix_HaltChannel");
    RESOLVE(allocate, mix_int_fn, "Mix_AllocateChannels");
    RESOLVE(volume, mix_vol_chan_fn, "Mix_Volume");
    RESOLVE(version, mix_version_fn, "Mix_Linked_Version");
    mx.lib = lib;
    mx.init(0x1 | 0x8 | 0x10); /* FLAC, MP3, OGG: ignore the result, WAV always works */
    if (mx.open_audio(freq, AUDIO_S16SYS, channels, chunk) != 0) {
        mx.quit();
        zsdl_dyn_close(lib);
        memset(&mx, 0, sizeof mx);
        return -1;
    }
    mx.audio_open = 1;
    return 0;
}

void zsdl_mix_close(void) {
    if (!mx.lib) return;
    if (mx.audio_open) mx.close_audio();
    mx.quit();
    zsdl_dyn_close(mx.lib);
    memset(&mx, 0, sizeof mx);
}

const char *zsdl_mix_version(void) {
    static char buf[32];
    if (!mx.lib) return "";
    const SDL_version *v = mx.version();
    SDL_snprintf(buf, sizeof buf, "%d.%d.%d", v->major, v->minor, v->patch);
    return buf;
}

static int need_loaded(void) {
    if (mx.lib) return 1;
    SDL_SetError("SDL2_mixer is not loaded: call Sdl2.Mixer.Init() first");
    return 0;
}

void *zsdl_mix_load_sound(const char *path) {
    if (!need_loaded()) return NULL;
    SDL_RWops *rw = SDL_RWFromFile(path, "rb");
    if (!rw) return NULL;
    return mx.load_wav(rw, 1);
}
void *zsdl_mix_load_music(const char *path) { return need_loaded() ? mx.load_mus(path) : NULL; }
void zsdl_mix_free_sound(void *s) { if (mx.lib && s) mx.free_chunk(s); }
void zsdl_mix_free_music(void *m) { if (mx.lib && m) mx.free_music(m); }

// Plays a sound on the first free channel (-1) or the given one. loops: 0 once, -1 forever. Returns the channel.
int zsdl_mix_play_sound(void *s, int channel, int loops) {
    return need_loaded() ? mx.play(channel, s, loops, -1) : -1;
}
int zsdl_mix_sound_volume(void *s, int v) { return need_loaded() ? mx.vol_chunk(s, v) : -1; }
int zsdl_mix_channel_volume(int ch, int v) { return need_loaded() ? mx.volume(ch, v) : -1; }
int zsdl_mix_channel_playing(int ch) { return mx.lib ? mx.playing(ch) : 0; }
int zsdl_mix_halt_channel(int ch) { return mx.lib ? mx.halt_channel(ch) : 0; }
int zsdl_mix_channels(int n) { return need_loaded() ? mx.allocate(n) : -1; }

int zsdl_mix_play_music(void *m, int loops) { return need_loaded() ? mx.play_music(m, loops) : -1; }
void zsdl_mix_pause_music(void) { if (mx.lib) mx.pause_music(); }
void zsdl_mix_resume_music(void) { if (mx.lib) mx.resume_music(); }
int zsdl_mix_halt_music(void) { return mx.lib ? mx.halt_music() : 0; }
int zsdl_mix_music_volume(int v) { return need_loaded() ? mx.volume_music(v) : -1; }
int zsdl_mix_music_playing(void) { return mx.lib ? mx.playing_music() : 0; }
int zsdl_mix_music_paused(void) { return mx.lib ? mx.paused_music() : 0; }
