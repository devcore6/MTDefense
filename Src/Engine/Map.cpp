#include "Map.hpp"
#include "Engine.hpp"

#include <fstream>
#include <sstream>

std::vector<map_t> maps;
map_t* current_map = nullptr;

void render_map() {
    current_map->map.render();
}

map_t* init_map(std::string path, uint8_t difficulty) {
    map_t map;
    map.difficulty = difficulty;
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if(!file.is_open()) {
        conerr("Could not open file \""_str + path + "\" to initialize map.");
        return nullptr;
    }

    uint32_t size;

#define _loop(T, name) size = 0_u32; file.read((char*)&size, sizeof(uint32_t)); for(uint32_t i = 0; i < size; i++) { uint32_t s = 0_u32; file.read((char*)&s, sizeof(uint32_t)); T arr; for(uint32_t j = 0; j < s; j++) { vertex_2d vertex; file.read((char*)&vertex.x, sizeof(double)); file.read((char*)&vertex.y, sizeof(double)); arr.vertices.push_back(vertex); } map.name.push_back(arr); }

    _loop(line_strip_t, paths);
    _loop(triangle_strip_t, water);
    _loop(triangle_strip_t, clips);

#undef _loop

    std::ostringstream img_data_stream;
    img_data_stream << file.rdbuf();
    std::string img_data = img_data_stream.str();

    SDL_RWops* mem = SDL_RWFromConstMem(img_data.data(), (int)img_data.capacity());
    if(!mem) {
        file.close();
        conerr("Could not allocate memory for map. Out of memory?");
        return nullptr;
    }

    texture_t image = texture_t(mem);
    if(image.textid == 0) {
        file.close();
        conerr("Could not open map image. Map corrupted/invalid.");
        return nullptr;
    }
    map.map.anim.frames.push_back(image);
    map.map.vertices[0] = { 0, 0 }; 
    map.map.vertices[1] = { 1620, 0 };
    map.map.vertices[2] = { 1620, 1080 };
    map.map.vertices[3] = { 0, 1080 };
    maps.push_back(map);
    
    file.close();
    return &maps[maps.size() - 1];
}
