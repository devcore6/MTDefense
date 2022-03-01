#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <enet/enet.h>

uint32_t ip_to_uint32(std::string ip);
std::string uint32_to_ip(uint32_t ip);
void send_packet(ENetPeer* peer, uint8_t chan, bool reliable, std::ostringstream& data);
