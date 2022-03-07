#include "Map.hpp"
#include "Engine.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

std::vector<map_t> maps;
map_t* current_map = nullptr;

#ifndef __SERVER__
void render_map() {
    current_map->map.render();
}
#endif

map_t* init_map(std::string path) {
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if(!file.is_open()) {
        conerr("Could not open file \""_str + path + "\" to initialize map.");
        return nullptr;
    }

    std::stringstream in;
    in << file.rdbuf();
    file.close();

    return init_map(in);
}

map_t* init_map(std::stringstream& in) {
    map_t map;

    in.read((char*)(&map.difficulty), 1);
    uint32_t size;

#define _loop(T, name) size = 0_u32; in.read((char*)&size, sizeof(uint32_t)); for(uint32_t i = 0; i < size; i++) { uint32_t s = 0_u32; in.read((char*)&s, sizeof(uint32_t)); T arr; for(uint32_t j = 0; j < s; j++) { vertex_2d vertex; in.read((char*)&vertex.x, sizeof(double)); in.read((char*)&vertex.y, sizeof(double)); arr.vertices.push_back(vertex); } map.name.push_back(arr); }

    _loop(line_strip_t, paths);
    _loop(triangle_strip_t, water);
    _loop(triangle_strip_t, clips);

#undef _loop

    if(map.paths.size() == 0) {
        conerr("Map is invalid or damaged.");
        return nullptr;
    }


    std::ostringstream img_data_stream;
#ifndef __SERVER__
    img_data_stream << in.rdbuf();
    std::string data = img_data_stream.str();

    SDL_RWops* mem = SDL_RWFromConstMem(data.data(), (int)data.size());
    if(!mem) {
        conerr("Could not allocate memory for map. Out of memory?");
        return nullptr;
    }

    texture_t image = texture_t(mem);
    if(image.textid == 0) {
        conerr("Could not open map image. Map corrupted/invalid.");
        return nullptr;
    }
    map.map.anim.frames.push_back(image);
    map.map.vertices[0] = { 0, 0 };
    map.map.vertices[1] = { 1620, 0 };
    map.map.vertices[2] = { 1620, 1080 };
    map.map.vertices[3] = { 0, 1080 };
#endif

    auto buf = in.rdbuf();
    buf->pubseekpos(0);
    img_data_stream.clear();
    img_data_stream << buf;
    map.data = img_data_stream.str();
    maps.push_back(map);

    return &maps[maps.size() - 1];
}
