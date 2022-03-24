#pragma once
#include "tools.hpp"
#include "network.hpp"
#include <chrono>

using sc = std::chrono::system_clock;

struct client_t {
    uint32_t cn;
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

constexpr const char* disconnect_reason[NUMDISCS] = {
    "",
    "end of packet",
    "message error",
    "server is full",
    "connection timed out",
    "unknown reason"
};

extern std::vector<client_t> clients;
extern void do_disconnect(uint32_t cn);
extern result<void, int> handle_packets(packetstream packet, ENetPeer* peer);
extern void servertick();
client_t* get_raw_client(uint32_t cn);
