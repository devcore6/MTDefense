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

export module renderer.window;

import tools.types;

import <memory>;
import <stdexcept>;
import <optional>;

class sdl_status {
public:
    sdl_status() {
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) exit(1);
        if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0) exit(1);
        if(TTF_Init() < 0) exit(1);
    }

    ~sdl_status() {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
} static status { };

export template<class T>
class window {
public:
    window(
        cstring title_v,
        int width_v,
        int height_v,
        bool fullscreen_v
    ) : r(std::make_shared<T>()),
        fullscreen(fullscreen_v),
        width(width_v),
        height(height_v),
        title(title_v) {
        wind = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            T::window_flags | SDL_WINDOW_SHOWN
        );
        if(!wind) throw std::runtime_error(std::string("Could not create window: ") + SDL_GetError());

        SDL_SetWindowResizable(wind, SDL_TRUE);
        SDL_SetWindowMinimumSize(wind, 640, 360);

        update_settings();

        r->init(wind);
        r->set_viewport({ 0.0, 0.0, (double)width, (double)height });

        SDL_StopTextInput();
        SDL_ShowCursor(1);
    }

    // Disable copy constructor
    window(copy_t<window> w) = delete;
    window& operator=(copy_t<window> w) = delete;

    window(move_t<window> w) {
        std::swap(fullscreen, w.fullscreen);
        std::swap(title,      w.title);
        std::swap(width,      w.width);
        std::swap(height,     w.height);
        std::swap(quit,       w.quit);
        std::swap(r,          w.r);
        std::swap(wind,       w.wind);
    }

    window& operator=(move_t<window> w) {
        std::swap(fullscreen, w.fullscreen);
        std::swap(title,      w.title);
        std::swap(width,      w.width);
        std::swap(height,     w.height);
        std::swap(quit,       w.quit);
        std::swap(r,          w.r);
        std::swap(wind,       w.wind);
    }

    ~window() noexcept {
        if(wind) SDL_DestroyWindow(wind);
    }

    std::optional<SDL_Event> poll() {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0) {
            switch(e.type) {
                case SDL_QUIT: { quit = true; break; }
                case SDL_WINDOWEVENT:
                {
                    switch(e.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
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

    void update() noexcept {
        r->update(wind);
    }

    const std::shared_ptr<T>get_renderer() noexcept { return r; }

private:
    bool    fullscreen = false;
    cstring title      = nullptr;
    int     width      = 0;
    int     height     = 0;
    bool    quit       = false;

    const std::shared_ptr<T> r;

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
