#pragma once
#include "Texture.hpp"

struct vertex_2d {
    double          x           = 0;
    double          y           = 0;

    vertex_2d       operator+(vertex_2d& rhs);
    vertex_2d       operator-(vertex_2d& rhs);
    vertex_2d       operator*(double d);

    vertex_2d&      operator+=(vertex_2d& rhs);
    vertex_2d&      operator-=(vertex_2d& rhs);
    vertex_2d&      operator*=(double d);

    double&         operator[](uint8_t id);                                     // 0 = x, 1 = y, 2 ... 255 = y
};

struct base_triangle_t {
    vertex_2d       vertices[3] = { };

    static double   get_area(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3);
    double          get_area();

    static bool     contains(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3, vertex_2d& vertex);
    bool            contains(vertex_2d& vertex);

    vertex_2d& operator[](uint8_t id);
};

struct triangle_strip_t {                                                       // Effectively a polygon, but represented by a triangle strip
                                                                                // Used for clipping areas
    std::vector<vertex_2d> vertices;
    vertex_2d&      operator[](size_t id);
    bool            contains(vertex_2d& vertex);
};

struct base_rect_t {
    vertex_2d       vertices[4] = { };                                          // top-left, top-right, bottom-right, bottom-left

    double          get_area();

    bool            contains(vertex_2d& vertex);

    vertex_2d&      operator[](uint8_t id);
};

struct rect_t : public base_rect_t {
    animation_t     anim;

    void            render();
    void            render(vertex_2d& offset, double& rot);
};

struct line_strip_t {
    std::vector<vertex_2d> vertices;
    vertex_2d& operator[](size_t id);
    vertex_2d  get_position_at(double d);
};
