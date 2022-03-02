#include "../Engine/Server.hpp"
#include "../Engine/Command.hpp"
#include "Game.hpp"

#include <random>

std::mt19937_64 rng;

ivarp(timeouttime, 30, 60, 600);

gamestate gs;

struct clientinfo {
    uint32_t id = 0;
    client_iterator* cn = nullptr;
    sc::time_point last_message = sc::now();
    double cash = 0.0;
};

std::vector<clientinfo> clientinfos;

extern intmax_t maxclients;

void do_disconnect(client_iterator& cn) {
    for(auto ci = clientinfos.begin(); ci != clientinfos.end(); ci++)
        if(*ci->cn == cn) { clientinfos.erase(ci); return; }
}

void init(gamestate& state) {
    state = { };
    // todo
}

result<void, int> handle_packets(packetstream packet, ENetPeer* peer) {
    if(!packet.size()) return { };

    if(!peer->data) {
        uint32_t packet_type = NUMMSG;
        packet >> packet_type;

        if(packet_type == N_CONNECT) {
            if(clients.size() == maxclients)
                return DISC_SERVER_FULL;

            uint32_t size = 0;
            packet >> size;

            client_t c;
            c.peer = peer;
            packet >> c.name;
            clients.push_back(c);
            for(auto cn = clients.begin(); cn != clients.end(); cn++) { cn->cn = cn; }

            clientinfo ci;
            ci.cn = &clients[clients.size() - 1].cn;
            ci.id = clientinfos.size();

            clientinfos.push_back(ci);
            clients[clients.size() - 1].data = (void*)&clientinfos[clientinfos.size() - 1];
            return { };
        }

        return DISC_MSGERR;
    }

    auto& cn = ((client_t*)(peer->data))->cn;
    auto client = (clientinfo*)cn->data;
    client->last_message = sc::now();

    packetstream reply { };
    packetstream broadcast { };

    while(true) {
        uint32_t packet_type = NUMMSG;
        packet >> packet_type;

        if(!packet) break;

        uint32_t size = 0;
        packet >> size;

        if(msgsizes[packet_type][0] != -1 && msgsizes[packet_type][0] != size) return DISC_MSGERR;

        switch(packet_type) {
            case N_CONNECT:         return DISC_MSGERR;
            case N_TEXT: {
                char* s = new char[size];
                packet.read(s, size);
                broadcast << N_TEXT
                          << size;
                broadcast.write(s, size);
                delete[] s;
                break;
            }

            case N_PLACETOWER:      break;                            // todo
            case N_UPGRADETOWER:    break;                            // todo
            case N_UPDATETARGETING: break;                            // todo
            case N_SELLTOWER:       break;                            // todo

            case N_UPDATE_CASH:     return DISC_MSGERR;
            case N_UPDATE_LIVES:    return DISC_MSGERR;

            case N_REQUEST_UPDATE: {
                for(auto& ci : clientinfos)
                    reply << N_UPDATE_CASH
                          << 12_u32
                          << ci.id
                          << ci.cash;

                reply << N_UPDATE_LIVES
                      << 12_u32
                      << gs.lives
                      << N_UPDATE_ENTITIES;
                // todo: entities_packet
                break;
            }

            case N_UPDATE_ENTITIES: return DISC_MSGERR;

            case N_PING:            reply << N_PONG << 0_u32; break;

            case N_PONG:            return DISC_MSGERR;
            case N_ROUNDINFO:       return DISC_MSGERR;

            case N_STARTROUND: {
                if(!gs.running && gs.last_round != gs.cur_round) {
                    gs.running = true;
                    gs.last_round = gs.cur_round;
                    broadcast << N_STARTROUND << 0_u32 << N_ROUNDINFO << 4_u32 << gs.cur_round;
                }
                break;
            }
            case N_SET_SPEED:       packet >> gs.fast_forward; broadcast << N_SET_SPEED << 1_u32 << gs.fast_forward; break;
            case N_PAUSE:           if(!gs.paused) { gs.paused =  true; broadcast << N_PAUSE  << 0_u32; } break;
            case N_RESUME:          if( gs.paused) { gs.paused = false; broadcast << N_RESUME << 0_u32; } break;

            case N_ROUNDOVER:       return DISC_MSGERR;
            case N_GAMEOVER:        return DISC_MSGERR;

            case N_RESTART:         if(gs.lives <= 0) { init(gs); broadcast << N_RESTART << 0_u32; break; }
            case N_CONTINUE:        break;                            // todo
            case N_DISCONNECT:      return DISC_OK;

            // Forward unrecognized packets to all, will help with moddability
            default: {
                char* s = new char[size];
                packet.read(s, size);
                broadcast << packet_type << size;
                broadcast.write(s, size);
                delete[] s;
                break;
            }
        }
    }

    if(    reply.size()) send_packet(cn->peer, 0, true, reply);
    if(broadcast.size()) send_packet(nullptr,  0, true, broadcast);

    return { };
}
