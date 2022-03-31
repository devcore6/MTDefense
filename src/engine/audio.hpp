#pragma once
#include <SDL/SDL_mixer.h>
#include <vector>

#ifndef __SERVER__
#include <string>
#include <memory>

struct audio_t {
    Mix_Chunk* data = nullptr;
    ~audio_t() { if(data) Mix_FreeChunk(data); }
};

class sfx_t {
private:
    std::shared_ptr<audio_t> data;

public:
    sfx_t() = default;
    sfx_t(const char* path);
    
    void play() const;
};
#endif

struct sfxs_t {
    sfxs_t() = default;

#ifndef __SERVER__
    std::vector<sfx_t> sfxs;
    sfxs_t(std::string path);
    void play_random() const;
#else
    sfxs_t(const char* path) { }
#endif
};
