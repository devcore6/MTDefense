#pragma once
#include <cxxgui/cxxgui.hpp>

extern intmax_t width, height;

class gui : public cxxgui::view {
public:
    view* body = nullptr;
    void render_wrapper(bool clicking, bool send_click, int lastx, int lasty) {
        if(!body) return;
        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        // todo: probably bugs out when full screen on non native res
        float rel_x = (float)x / width * 1920.0f, rel_y = (float)y / height * 1080.0f;

        // I am about 87% certain this will crash or otherwise break something as body is never a gui object
        // Update: It doesn't. The fuck? I mean I guess I get why, but it's weird af
        ((gui*)body)->do_render(rel_x, rel_y, clicking, send_click, lastx, lasty);
    }
}; 
