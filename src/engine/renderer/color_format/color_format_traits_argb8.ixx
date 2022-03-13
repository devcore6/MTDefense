export module renderer.color_format.color_format_traits_argb8;

import renderer.color_format.color_format;

export template<color_format format>
struct color_format_traits;

export template<>
struct color_format_traits<FORMAT_ARGB8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 4;

    static constexpr unsigned long r_index = 1;
    static constexpr unsigned long g_index = 2;
    static constexpr unsigned long b_index = 3;
    static constexpr unsigned long a_index = 0;

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = true;
};
