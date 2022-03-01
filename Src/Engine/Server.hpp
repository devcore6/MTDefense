#pragma once
#include "Network.hpp"
#include <chrono>

using sc = std::chrono::system_clock;

struct client_t;

using client_iterator = std::vector<client_t>::iterator;

struct client_t {
    client_iterator cn { };
    ENetPeer* peer = nullptr;
    std::string name = "";
    bool pending_disconnect = false;
    sc::time_point disconnect_at { };
};

extern std::vector<client_t> clients;
extern void handle_packets(ENetPacket* packet, ENetPeer* peer);
