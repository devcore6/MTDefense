#pragma once
#include "Texture.hpp"
#include "Tools.hpp"

using vertex_2d = vec2<double>;

struct base_triangle_t {
    vertex_2d       vertices[3] = { };

    static double   get_area(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3, double range = 0.0);
    double          get_area();

    static bool     contains(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3, vertex_2d& vertex, double range);
    bool            contains(vertex_2d vertex);

    vertex_2d& operator[](uint8_t id);
};

struct triangle_strip_t {                                                       // Effectively a polygon, but represented by a triangle strip
                                                                                // Used for clipping areas
    std::vector<vertex_2d> vertices;
    vertex_2d&      operator[](size_t id);
    bool            contains(vertex_2d vertex, double range);
};

struct base_rect_t {
    vertex_2d       vertices[4] = { };                                          // top-left, top-right, bottom-right, bottom-left

    double          get_area();

    bool            contains(vertex_2d vertex);

    vertex_2d&      operator[](uint8_t id);
};

struct rect_t : public base_rect_t {
    animation_t     anim;
#ifndef __SERVER__
    void            render();
    void            render(vertex_2d offset, double& rot);
#endif
};

struct line_strip_t {
    std::vector<vertex_2d> vertices;
    vertex_2d& operator[](size_t id);
    vertex_2d  get_position_at(double d);
#ifndef __SERVER__
    void render();
#endif

    double distance(vertex_2d v);
};

struct line_t {
    vertex_2d v1 = { 0.0, 0.0 };
    vertex_2d v2 = { 0.0, 0.0 };

    double distance(vertex_2d v);
};

#ifndef __SERVER__
extern uint32_t render_circle(double radius, std::string fill_color = "none", double stroke = 0, std::string stroke_color = "none");
#endif
