#include "Engine.hpp"
#include "Map.hpp"
#include <iostream>

extern void init_game();
extern void render_map();
extern void render_sidebar();
extern void render_menu();

void render_frame() {
    if(current_map) {
        render_map();
        render_sidebar();
    } else {
        render_menu();
    }
    render_console();
    render_chat();
}

void main_loop() {

}
#include <iomanip>
int main(int argc, char* argv[]) {
    TTF_Init();

    execfile("Data/Config/Config.conf");
    execfile("Data/Config/Autoexec.conf");

    if(!init_gl()) {
        std::cerr << "Could not initialize SDL2/openGL.\n";
        return EXIT_FAILURE;
    }

    init_game();
    current_map = init_map("Data/Maps/Test.bin"_str, MAP_BEGINNER);

    while(!quit) {
        main_loop_stub();
    }

    savevars();

    deinit_fonts();
    deinit_gl();

    TTF_Quit();
    return 0;
}
