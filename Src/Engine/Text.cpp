#include "Engine.hpp"

TTF_Font* regular_font = nullptr;
TTF_Font* large_regular = nullptr;
TTF_Font* monospace_font = nullptr;
TTF_Font* large_mono = nullptr;

void deinit_fonts() {
    if(regular_font)   { TTF_CloseFont(regular_font);   regular_font = nullptr;   }
    if(large_regular)  { TTF_CloseFont(large_regular);  large_regular = nullptr;  }
    if(monospace_font) { TTF_CloseFont(monospace_font); monospace_font = nullptr; }
    if(large_mono)     { TTF_CloseFont(large_mono);     large_mono = nullptr;     }
}

svarpf(main_font, "Data/Fonts/Roboto-Regular.ttf", []() {
    if(regular_font)   { TTF_CloseFont(regular_font);   regular_font = nullptr; }
    if(large_regular)  { TTF_CloseFont(large_regular);  large_regular = nullptr; }
});
svarpf(mono_font, "Data/Fonts/RobotoMono-Regular.ttf", []() {
    if(monospace_font) { TTF_CloseFont(monospace_font); monospace_font = nullptr; }
    if(large_mono)     { TTF_CloseFont(large_mono);     monospace_font = nullptr; }
});
ivarpf(font_size, 1, 16, 256, deinit_fonts);

int render_text(std::string text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool monospace, uint8_t alignment, bool large) {
    if(text == "") return 0;
    TTF_Font* font;
    if(large) {
        if(monospace) {
            if(!large_mono) large_mono = TTF_OpenFont(mono_font.c_str(), (int)(font_size * 3.0));
            if(!large_mono) {
                conerr("Could not load monospace font \""_str + mono_font + "\": " + TTF_GetError());
                return 0;
            }
            font = large_mono;
        } else {
            if(!large_regular) large_regular = TTF_OpenFont(main_font.c_str(), (int)(font_size * 3.0));
            if(!large_regular) {
                conerr("Could not load font \""_str + main_font + "\": " + TTF_GetError());
                return 0;
            }
            font = large_regular;
        }
    } else {
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
                conerr("Could not load font \""_str + main_font + "\": " + TTF_GetError());
                return 0;
            }
            font = regular_font;
        }
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
        glTexCoord2f(0.0f, 0.0f); glVertex2i(x - (alignment == CENTER ? surface->w / 2 : (alignment == RIGHT ? surface->w : 0)), y - (alignment != LEFT ? surface->h / 2 : 0));
        glTexCoord2f(1.0f, 0.0f); glVertex2i(x - (alignment == CENTER ? surface->w / 2 : (alignment == RIGHT ? surface->w : 0)) + surface->w, y - (alignment != LEFT ? surface->h / 2 : 0));
        glTexCoord2f(1.0f, 1.0f); glVertex2i(x - (alignment == CENTER ? surface->w / 2 : (alignment == RIGHT ? surface->w : 0)) + surface->w, y - (alignment != LEFT ? surface->h / 2 : 0) + surface->h);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(x - (alignment == CENTER ? surface->w / 2 : (alignment == RIGHT ? surface->w : 0)), y - (alignment != LEFT ? surface->h / 2 : 0) + surface->h);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture);
    int width = surface->w;
    SDL_FreeSurface(surface);
    return width;
}
