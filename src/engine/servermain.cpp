#include <sstream>
#include <iostream>
#include <functional>
#include <future>

#include "server.hpp"
#include "command.hpp"

ENetHost* server = nullptr;

svarp(serverip, "");
ivarp(serverport, 1, 20069, 65535)
ivarp(maxclients, 1, 2, 4);

client_t* get_raw_client(uint32_t cn) {
    for(auto& cl : clients)
        if(cl.cn == cn)
            return &cl;
    return nullptr;
}

#ifdef __SERVER__
void conerr(std::string str) { std::cerr << str << '\n'; }
void conout(std::string str) { std::cout << str << '\n'; }
#else
extern void conerr(std::string str);
extern void conout(std::string str);
#endif

std::vector<client_t> clients;

void disconnect_client(client_t* cn, int reason) {
    if(reason < NUMDISCS && reason > 0) {
        const char* r = disconnect_reason[reason];
        // send reason
    }
    enet_peer_disconnect(cn->peer, 0);
    cn->pending_disconnect = true;
    cn->disconnect_at = sc::now() + 3_s;
}

ivarp(tickrate, 1, 100, 1000);

void serverslice() {
    ENetEvent event { };
    if(enet_host_service(server, &event, 1) > 0) {
        bool client_disconnected = false;
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                conout("Connection attempt from "_str + uint32_to_ip(event.peer->address.host) + ':' + std::to_string(event.peer->address.port));
                event.peer->data = (void*)(size_t)-1;
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                auto ret = handle_packets({ (const char*)event.packet->data, event.packet->dataLength }, event.peer);
                enet_packet_destroy(event.packet);
                if(!ret) {
                    if((size_t)event.peer->data != -1) {
                        client_t* client = get_raw_client((uint32_t)(size_t)event.peer->data);
                        conout(client->name + " kicked from server ("_str + disconnect_reason[ret] + ')');
                        std::cerr << ret.err;
                        disconnect_client(client, ret);
                    } else {
                        conout(uint32_to_ip(event.peer->address.host) + ':' + std::to_string(event.peer->address.port) + " kicked from server ("_str + disconnect_reason[ret] + ")");
                        enet_peer_reset(event.peer);
                    }
                }
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: {
                if((size_t)event.peer->data == -1)
                    conout(uint32_to_ip(event.peer->address.host) + ':' + std::to_string(event.peer->address.port) + " disconnected.");
                else {
                    client_disconnected = true;
                    client_t* client = get_raw_client((uint32_t)(size_t)event.peer->data);
                    conout(client->name + " disconnected");
                    do_disconnect(client->cn);
                    for(auto cn = clients.begin(); cn != clients.end(); cn++)
                        if(cn->cn == client->cn) { clients.erase(cn); return; }
                    event.peer->data = nullptr;
                }
                break;
            }
        }
        for(auto cn = clients.begin(); cn != clients.end(); cn++) if(cn->pending_disconnect) if(sc::now() > cn->disconnect_at) {
            client_disconnected = true;
            enet_peer_reset(cn->peer);
            clients.erase(cn);
        }
    }
}

void server_main(std::future<void> quit) {
    ENetAddress address { };

    if(serverip != "" || enet_address_set_host(&address, serverip.c_str()) < 0)
        address.host = ENET_HOST_ANY;
    address.port = (uint16_t)serverport;

    server = enet_host_create(&address, maxclients, 1, 0, 0);
    if(server == nullptr) {
        conerr("An error occurred while trying to create an ENet server host.\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event { };

    conout("Server started!");

    while(quit.wait_for(0_s) == std::future_status::timeout) {
        sc::time_point last_tick = sc::now();
        while((1000_ms / tickrate - std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - last_tick)).count() > 0)
            serverslice();

        servertick();
    }

    enet_host_destroy(server);
}

std::promise<void> quit_server;

#ifdef __SERVER__

int main(int argc, char* argv[]) {
    if(argc > 1) {
        if(!execfile(argv[1])) {
            std::cerr << "Could not open server main configuration file \"" << argv[1] << "\".\n";
            return EXIT_FAILURE;
        }
    } else {
        execfile("data/server/server.conf");
    }

    if(enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    enet_time_set((uint32_t)time(nullptr));

    server_main(quit_server.get_future());
}
#endif

#ifndef __SERVER__
std::thread server_thread;
bool server_running = false;

command(startserver, [](std::vector<std::string>& args) {
    if(server_running) {
        conerr("server is already running!");
        return;
    }
    server_running = true;
    quit_server = { };
    server_thread = std::thread(server_main, quit_server.get_future());
});

command(stopserver, [](std::vector<std::string>& args) {
    if(!server_running) {
        conerr("server is not running!");
        return;
    }
    server_running = false;
    quit_server.set_value();
})

#else

command(stopserver, [](std::vector<std::string>& args) {
    quit_server.set_value();
})

#endif
