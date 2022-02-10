#include "Texture.hpp"
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#if defined(_WIN32) || defined(_WIN64)
PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
#endif

#define glGenerateMipmap _glGenerateMipmap

texture_t::texture_t(std::string path) {
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

#if defined(_WIN32) || defined(_WIN64)
    if(!glGenerateMipmap) glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    if(glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
#else
    glGenerateMipmap(GL_TEXTURE_2D);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    width = surface->w;
    height = surface->h;

    SDL_FreeSurface(surface);
}

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

    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

#if defined(_WIN32) || defined(_WIN64)
    if(!glGenerateMipmap) glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    if(glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
#else
    glGenerateMipmap(GL_TEXTURE_2D);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    width = surface->w;
    height = surface->h;

    SDL_FreeSurface(surface);
}
