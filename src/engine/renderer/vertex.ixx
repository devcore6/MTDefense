export module renderer.vertex;

import renderer.color_format.color_format;
import renderer.color_format.color_format_traits;

import math.vec;

import <cstdint>;

export template<size_t size, color_format format, class traits = color_format_traits<format>>
struct vertex {
    vec<double, size> pos;
    traits::base_type color[traits::size];
};
