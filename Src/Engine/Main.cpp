#include "Engine.hpp"
#include "Server.hpp"
#include "Map.hpp"
#include "Network.hpp"
#include <iostream>
#include <enet/enet.h>

ENetHost* client = nullptr;

command(connect, [](std::vector<std::string> args) {
    if(args.size() == 0) { conerr("Usage: connect <ip address> [port]"); return; }
    uint32_t addr = ip_to_uint32(args[0]);
    if(addr == ENET_HOST_ANY) { conerr("Not a valid IP address. Usage: connect <ip address> [port]"); return; }
    uint16_t port = 20069;
    if(args.size() >= 2) {
        port = std::stoi(args[1]);
        if(port == 0) { conerr("Not a valid port. Usage: connect <ip address> [port]"); return; }
    }
    // todo: finish this function
});

extern void game_tick();
extern void init_game();
extern void render_map();
extern void render_sidebar();
extern void render_menu();
extern void deinit_game();

void render_frame() {
    if(current_map) {
        render_map();
    } else {
    //    render_menu();
    }
    render_console();
    render_chat();
}

void main_loop() {
    //game_tick();
    //if(current_map) render_sidebar();
}

int main(int argc, char* argv[]) {
    TTF_Init();

    execfile("Data/Config/Config.conf");
    execfile("Data/Config/Autoexec.conf");

    if(!init_gl()) {
        std::cerr << "Could not initialize SDL2/openGL.\n";
        return EXIT_FAILURE;
    }

    client = enet_host_create(nullptr, 1, 1, 0, 0);
    if(client == nullptr) {
        std::cerr << "An error occurred while trying to create an ENet client host.\n";
        return EXIT_FAILURE;
    }

    init_game();
    //init_match(init_map("Data/Maps/Test.bin"_str, MAP_BEGINNER), medium);

    while(!quit) {
        main_loop_stub();
    }

    savevars();

    //deinit_game();

    enet_host_destroy(client);
    deinit_fonts();
    deinit_gl();

    TTF_Quit();
    return 0;
}
