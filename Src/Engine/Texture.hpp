#pragma once
#include <vector>
#include <chrono>

#ifndef SDL_MAIN_HANDLED
# define SDL_MAIN_HANDLED
#endif // SDL_MAIN_HANDLED
#include <SDL/SDL.h>

using sc = std::chrono::system_clock;

struct texture_t {
    unsigned int                                        textid              = 0;
    unsigned int                                        width               = 0;
    unsigned int                                        height              = 0;

    texture_t()                                                             = default;
    texture_t(std::string path);
    texture_t(SDL_RWops*  data);
};

struct animation_t {
    animation_t()                                                           = default;
    animation_t(std::string dir);
    std::vector<texture_t>                              frames;
    double                                              frame_rate          = 30;
    size_t                                              last_frame          = 0;
    sc::time_point                                      last_frame_time     = sc::now();
};
