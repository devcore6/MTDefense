#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

import renderer.command_buffer;
import renderer.window;
import renderer.opengl;

import math.vec2;

int main(int argc, const char* argv[]) {
    try {
        auto w = std::make_shared<window<opengl_renderer>>("Test", 640, 480, false);
        while(!w->should_quit()) {
            w->poll();
            command_buffer<2, 3> buffer { RENDER_TRIANGLES, {
                { vec2<double>{   0.0,   0.0 }, { 255, 0, 0 } },
                { vec2<double>{ 320.0, 480.0 }, { 0, 255, 0 } },
                { vec2<double>{ 640.0,   0.0 }, { 0, 0, 255 } }
            } };
            auto& r = w->get_renderer();
            r->prepare();
            r->clear();
            r->send_command(buffer);
            w->update();
            std::this_thread::sleep_for(std::chrono::milliseconds(6));
        }
        return 0;
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
        return 1;
    }
}
