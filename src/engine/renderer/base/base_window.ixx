module;

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

export module renderer.base.window;

import renderer.base.renderer;

import tools.types;

import <memory>;
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

export class window {
public:
    window() = default;

    // Disable copy constructor
    window(copy_t<window> w) = delete;
    window& operator=(copy_t<window> w) = delete;

    // Disable move constructor
    window(move_t<window> w) = delete;
    window& operator=(move_t<window> w) = delete;

    virtual std::optional<SDL_Event> poll() = 0;

    virtual bool    is_fullscreen()      noexcept = 0;
    virtual cstring get_title()          noexcept = 0;
    virtual int     get_width()          noexcept = 0;
    virtual int     get_height()         noexcept = 0;
    virtual bool    should_quit()        noexcept = 0;

    virtual window& set_fullscreen(bool) noexcept = 0;
    virtual window& set_width(int)       noexcept = 0;
    virtual window& set_height(int)      noexcept = 0;

    virtual void    update()             noexcept = 0;

    virtual const std::shared_ptr<renderer>get_renderer() = 0;
};
