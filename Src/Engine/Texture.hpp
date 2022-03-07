#pragma once
#include <vector>
#include <map>
#include <chrono>
#include <string>

#ifndef __SERVER__
# ifndef SDL_MAIN_HANDLED
#  define SDL_MAIN_HANDLED
# endif // SDL_MAIN_HANDLED
# include <SDL/SDL.h>
#endif

using sc = std::chrono::system_clock;

class texture_t {
#ifndef __SERVER__
private:
    size_t*     ref_count   = nullptr;
    bool        shared_ref  = false;
#endif

public:
#ifndef __SERVER__
    uint32_t    textid      = 0;
    uint32_t    width       = 0;
    uint32_t    height      = 0;
#endif

    texture_t()             = default;
    texture_t(std::string path);
#ifndef __SERVER__
    texture_t(SDL_RWops*  data);
    texture_t(const texture_t&  copy);
    texture_t(      texture_t&& move);

    texture_t& operator=(const texture_t& copy);
    texture_t& operator=(texture_t&& move);
    ~texture_t();
#endif
};

struct animation_t {
#ifndef __SERVER__
    animation_t()                           = default;
    animation_t(std::string dir);
    std::vector<texture_t>  frames;
    double                  frame_rate      = 30;
    size_t                  last_frame      = 0;
    sc::time_point          last_frame_time = sc::now();
#endif
};

extern std::map<std::string, animation_t> map_animations(std::string path);

