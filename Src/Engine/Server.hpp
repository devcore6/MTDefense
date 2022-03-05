#pragma once
#include "Tools.hpp"
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
    void* data = nullptr;
};

enum {
    DISC_OK = 0,
    DISC_EOP,
    DISC_MSGERR,
    DISC_SERVER_FULL,
    DISC_TIMEOUT,
    DISC_UNKNOWN,
    NUMDISCS
};

constexpr const char* disconnect_reason[NUMDISCS - 1] = {
    "",
    "end of packet",
    "server is full",
    "message error",
    "connection timed out"
    "unknown reason"
};

extern std::vector<client_t> clients;
extern void do_disconnect(client_iterator&);
extern result<bool, int> handle_packets(packetstream packet, ENetPeer* peer);
extern void servertick();
