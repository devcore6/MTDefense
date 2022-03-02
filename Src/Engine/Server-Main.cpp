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

#ifdef __SERVER__
void server_main() {
#else
void server_main(std::future<void> quit) {
#endif
    ENetAddress address { };

    address.host = ip_to_uint32(serverip);
    address.port = (uint16_t)serverport;

    server = enet_host_create(&address, 4, 1, 0, 0);
    if(server == nullptr) {
        std::cerr << "An error occurred while trying to create an ENet server host.\n";
        exit(EXIT_FAILURE);
    }

    ENetEvent event { };

    while(enet_host_service(server, &event, 1000) >= 0) {
        bool client_disconnected = false;
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                std::cout << "connection attempt from " << uint32_to_ip(event.peer->address.host) << ':' << event.peer->address.port << '\n';
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                auto ret = handle_packets({ (const char*)event.packet->data, event.packet->dataLength }, event.peer);
                enet_packet_destroy(event.packet);
                if(!ret) {
                    if(event.peer->data) disconnect_client(((client_t*)(event.peer->data))->cn, ret.err);
                    else enet_peer_reset(event.peer);
                }
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: {
                if(event.peer->data == nullptr)
                    std::cout << uint32_to_ip(event.peer->address.host) << ':' << event.peer->address.port << " disconnected.\n";
                else {
                    client_disconnected = true;
                    client_t* client = (client_t*)event.peer->data;
                    std::cout << client->name << " disconnected\n";
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
#ifndef __SERVER__
        if(quit.wait_for(1_us) != std::future_status::timeout) break;
#endif
    }
    
    enet_host_destroy(server);
}

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

    server_main();
}
#endif
