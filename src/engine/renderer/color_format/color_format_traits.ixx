export module renderer.color_format.color_format_traits;

import renderer.color_format.color_format;

export struct color_format_traits_t {
    using base_type = void;
    unsigned long size;

    unsigned long r_index;
    unsigned long g_index;
    unsigned long b_index;
    unsigned long a_index;

    bool has_red;
    bool has_green;
    bool has_blue;
    bool has_alpha;
} color_format_traits[FORMAT_MAX] = {
       /* FORMAT_RGB:  */ {
        /* size:       */ 3,
        /* r_index:    */ 0,
        /* g_index:    */ 1,
        /* b_index:    */ 2,
        /* a_index:    */ static_cast<unsigned long>(-1),
        /* has_red  :  */ true,
        /* has_green:  */ true,
        /* has_blue :  */ true,
        /* has_alpha:  */ false
    }, /* FORMAT_RGBA: */ {
        /* size:       */ 4,
        /* r_index:    */ 0,
        /* g_index:    */ 1,
        /* b_index:    */ 2,
        /* a_index:    */ 3,
        /* has_red  :  */ true,
        /* has_green:  */ true,
        /* has_blue :  */ true,
        /* has_alpha:  */ true
    }, /* FORMAT_ARGB: */ {
        /* size:       */ 4,
        /* r_index:    */ 1,
        /* g_index:    */ 2,
        /* b_index:    */ 3,
        /* a_index:    */ 0,
        /* has_red  :  */ true,
        /* has_green:  */ true,
        /* has_blue :  */ true,
        /* has_alpha:  */ true
    }, /* FORMAT_BGR:  */ {
        /* size:       */ 3,
        /* r_index:    */ 2,
        /* g_index:    */ 1,
        /* b_index:    */ 0,
        /* a_index:    */ static_cast<unsigned long>(-1),
        /* has_red  :  */ true,
        /* has_green:  */ true,
        /* has_blue :  */ true,
        /* has_alpha:  */ false
    }, /* FORMAT_BGRA: */ {
        /* size:       */ 4,
        /* r_index:    */ 2,
        /* g_index:    */ 1,
        /* b_index:    */ 0,
        /* a_index:    */ 3,
        /* has_red  :  */ true,
        /* has_green:  */ true,
        /* has_blue :  */ true,
        /* has_alpha:  */ true
    }, /* FORMAT_ABGR: */ {
        /* size:       */ 4,
        /* r_index:    */ 3,
        /* g_index:    */ 2,
        /* b_index:    */ 1,
        /* a_index:    */ 0,
        /* has_red  :  */ true,
        /* has_green:  */ true,
        /* has_blue :  */ true,
        /* has_alpha:  */ true
    }
};
