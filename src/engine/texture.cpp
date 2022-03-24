#include "texture.hpp"
#ifndef __SERVER__
# include <SDL/SDL_opengl.h>
# include <SDL/SDL_image.h>
#endif

#include "tools.hpp"
#include <filesystem>
#include <map>
#include <algorithm>

#ifndef __SERVER__
# if defined(_WIN32) || defined(_WIN64)
extern PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
# endif
#endif

#define glGenerateMipmap _glGenerateMipmap

#ifndef __SERVER__
struct texture_ref {
    uint32_t textid;
    uint32_t width;
    uint32_t height;
    std::shared_ptr<size_t> ref_count;
};

std::map<std::string, texture_ref> texture_refs { };

extern bool gl_loaded;
extern bool init_gl();
#endif

texture_t::texture_t(std::string path) {
#ifndef __SERVER__
    if(!gl_loaded) init_gl();
    if(texture_refs.contains(path)) {
        auto& ref  = texture_refs[path];
        textid     = ref.textid;
        width      = ref.width;
        height     = ref.height;
        ref_count  = ref.ref_count;
        shared_ref = true;
        (*ref_count)++;
        return;
    }

    SDL_Surface* surface = IMG_Load(path.c_str());
    if(!surface) return;

    int mode = GL_RGB;
    if(surface->format->BytesPerPixel == 4) {
        if(surface->format->Rmask == 0x000000ff)
            mode = GL_RGBA;
        else
            mode = GL_BGRA;
    } else {
        if(surface->format->Rmask == 0x000000ff)
            mode = GL_RGB;
        else
            mode = GL_BGR;
    }

    glGenTextures(1, &textid);
    glBindTexture(GL_TEXTURE_2D, textid);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

# if defined(_WIN32) || defined(_WIN64)
    if(!glGenerateMipmap) glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    if(glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
# else
    glGenerateMipmap(GL_TEXTURE_2D);
# endif

    glBindTexture(GL_TEXTURE_2D, 0);

    width = surface->w;
    height = surface->h;

    SDL_FreeSurface(surface);

    texture_refs.insert({ path, texture_ref { textid, width, height, std::make_shared<size_t>(1_zu) } });
    ref_count = texture_refs[path].ref_count;
#endif
}

#ifndef __SERVER__
texture_t::texture_t(SDL_RWops* data) {
    SDL_Surface* surface = IMG_LoadPNG_RW(data);
    if(!surface) surface = IMG_LoadJPG_RW(data);
    if(!surface) return;

    int mode = GL_RGB;
    if(surface->format->BytesPerPixel == 4) {
        if(surface->format->Rmask == 0x000000ff)
            mode = GL_RGBA;
        else
            mode = GL_BGRA;
    } else {
        if(surface->format->Rmask == 0x000000ff)
            mode = GL_RGB;
        else
            mode = GL_BGR;
    }

    glGenTextures(1, &textid);
    glBindTexture(GL_TEXTURE_2D, textid);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    char* p = (char*)surface->pixels;
    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

# if defined(_WIN32) || defined(_WIN64)
    if(!glGenerateMipmap) glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    if(glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
# else
    glGenerateMipmap(GL_TEXTURE_2D);
# endif

    glBindTexture(GL_TEXTURE_2D, 0);

    width = surface->w;
    height = surface->h;

    SDL_FreeSurface(surface);

    ref_count = std::make_shared<size_t>(1_zu);
}

texture_t::~texture_t() {
    if(ref_count && !--(*ref_count))
        glDeleteTextures(1, &textid);
}

texture_t::texture_t(const texture_t& copy) {
    textid     = copy.textid;
    width      = copy.width;
    height     = copy.height;
    ref_count  = copy.ref_count;
    shared_ref = copy.shared_ref;
    if(ref_count) (*ref_count)++;
}

texture_t::texture_t(texture_t&& move) noexcept {
    std::swap(textid,     move.textid);
    std::swap(width,      move.width);
    std::swap(height,     move.height);
    std::swap(ref_count,  move.ref_count);
    std::swap(shared_ref, move.shared_ref);
}

texture_t& texture_t::operator=(const texture_t& copy) {
    textid     = copy.textid;
    width      = copy.width;
    height     = copy.height;
    ref_count  = copy.ref_count;
    shared_ref = copy.shared_ref;
    if(ref_count) (*ref_count)++;
    return *this;
}

texture_t& texture_t::operator=(texture_t&& move) noexcept {
    std::swap(textid,     move.textid);
    std::swap(width,      move.width);
    std::swap(height,     move.height);
    std::swap(ref_count,  move.ref_count);
    std::swap(shared_ref, move.shared_ref);
    return *this;
}

animation_t::animation_t(std::string folder) {
    for(const auto& f : std::filesystem::directory_iterator(folder))
        if(f.is_regular_file())
            frames.push_back(texture_t(f.path().string()));
}
#endif

std::map<std::string, animation_t> map_animations(std::string path) {
    std::map<std::string, animation_t> ret;
#ifndef __SERVER__
    for(auto& dir : std::filesystem::directory_iterator(path)) {
        if(!dir.is_directory()) continue;
        std::string name = dir.path().filename().string();
        if(name.length() != 5) continue;
        if(!isdigit(name[0]) || name[1] != '-' || !isdigit(name[2]) || name[3] != '-' || !isdigit(name[4])) continue;
        ret.insert({ { name, animation_t(dir.path().string()) } });
    }
#endif
    return ret;
}
