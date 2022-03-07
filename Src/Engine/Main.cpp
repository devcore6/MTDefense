#include "Engine.hpp"
#include "Server.hpp"
#include "Map.hpp"
#include <iostream>
#include <enet/enet.h>
#include "../Game/Game.hpp"
#include <ctime>

ENetHost* client = nullptr;
ENetPeer* peer = nullptr;

extern std::string name;

extern void do_connect();

command(connect, [](std::vector<std::string>& args) {
    if(args.size() == 0) { conerr("Usage: connect <ip address> [port]"); return; }
    uint16_t port = 20069;
    if(args.size() > 2) {
        port = std::stoi(args[2]);
        if(port == 0) { conerr("Not a valid port. Usage: connect <ip address> [port]"); return; }
    }
    ENetEvent event { };
    ENetAddress address { };
    enet_address_set_host(&address, args[1].c_str());
    address.port = port;
    conout("Attempting to connect to "_str + args[1] + ":" + std::to_string(port));
    for(int i = 0; i < 5; i++) {
        peer = enet_host_connect(client, &address, 1, 0);
        if(!peer) continue;
        if(enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
            conout("Connected!");
            do_connect();
            return;
        }
        enet_peer_reset(peer);
        if(i != 4) conout("...");
    }
    conerr("Connection failed.");
});
command(quit, [](std::vector<std::string>& args) { quit = true; })

//extern void init_game();

int main(int argc, char* argv[]) {
    if(enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    enet_time_set((uint32_t)time(nullptr));

    client = enet_host_create(nullptr, 1, 1, 0, 0);
    if(client == nullptr) {
        std::cerr << "An error occurred while trying to create an ENet client host.\n";
        return EXIT_FAILURE;
    }

    TTF_Init();

    if(!init_gl()) {
        std::cerr << "Could not initialize SDL2/openGL.\n";
        return EXIT_FAILURE;
    }

    execfile("Data/Config/Config.conf");
    execfile("Data/Config/Autoexec.conf");

    // this is just for testing
    /*if(peer) {
        for(;;) {
            ENetEvent e { };
            if(enet_host_service(client, &e, 5000) > 0) {
                switch(e.type) {
                    case ENET_EVENT_TYPE_RECEIVE: {
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: exit(1);
                    default: break;
                }
            }
            packetstream p;
            p << N_PING << 0_u32;
            send_packet(peer, 0, true, p);
        }
    }*/

    //init_game();
    //init_match(init_map("Data/Maps/Test.bin"_str, MAP_BEGINNER), medium);

    while(!quit) {
        main_loop_stub();
    }

    savevars();

    //deinit_game();

    if(peer) enet_peer_reset(peer);
    enet_host_destroy(client);
    deinit_fonts();
    deinit_gl();

    TTF_Quit();
    return 0;
}
