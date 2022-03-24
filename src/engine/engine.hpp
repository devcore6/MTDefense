#pragma once
#ifndef __SERVER__
# define SDL_MAIN_HANDLED
# include <SDL/SDL.h>
# include <SDL/SDL_opengl.h>
# include <SDL/SDL_ttf.h>
# include <SDL/SDL_image.h>
# include <gl/GLU.h>
#endif
#include <functional>
#include <vector>

#ifndef __SERVER__
# ifdef _WIN32
// For some reason SDL can't find some extensions on windows...
extern PFNGLACTIVETEXTUREPROC   _glActiveTexture;
extern PFNGLBINDTEXTURESPROC    _glBindTextures;
#  define glActiveTexture        _glActiveTexture
#  define glBindTextures         _glBindTextures
# endif
#endif

#include "tools.hpp"

// engine/command.cpp
#include "command.hpp"

// engine/console.cpp
extern void conout(std::string msg);
extern void conerr(std::string msg);
#ifndef __SERVER__
extern void add_to_chat(std::string msg);
extern void render_console();
extern void render_chat();

// engine/initgl.cpp
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_GLContext context;

extern intmax_t width;
extern intmax_t height;
extern bool fullscreen;
extern bool fullscreen_windowed;
extern bool texture_compression; // Can help on ridiculously low end GPUs
extern intmax_t maxfps;

extern bool quit;

extern bool init_gl();
extern void deinit_gl();
extern void render_frame();
extern void main_loop();
extern void main_loop_stub();

// engine/text.cpp

enum {
    LEFT = 0,
    CENTER,
    RIGHT
};

extern intmax_t font_size;
extern int render_text(std::string text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool monospace = false, uint8_t alignemnt = LEFT, bool large = false);
extern void deinit_fonts();
#endif
