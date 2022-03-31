#include "network.hpp"
#ifdef __SERVER__
# include "server.hpp"
#endif

#include "tools.hpp"

uint32_t ip_to_uint32(std::string ip) {
    if(ip == "" || ip == "0") return ENET_HOST_ANY;

    uint32_t val = 0;
    std::istringstream ss(ip);
    std::string buf;

    for(uint8_t i = 0; i < 4; i++)
        if(std::getline(ss, buf, '.'))
            val |= (std::stoi(buf) & 0xFF) << (i * 8);
        else return ENET_HOST_ANY;
    
    return val;
}

std::string uint32_to_ip(uint32_t ip) {
    std::string val = "";

    for(uint8_t i = 0; i < 4; i++) {
        if(i) val += ".";
        val += std::to_string((ip >> (i * 8)) & 0xFF);
    }

    return val;
}

extern ENetHost* server;
#ifndef __SERVER__
extern bool server_running;
#endif

void send_packet(ENetPeer* peer, uint8_t chan, bool reliable, packetstream& data) {
    size_t size = data.size();
    if(!peer) {
#ifndef __SERVER__
        if(!server_running) return;
#endif
        ENetPacket* packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        enet_host_broadcast(server, chan, packet);
        return;
    }
    ENetPacket* packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(peer, chan, packet);
}
