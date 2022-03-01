#include "Network.hpp"
#ifdef __SERVER__
# include "Server.hpp"
#endif

uint32_t ip_to_uint32(std::string ip) {
    if(ip == "" || ip == "0") return ENET_HOST_ANY;

    uint32_t val = 0;
    std::istringstream ss(ip);
    std::string buf;

    for(uint8_t i = 0; i < 4; i++)
        if(std::getline(ss, buf, '.'))
            val |= (std::stoi(buf) & 0xFF) << ((3 - i) * 8);
        else return ENET_HOST_ANY;
    
    return val;
}

std::string uint32_to_ip(uint32_t ip) {
    std::string val = "";

    for(uint8_t i = 0; i < 4; i++) {
        if(i) val += ".";
        val += std::to_string((ip >> ((3 - i) * 8)) & 0xFF);
    }

    return val;
}

void send_packet(ENetPeer* peer, uint8_t chan, bool reliable, std::ostringstream& data) {
    if(!peer) {
#ifdef __SERVER__
        std::string str = data.str();
        ENetPacket* packet = enet_packet_create(str.c_str(), str.length(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        for(auto& client : clients)
            enet_peer_send(client.peer, chan, packet);
        enet_packet_destroy(packet);
#endif
        return;
    }
    std::string str = data.str();
    ENetPacket* packet = enet_packet_create(str.c_str(), str.length(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(peer, chan, packet);
    enet_packet_destroy(packet);
}