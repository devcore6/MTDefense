#pragma once
#include "gl.hpp"

// todo: Add an elevation system
// todo: Add clearable areas

enum {
    MAP_BEGINNER = 0,
    MAP_INTERMEDIATE,
    MAP_ADVANCED,
    MAP_EXPERT,
    MAP_IMPOSSIBLE
};

struct map_t {
    rect_t                          map;

    std::vector<triangle_strip_t>   water;
    std::vector<triangle_strip_t>   clips;
    
    std::vector<line_strip_t>       paths;

    uint8_t                         difficulty = (uint8_t)MAP_BEGINNER;

    std::string                     data;
};

extern std::vector<map_t>           maps;
extern map_t*                       current_map;
extern map_t*                       init_map(std::string path);
extern map_t*                       init_map(std::stringstream& in);
