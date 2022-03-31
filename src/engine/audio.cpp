#include "audio.hpp"
#include "tools.hpp"

#include <filesystem>

sfx_t::sfx_t(const char* path) : data(std::make_shared<audio_t>()) { data->data = Mix_LoadWAV(path); }

void sfx_t::play() const { if(data && data->data) Mix_PlayChannel(-1, data->data, 0); }

sfxs_t::sfxs_t(std::string path) {
    for(const auto& f : std::filesystem::directory_iterator(path))
        if(f.is_regular_file()) {
            std::string str = f.path().string();
            replace_all(str, "\\"_str, "/");
            sfxs.push_back({ str.c_str() });
        }
}

void sfxs_t::play_random() const { if(sfxs.size()) sfxs[rand() % sfxs.size()].play(); }
