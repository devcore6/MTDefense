#include "engine.hpp"
#include "../game/game.hpp"
#include <thread>
#include "Audio.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_GLContext context;

uint32_t fps = 0;
bool gl_loaded = false;

void reload_graphics() {
    if(!gl_loaded) return;
    if(fullscreen) {
        if(fullscreen_windowed) SDL_SetWindowFullscreen(window, SDL_WINDOW_BORDERLESS);
        else SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_DisplayMode mode { };
        SDL_GetWindowDisplayMode(window, &mode);
        mode.w = (int)width;
        mode.h = (int)height;
        SDL_SetWindowDisplayMode(window, &mode);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, (int)width, (int)height);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

command(reload_graphics, [](std::vector<std::string>& args) { reload_graphics(); });
rcommand(getfps, [](std::vector<std::string>& args) -> std::string { return std::to_string(fps); });

ivarp(width, 420, 1920, 8192);
ivarp(height, 360, 1080, 4320);
ivarp(antialias_samples, 1, 8, 16);
bvarp(fullscreen, true);
bvarp(fullscreen_windowed, true);
bvarp(texture_compression, false);

ivarp(maxfps, 0, 0, 1000);
ivarp(menufps, 0, 60, 1000);

iconst(audio_format_u16, AUDIO_U16SYS);
iconst(audio_format_s16, AUDIO_S16SYS);
iconst(audio_format_s32, AUDIO_S32SYS);
iconst(audio_format_f32, AUDIO_F32SYS);

dvarp(audio_volume, 0.0, 0.2, 1.0);

bool quit = false;

ivarp(audio_device_frequency, 0, 44100, 192000);
ivarp(audio_device_format, AUDIO_U16SYS, MIX_DEFAULT_FORMAT, AUDIO_F32SYS);
ivarp(audio_device_channels, 1, 2, 16);
ivarp(audio_device_chunksize, 256, 4096, 65536);

bool init_gl() {
    if(gl_loaded) return true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    if(Mix_OpenAudio(
        (int)audio_device_frequency,
        (uint16_t)audio_device_format,
        (int)audio_device_channels,
        (int)audio_device_chunksize
    ) == -1) return false;

    Mix_Volume(-1, (int)(MIX_MAX_VOLUME * audio_volume));
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (int)antialias_samples);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)width, (int)height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) return false;

    context = SDL_GL_CreateContext(window);
    if(!context) return false;

    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_SetWindowMinimumSize(window, 640, 360);

    if(fullscreen) {
        if(fullscreen_windowed) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        else SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_DisplayMode mode { };
        SDL_GetWindowDisplayMode(window, &mode);
        mode.w = (int)width;
        mode.h = (int)height;
        SDL_SetWindowDisplayMode(window, &mode);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, (int)width, (int)height);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0, 0, 0, 1);

    SDL_StopTextInput();
    SDL_ShowCursor(0);

    conout("OpenGL Vendor:   "_str + reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    conout("OpenGL Renderer: "_str + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    conout("OpenGL Version:  "_str + reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    return gl_loaded = true;
}

void deinit_gl() {
    gl_loaded = false;
    Mix_CloseAudio();
    if(renderer) SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    if(window) SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}

extern void mouse_press_handler(int x, int y);
extern void mouse_release_handler(int x, int y);
extern void space_bar();

void main_loop_stub() {
    sc::time_point start = sc::now();

    SDL_Event e;

    while(SDL_PollEvent(&e) != 0) {
        switch(e.type) {
            case SDL_QUIT: { quit = true; break; }
            case SDL_WINDOWEVENT: {
                switch(e.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        width  = e.window.data1;
                        height = e.window.data2;
                        break;
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: { mouse_press_handler(e.button.x, e.button.y); break; }
            case SDL_MOUSEBUTTONUP: { mouse_release_handler(e.button.x, e.button.y); break; }
            case SDL_KEYDOWN: {
                switch(e.key.keysym.scancode) {
                    case SDL_SCANCODE_SPACE: space_bar(); break;
                }
                break;
            }
        }
    }
    
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glViewport(0, 0, (int)width, (int)height);
    gluOrtho2D(0, 1920, 1080, 0);

    render_frame();
    main_loop();

    glFlush();
    SDL_GL_SwapWindow(window);

    if(maxfps != 0) {
        std::chrono::milliseconds sleep_time = 1000_ms / maxfps - std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - start);
        if(sleep_time.count() > 0) std::this_thread::sleep_for(sleep_time);
    }
}
