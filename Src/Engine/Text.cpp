#include "Engine.hpp"

TTF_Font* regular_font = nullptr;
TTF_Font* monospace_font = nullptr;

svarpf(main_font, "Data/Fonts/Roboto-Regular.ttf",     []() { if(regular_font)   { TTF_CloseFont(regular_font);   regular_font = nullptr;   }});
svarpf(mono_font, "Data/Fonts/RobotoMono-Regular.ttf", []() { if(monospace_font) { TTF_CloseFont(monospace_font); monospace_font = nullptr; }});
ivarpf(font_size, 1, 16, 256, []() {
    if(regular_font)   { TTF_CloseFont(regular_font);   regular_font = nullptr; }
    if(monospace_font) { TTF_CloseFont(monospace_font); monospace_font = nullptr; }
});

int render_text(std::string text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool monospace, bool centered) {
    if(text == "") return 0;
    TTF_Font* font;
    if(monospace) {
        if(!monospace_font) monospace_font = TTF_OpenFont(mono_font.c_str(), (int)(font_size * 1.5));
        if(!monospace_font) {
            conerr("Could not load monospace font \""_str + mono_font + "\": " + TTF_GetError());
            return 0;
        }
        font = monospace_font;
    } else {
        if(!regular_font) regular_font = TTF_OpenFont(main_font.c_str(), (int)(font_size * 1.5));
        if(!regular_font) {
            conerr("Could not load monospace font \""_str + main_font + "\": " + TTF_GetError());
            return 0;
        }
        font = regular_font;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), { r, g, b, a });
    if(!surface) {
        conerr("Could not render text: "_str + TTF_GetError());
        return 0;
    }

    uint8_t colors = surface->format->BytesPerPixel;
    GLenum format;
    if(colors == 4) { if(surface->format->Rmask == 0x000000ff) format = GL_RGBA; else format = GL_BGRA; }
    else            { if(surface->format->Rmask == 0x000000ff) format = GL_RGB ; else format = GL_BGR ; }

    GLuint texture;
    glGenTextures(1, &texture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, colors, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(x + surface->w, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2i(x + surface->w, y + surface->h);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(x, y + surface->h);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture);
    int width = surface->w;
    SDL_FreeSurface(surface);
    return width;
}

void deinit_fonts() {
    if(regular_font)   { TTF_CloseFont(regular_font);   regular_font = nullptr; }
    if(monospace_font) { TTF_CloseFont(monospace_font); monospace_font = nullptr; }
}
