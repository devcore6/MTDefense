#include <sstream>
#include <iostream>
#include <functional>
#include <future>

#include "Server.hpp"
#include "Command.hpp"

ENetHost* server = nullptr;

svarp(serverip, "");
ivarp(serverport, 1, 20069, 65535)
ivarp(maxclients, 1, 2, 4);

#ifdef __SERVER__
void conerr(std::string str) { std::cerr << str << '\n'; }
void conout(std::string str) { std::cout << str << '\n'; }
#else
extern void conerr(std::string str);
extern void conout(std::string str);
#endif

std::vector<client_t> clients;

void disconnect_client(client_iterator cn, int reason) {
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
    if(enet_host_service(server, &event, (uint32_t)(1000 / tickrate)) > 0) {
        bool client_disconnected = false;
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                conout("Connection attempt from "_str + uint32_to_ip(event.peer->address.host) + ':' + std::to_string(event.peer->address.port));
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
#ifdef _DEBUG
                conout("Received packet - size: "_str + std::to_string(event.packet->dataLength) + " - data: " + (const char*)event.packet->data);
#endif
                auto ret = handle_packets({ (const char*)event.packet->data, event.packet->dataLength }, event.peer);
                enet_packet_destroy(event.packet);
                if(!ret) {
                    if(event.peer->data) {
                        client_t* client = (client_t*)event.peer->data;
                        conout(client->name + " kicked from server ("_str + disconnect_reason[ret] + ')');
                        std::cerr << ret.err;
                        disconnect_client(((client_t*)(event.peer->data))->cn, ret);
                    } else {
                        conout(uint32_to_ip(event.peer->address.host) + ':' + std::to_string(event.peer->address.port) + " kicked from server ("_str + disconnect_reason[ret] + ")");
                        enet_peer_reset(event.peer);
                    }
                }
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: {
                if(event.peer->data == nullptr)
                    conout(uint32_to_ip(event.peer->address.host) + ':' + std::to_string(event.peer->address.port) + " disconnected.");
                else {
                    client_disconnected = true;
                    client_t* client = (client_t*)event.peer->data;
                    conout(client->name + " disconnected");
                    do_disconnect(client->cn);
                    clients.erase(client->cn);
                    event.peer->data = nullptr;
                }
                break;
            }
        }
        for(auto& c : clients) if(c.pending_disconnect) if(sc::now() > c.disconnect_at) {
            clients.erase(c.cn);
            client_disconnected = true;
            enet_peer_reset(c.peer);
        }
        if(client_disconnected) for(auto cn = clients.begin(); cn != clients.end(); cn++) { cn->cn = cn; }
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
        serverslice();

        //servertick();

        std::chrono::milliseconds sleep_time = 1000_ms / tickrate - std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - last_tick);
        if(sleep_time.count() > 0)
            if(quit.wait_for(sleep_time) != std::future_status::timeout)
                break;
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
command(startserver, [](std::vector<std::string>& args) {
    quit_server = { };
    server_main(quit_server.get_future());
});
#endif
command(stopserver, [](std::vector<std::string>& args) { quit_server.set_value(); })
