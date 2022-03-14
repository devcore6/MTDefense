export module renderer.color_format_traits;

import renderer.color_format;

export template<color_format format>
struct color_format_traits {
    using base_type = void;
    static constexpr unsigned long size = 0;
    static constexpr float max_value = 0.0f;

    static constexpr unsigned long r_index = 0;
    static constexpr unsigned long g_index = 0;
    static constexpr unsigned long b_index = 0;
    static constexpr unsigned long a_index = 0;

    static constexpr bool has_red   = false;
    static constexpr bool has_green = false;
    static constexpr bool has_blue  = false;
    static constexpr bool has_alpha = false;
};

export template<>
struct color_format_traits<FORMAT_RGB8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 3;
    static constexpr float max_value = 255.0f;

    static constexpr unsigned long r_index = 0;
    static constexpr unsigned long g_index = 1;
    static constexpr unsigned long b_index = 2;
    static constexpr unsigned long a_index = static_cast<unsigned long>(-1);

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = false;
};

export template<>
struct color_format_traits<FORMAT_RGBA8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 3;
    static constexpr float max_value = 255.0f;

    static constexpr unsigned long r_index = 0;
    static constexpr unsigned long g_index = 1;
    static constexpr unsigned long b_index = 2;
    static constexpr unsigned long a_index = 3;

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = true;
};

export template<>
struct color_format_traits<FORMAT_ARGB8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 4;
    static constexpr float max_value = 255.0f;

    static constexpr unsigned long r_index = 1;
    static constexpr unsigned long g_index = 2;
    static constexpr unsigned long b_index = 3;
    static constexpr unsigned long a_index = 0;

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = true;
};

export template<>
struct color_format_traits<FORMAT_BGR8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 3;
    static constexpr float max_value = 255.0f;

    static constexpr unsigned long r_index = 2;
    static constexpr unsigned long g_index = 1;
    static constexpr unsigned long b_index = 0;
    static constexpr unsigned long a_index = static_cast<unsigned long>(-1);

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = false;
};

export template<>
struct color_format_traits<FORMAT_BGRA8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 3;
    static constexpr float max_value = 255.0f;

    static constexpr unsigned long r_index = 2;
    static constexpr unsigned long g_index = 1;
    static constexpr unsigned long b_index = 0;
    static constexpr unsigned long a_index = 3;

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = true;
};

export template<>
struct color_format_traits<FORMAT_ABGR8> {
    using base_type = unsigned char;
    static constexpr unsigned long size = 3;
    static constexpr float max_value = 255.0f;

    static constexpr unsigned long r_index = 3;
    static constexpr unsigned long g_index = 2;
    static constexpr unsigned long b_index = 1;
    static constexpr unsigned long a_index = 0;

    static constexpr bool has_red   = true;
    static constexpr bool has_green = true;
    static constexpr bool has_blue  = true;
    static constexpr bool has_alpha = true;
};
