module;

// define APIENTRY to avoid SDL_opengl.h including <Windows.h>, which, of course
// being a Windows header, generates 259 warnings.

#if defined(_WIN32) && !defined(APIENTRY)
# define APIENTRY __stdcall
#endif

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

export module renderer.opengl;

import renderer.color_format;
import renderer.color_format_traits;
import renderer.command_buffer;
import renderer.viewport;

import <stdexcept>;
import <cstdint>;

export class opengl_renderer {
public:
    static constexpr int window_flags = SDL_WINDOW_OPENGL;
    void init(SDL_Window* window) {
        context = SDL_GL_CreateContext(window);
        if(!context) throw std::runtime_error(std::string("Could not create OpenGL context: ") + SDL_GetError());
        glShadeModel(GL_SMOOTH);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    opengl_renderer() {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    }

    ~opengl_renderer() {
        if(context) SDL_GL_DeleteContext(context);
    }

    void clear() noexcept { glClear(GL_COLOR_BUFFER_BIT); }

    void set_clear_color(float r, float g, float b, float a) {
        if(r < 0.0 || r > 1.0
        || g < 0.0 || g > 1.0
        || b < 0.0 || b > 1.0
        || a < 0.0 || a > 1.0) [[unlikely]] throw std::out_of_range("Clear color values must be between 0.0 and 1.0.");
        glClearColor(r, g, b, a);
    }

    void set_viewport(viewport v) {
        if(v.w < 0.0 || v.h < 0.0) [[unlikely]] throw std::out_of_range("Viewport width and height must be positive.");
        vp = v;
    }

    template<size_t vertex_size,
             size_t vertex_count,
             color_format format,
             class Traits = color_format_traits<format>>
        requires(vertex_size == 2 || vertex_size == 3)
    void send_command(command_buffer<vertex_size,
                                     vertex_count,
                                     format> buffer) {
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

        for(auto& vertex : buffer.vertices) {
            float r = Traits::has_red   ? (float)vertex.color[Traits::r_index] / Traits::max_value : 0.0f;
            float g = Traits::has_green ? (float)vertex.color[Traits::g_index] / Traits::max_value : 0.0f;
            float b = Traits::has_blue  ? (float)vertex.color[Traits::b_index] / Traits::max_value : 0.0f;
            float a = Traits::has_alpha ? (float)vertex.color[Traits::a_index] / Traits::max_value : 1.0f;
            glColor4f(r, g, b, a);
            if(vertex_size == 2) glVertex2d(vertex.pos[0], vertex.pos[1]);
            else glVertex3d(vertex.pos[0],  vertex.pos[1], vertex.pos[2]);
        }

        glEnd();
    }

    void prepare() {
        glLoadIdentity();
        glViewport((int)vp.x, (int)vp.y, (int)vp.w, (int)vp.h);
        glOrtho(vp.x, vp.x + vp.w, vp.y, vp.y + vp.h, 0.0, 1.0);
    }

    void update(SDL_Window* window) {
        glFlush();
        SDL_GL_SwapWindow(window);
    }

private:
    viewport vp;
    SDL_GLContext context = nullptr;
};
