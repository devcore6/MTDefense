#include "Engine.hpp"
#include "Map.hpp"
#include <iostream>

extern void game_tick();
extern void init_game();
extern void render_map();
extern void render_sidebar();
extern void render_menu();
extern void deinit_game();

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
    game_tick();
}

// temporary until there is a menu
#include "../Game/Difficulty.hpp"
extern void init_match(map_t* map, difficulty diff);

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
    init_match(init_map("Data/Maps/Test.bin"_str, MAP_BEGINNER), medium);

    while(!quit) {
        main_loop_stub();
    }

    savevars();

    deinit_game();
    deinit_fonts();
    deinit_gl();

    TTF_Quit();
    return 0;
}
