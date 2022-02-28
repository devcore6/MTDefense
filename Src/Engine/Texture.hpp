#pragma once
#include <vector>
#include <map>
#include <chrono>

#ifndef SDL_MAIN_HANDLED
# define SDL_MAIN_HANDLED
#endif // SDL_MAIN_HANDLED
#include <SDL/SDL.h>

using sc = std::chrono::system_clock;

class texture_t {
private:
    size_t*     ref_count   = nullptr;

public:
    uint32_t    textid      = 0;
    uint32_t    width       = 0;
    uint32_t    height      = 0;

    texture_t()             = default;
    texture_t(std::string path);
    texture_t(SDL_RWops*  data);
    ~texture_t();
};

struct animation_t {
    animation_t()                           = default;
    animation_t(std::string dir);
    std::vector<texture_t>  frames;
    double                  frame_rate      = 30;
    size_t                  last_frame      = 0;
    sc::time_point          last_frame_time = sc::now();
};

extern std::map<std::string, animation_t> map_animations(std::string path);
