export module renderer.base.renderer;

import renderer.color_format.color_format;

import renderer.base.command_buffer;
import renderer.base.viewport;

import renderer.vertex;

import tools.types;

import <cstdint>;

export enum renderer_type : unsigned char {
    RENDERER_OPENGL = 0
};

export template<renderer_type T>
class renderer {
public:
    void clear() noexcept { }
    void set_clear_color(float r, float g, float b, float a) { }
    void set_viewport(viewport) { }

    template<size_t vertex_size,
             size_t vertex_count,
             color_format format>
    void send_command(command_buffer<vertex_size, vertex_count, format>) { }
};

