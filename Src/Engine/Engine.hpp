#pragma once
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <gl/GLU.h>
#include <functional>
#include <vector>

#ifdef _WIN32
// For some reason SDL can't find some extensions on windows...
extern PFNGLACTIVETEXTUREPROC   _glActiveTexture;
extern PFNGLBINDTEXTURESPROC    _glBindTextures;
# define glActiveTexture        _glActiveTexture
# define glBindTextures         _glBindTextures
#endif

#include "Tools.hpp"

// Engine/Command.cpp
// Credit: Command system is inspired by the system in Cube 2 @ https://sourceforge.net/p/sauerbraten/
enum {
    VAR_UNDEFINED = -1,
    VAR_INT,
    VAR_DOUBLE,
    VAR_STRING,
    VAR_BOOL
};

template<typename T>
struct var_t {
    T                       min         = 0,
                            max         = 0;
    T*                      ptr         = nullptr;
    std::string             name        = "";
    bool                    persist     = false;
    std::function<void()>   callback    = nullptr;
};

struct svar_t {
    std::string*            ptr         = nullptr;
    std::string             name        = "";
    bool                    persist     = false;
    std::function<void()>   callback    = nullptr;
};

using command_t = std::pair<std::string, std::function<void(std::vector<std::string>)>>;

extern std::vector<std::pair<std::string, std::string>> aliases;

extern std::vector<command_t>           commands;
extern std::vector<var_t<intmax_t>>     ivars;
extern std::vector<var_t<uint32_t>>     hvars;
extern std::vector<var_t<double>>       dvars;
extern std::vector<var_t<bool>>         bvars;
extern std::vector<svar_t>              svars;

inline bool addcommand(std::string name, std::function<void(std::vector<std::string>)> f) { commands.push_back(std::make_pair(name, f)); return true; }

extern void savevars();

extern void parsecmd(std::string cmd);
extern void  execcmd(std::string cmd);
extern void execfile(std::string file, bool silent = true);

#define __var(T, vecname, name, min, value, max, save, f) T name = value; \
bool _initf_ ## name () { vecname.push_back(var_t<T> { min, max, &name, #name, save, f}); return true; } \
bool _initv_ ## name  = _initf_ ## name ();

#define _svar(            name,      value,      save, f) std::string name = value; \
bool _initf_ ## name () { svars.push_back(svar_t { &name, #name, save, f}); return true; } \
bool _initv_ ## name  = _initf_ ## name ();
#define   ivar(name, min, value, max   ) __var(intmax_t, ivars, name, min,   value, max,  false, nullptr)
#define  ivarf(name, min, value, max, f) __var(intmax_t, ivars, name, min,   value, max,  false, f)
#define  ivarp(name, min, value, max   ) __var(intmax_t, ivars, name, min,   value, max,  true,  nullptr)
#define ivarpf(name, min, value, max, f) __var(intmax_t, ivars, name, min,   value, max,  true,  f)

#define   hvar(name, min, value, max   ) __var(uint32_t, hvars, name, min,   value, max,  false, nullptr)
#define  hvarf(name, min, value, max, f) __var(uint32_t, hvars, name, min,   value, max,  false, f)
#define  hvarp(name, min, value, max   ) __var(uint32_t, hvars, name, min,   value, max,  true,  nullptr)
#define hvarpf(name, min, value, max, f) __var(uint32_t, hvars, name, min,   value, max,  true,  f)

#define   dvar(name, min, value, max   ) __var(double,   dvars, name, min,   value, max,  false, nullptr)
#define  dvarf(name, min, value, max, f) __var(double,   dvars, name, min,   value, max,  false, f)
#define  dvarp(name, min, value, max   ) __var(double,   dvars, name, min,   value, max,  true,  nullptr)
#define dvarpf(name, min, value, max, f) __var(double,   dvars, name, min,   value, max,  true,  f)

#define   bvar(name,      value        ) __var(bool,     bvars, name, false, value, true, false, nullptr)
#define  bvarf(name,      value,      f) __var(bool,     bvars, name, false, value, true, false, f)
#define  bvarp(name,      value        ) __var(bool,     bvars, name, false, value, true, true,  nullptr)
#define bvarpf(name,      value,      f) __var(bool,     bvars, name, false, value, true, true,  f)

#define   svar(name,      value        ) _svar(                 name,        value,       false, nullptr)
#define  svarf(name,      value,      f) _svar(                 name,        value,       false, f)
#define  svarp(name,      value        ) _svar(                 name,        value,       true,  nullptr)
#define svarpf(name,      value,      f) _svar(                 name,        value,       true,  f)

#define  command(name, f) static void __command__ ## name (std::vector<std::string> args) { f; }; static bool __command_init__ ## name = addcommand(#name, &__command__ ## name);

#define rcommand(name, f) static void __command__ ## name (std::vector<std::string> args) { \
    std::string ret = ""; \
    f; \
    for(size_t i = 0; i < aliases.size(); i++) if(aliases[i].first == "return") { aliases.erase(aliases.begin() + i); break; } \
    aliases.push_back(std::make_pair(std::string("return"), ret)); \
}; static bool __command_init__ ## name = addcommand(#name, &__command__ ## name);

// Engine/Console.cpp
extern void conout(std::string msg);
extern void conerr(std::string msg);
extern void add_to_chat(std::string msg);
extern void render_console();
extern void render_chat();

// Engine/InitGL.cpp
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

// Engine/Text.cpp

enum {
    LEFT = 0,
    CENTER,
    RIGHT
};

extern intmax_t font_size;
extern int render_text(std::string text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool monospace = false, uint8_t alignemnt = LEFT, bool large = false);
extern void deinit_fonts();
