module;

// define APIENTRY to avoid SDL_opengl.h including <Windows.h>, which, of course
// being a Windows header, generates 259 warnings.

#if defined(_WIN32) && !defined(APIENTRY)
# define APIENTRY __stdcall
#endif

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

export module renderer.opengl.window;

import renderer.base.window;
import renderer.opengl.renderer;

import tools.types;

import <string>;
import <stdexcept>;
import <optional>;

export class opengl_window: public window {
public:
    opengl_window(
        cstring title_v,
        int width_v,
        int height_v,
        bool fullscreen_v
    ) : r(std::make_shared<opengl_renderer>()),
        fullscreen(fullscreen_v),
        width(width_v),
        height(height_v),
        title(title_v) {

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,    1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,   16);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,    1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,          1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,           24);

        wind = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        if(!wind)    throw std::runtime_error(std::string("Could not create window: ") + SDL_GetError());

        SDL_SetWindowResizable(wind, SDL_TRUE);
        SDL_SetWindowMinimumSize(wind, 640, 360);

        update_settings();

        glShadeModel(GL_SMOOTH);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 1);

        SDL_StopTextInput();
        SDL_ShowCursor(0);
    }

    ~opengl_window() noexcept {
        if(wind) SDL_DestroyWindow(wind);
    }

    std::optional<SDL_Event> poll() {
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
                            return { };
                        }
                    }
                    [[fallthrough]];
                }
                default: return e;
            }
        }
        return { };
    }

    bool    is_fullscreen()        noexcept { return fullscreen; }
    cstring get_title()            noexcept { return title; }
    int     get_width()            noexcept { return width; }
    int     get_height()           noexcept { return height; }
    bool    should_quit()          noexcept { return quit; }

    window& set_fullscreen(bool v) noexcept { fullscreen = v; update_settings(); return *this; }
    window& set_width(int v)       noexcept { width      = v; update_settings(); return *this; }
    window& set_height(int v)      noexcept { height     = v; update_settings(); return *this; }

    void    update() noexcept {

    }

    const std::shared_ptr<renderer>get_renderer() noexcept { return r; }
private:
    bool    fullscreen = false;
    cstring title      = nullptr;
    int     width      = 0;
    int     height     = 0;
    bool    quit       = false;

    const std::shared_ptr<renderer> r;

    SDL_Window* wind;

    void update_settings() noexcept {
        if(fullscreen) {
            SDL_SetWindowFullscreen(wind, SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_DisplayMode mode { };
            SDL_GetWindowDisplayMode(wind, &mode);
            mode.w = width;
            mode.h = height;
            SDL_SetWindowDisplayMode(wind, &mode);
        } else {
            SDL_SetWindowFullscreen(wind, 0);
            SDL_SetWindowSize(wind, width, height);
            SDL_SetWindowPosition(wind, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }
};
