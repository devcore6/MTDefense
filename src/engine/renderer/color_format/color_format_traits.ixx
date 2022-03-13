export module renderer.color_format.color_format_traits;

import renderer.color_format.color_format;

export template<color_format format>
struct color_format_traits {
    using base_type = void;
    static constexpr unsigned long size;

    static constexpr unsigned long r_index;
    static constexpr unsigned long g_index;
    static constexpr unsigned long b_index;
    static constexpr unsigned long a_index;

    static constexpr bool has_red;
    static constexpr bool has_green;
    static constexpr bool has_blue;
    static constexpr bool has_alpha;
};
