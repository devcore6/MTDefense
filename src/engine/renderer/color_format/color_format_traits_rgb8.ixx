export module renderer.color_format.color_format_traits_rgb8;

import renderer.color_format.color_format;

export template<color_format format>
struct color_format_traits;
 
export template<>
struct color_format_traits<FORMAT_RGB8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 3;

    static constexpr unsigned long r_index = 0;
    static constexpr unsigned long g_index = 1;
    static constexpr unsigned long b_index = 2;
    static constexpr unsigned long a_index = static_cast<unsigned long>(-1);

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = false;
};
