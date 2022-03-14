export module renderer.command_buffer;

import renderer.color_format;

import renderer.vertex;

import <cstdint>;

export enum command_mode: unsigned char {
    RENDER_POINTS = 0,
    RENDER_LINES,
    RENDER_LINE_LOOP,
    RENDER_LINE_STRIP,
    RENDER_TRIANGLES,
    RENDER_TRIANGLE_STRIP,
    RENDER_TRIANGLE_FAN,
    RENDER_QUADS,
    RENDER_QUAD_STRIP,
    RENDER_POLYGON
};

export template<size_t vertex_size,
                size_t vertex_count,
                color_format format = FORMAT_RGB8>
    requires(vertex_size == 2 || vertex_size == 3)
struct command_buffer {
    // todo: textures

    command_mode mode;
    vertex<vertex_size, format> vertices[vertex_count];
};
