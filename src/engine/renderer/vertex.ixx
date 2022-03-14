export module renderer.vertex;

import renderer.color_format.color_format;
import renderer.color_format.color_format_traits;

import math.vec;

import <cstdint>;

export template<size_t size, color_format format, color_format_traits_t traits = color_format_traits[format]>
    requires(size == 2 || size == 3)
struct vertex {
    vec<double, size> pos;
    unsigned char color[traits.size];
};
