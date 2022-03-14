module;

// define APIENTRY to avoid SDL_opengl.h including <Windows.h>, which, of course
// being a Windows header, generates 259 warnings.

#if defined(_WIN32) && !defined(APIENTRY)
# define APIENTRY __stdcall
#endif

#include <SDL/SDL_opengl.h>

export module renderer.opengl.renderer;

import renderer.base.renderer;
import renderer.base.command_buffer;

import <stdexcept>;

export class opengl_renderer: public renderer {
public:
    void clear() noexcept { glClear(GL_COLOR_BUFFER_BIT); }

    void set_clear_color(float r, float g, float b, float a) {
        if(r < 0.0 || r > 1.0
        || g < 0.0 || g > 1.0
        || b < 0.0 || b > 1.0
        || a < 0.0 || a > 1.0)     [[unlikely]] throw std::out_of_range("Clear color values must be between 0.0 and 1.0.");
        glClearColor(r, g, b, a);
    }

    void set_viewport(viewport v) {
        if(v.w < 0.0 || v.h < 0.0) [[unlikely]] throw std::out_of_range("Viewport width and height must be positive.");
        vp = v;
    }

    void send_command(command_buffer_wrapper buffer) {
        switch(buffer.mode) {
            case RENDER_POINTS:         glBegin(GL_POINTS);         break;
            case RENDER_LINES:          glBegin(GL_LINES);          break;
            case RENDER_LINE_LOOP:      glBegin(GL_LINE_LOOP);      break;
            case RENDER_LINE_STRIP:     glBegin(GL_LINE_STRIP);     break;
            case RENDER_TRIANGLES:      glBegin(GL_TRIANGLES);      break;
            case RENDER_TRIANGLE_STRIP: glBegin(GL_TRIANGLE_STRIP); break;
            case RENDER_TRIANGLE_FAN:   glBegin(GL_TRIANGLE_FAN);   break;
            case RENDER_QUADS:          glBegin(GL_QUADS);          break;
            case RENDER_QUAD_STRIP:     glBegin(GL_QUAD_STRIP);     break;
            case RENDER_POLYGON:        glBegin(GL_POLYGON);        break;
        }

        auto& traits = color_format_traits[buffer.format];
      
        // todo: textures
        for(size_t i = 0; i < buffer.v_count; i++) {
            float r = traits.has_red   ? (float)buffer.colors[i * traits.size + traits.r_index] / 255.0f : 0.0f;
            float g = traits.has_green ? (float)buffer.colors[i * traits.size + traits.g_index] / 255.0f : 0.0f;
            float b = traits.has_blue  ? (float)buffer.colors[i * traits.size + traits.b_index] / 255.0f : 0.0f;
            float a = traits.has_alpha ? (float)buffer.colors[i * traits.size + traits.a_index] / 255.0f : 1.0f;
            glColor4f(r, g, b, a);
            if(buffer.v_size == 2) glVertex2d(buffer.positions[i * 2], buffer.positions[i * 2 + 1]);
            else glVertex3d(buffer.positions[i * 3], buffer.positions[i * 3 + 1], buffer.positions[i * 3 + 2]);
        }

        glEnd();
    }

private:
    viewport vp;
};
