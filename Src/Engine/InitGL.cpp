#include "Engine.hpp"
#include "../Game/Game.hpp"
#include <thread>

using sc = std::chrono::steady_clock;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_GLContext context;

uint32_t fps = 0;
bool gl_loaded = false;

void reload_graphics() {
    if(!gl_loaded) return;
    deinit_gl();
    init_gl();
}

command(reload_graphics, { reload_graphics(); });
rcommand(getfps, { ret += fps; });

ivarpf(width, 420, 1920, 8192, reload_graphics);
ivarpf(height, 360, 1080, 4320, reload_graphics);
ivarpf(antialias_samples, 1, 8, 16, reload_graphics);
bvarpf(fullscreen, true, reload_graphics);
bvarpf(fullscreen_windowed, true, [](){ if(fullscreen) reload_graphics(); });
bvarpf(texture_compression, true, reload_graphics);

ivarp(maxfps, 0, 0, 1000);
ivarp(menufps, 0, 60, 1000);

bool quit = false;

bool init_gl() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    
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

    gl_loaded = true;

    return true;
}

void deinit_gl() {
    if(renderer) SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    if(window) SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}

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
                        width = e.window.data1;
                        height = e.window.data2;
                        break;
                    }
                }
                break;
            }
            case SDL_KEYDOWN: {
                //switch(e.key.keysym.scancode) {
                //    default: { }
                //}
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
