#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <enet/enet.h>

// Screw your macros
// Spent like an hour figuring out why this won't build. Enet includes something that defines min, max and clamp as macros...
#undef min
#undef max
#undef clamp

template<class CharT> class basic_packetstream;

using packetstream = basic_packetstream<char>;

uint32_t ip_to_uint32(std::string ip);
std::string uint32_to_ip(uint32_t ip);
void send_packet(ENetPeer* peer, uint8_t chan, bool reliable, packetstream& data);
