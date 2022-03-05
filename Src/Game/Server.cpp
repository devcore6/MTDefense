#include "../Engine/Server.hpp"
#include "../Engine/Command.hpp"
#include "../Engine/Map.hpp"
#include "Game.hpp"

#include <random>
#include <future>

extern void conout(std::string);
std::mt19937_64 rng;
game_state gs;
difficulty diff;
ivarp(timeouttime, 30, 60, 600);
std::vector<clientinfo> clientinfos;
extern intmax_t maxclients;

void do_disconnect(client_iterator& cn) {
    for(auto ci = clientinfos.begin(); ci != clientinfos.end(); ci++)
        if(*(client_iterator*)ci->cn == cn) { clientinfos.erase(ci); return; }
}

void init() {
    gs = {
        /* cur_round:       */  0,
        /* last_round:      */  (size_t)-1,
        /* last_route:      */  (size_t)-1,
        /* lives:           */  (double)diff.lives,
        /* double_speed:    */  false,
        /* paused:          */  false,
        /* running:         */  false,
        /* diff:            */  diff,
        /* spawned_enemies: */  0,
        /* done_spawning:   */  true
    };
}

void init_round() {
    gs.last_spawned_tick = sc::now();
    gs.last_round = gs.cur_round;
    gs.done_spawning = false;
}

void update_targeting_priorities() {
    gs.first.clear();
    gs.last.clear();
    gs.strong.clear();
    gs.weak.clear();

    for(auto& e : gs.created_enemies) {
        gs.first.push_back(&e);
        gs.last.push_back(&e);
        gs.strong.push_back(&e);
        gs.weak.push_back(&e);
    }

    std::sort(gs.first.begin(),  gs.first.end(),  [](enemy* e1, enemy* e2) -> bool { return e1->distance_travelled > e2->distance_travelled; });
    std::sort(gs.last.begin(),   gs.last.end(),   [](enemy* e1, enemy* e2) -> bool { return e1->distance_travelled < e2->distance_travelled; });
    std::sort(gs.strong.begin(), gs.strong.end(), [](enemy* e1, enemy* e2) -> bool { return e1->max_health > e2->max_health; });
    std::sort(gs.strong.begin(), gs.strong.end(), [](enemy* e1, enemy* e2) -> bool { return e1->max_health < e2->max_health; });
}

void enemy_cycle() {

}

ivarp(serverthreads, 1, std::thread::hardware_concurrency() - 1, INTMAX_MAX);

void servertick() {
    if(gs.lives > 0 && gs.running) {
        if(gs.last_round != gs.cur_round) init_round();

        if(!gs.done_spawning) {
            size_t pos = 0;
            double time_elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - gs.last_spawned_tick).count();
            
            for(auto& set : gs.diff.round_set.r[gs.cur_round].enemies) {
                size_t n_set = (size_t)(set.amount * gs.diff.enemy_amount_modifier);
                if(gs.spawned_enemies >= pos + n_set) { pos += n_set; continue; }
                size_t n_left = pos + n_set - gs.spawned_enemies;
                size_t n_spawn = (size_t)(time_elapsed * (gs.double_speed ? 0.05 : 0.025) / set.spacing);
                if(n_spawn == 0) break;
                if(n_left > 0) {
                    gs.last_spawned_tick = sc::now();
                    for(auto i : iterate(min(n_spawn, n_left))) {
                        gs.last_route = limit(0, gs.last_route + 1, current_map->paths.size() - 1);
                        gs.spawned_enemies++;
                        uint8_t random_flags = E_FLAG_NONE;

                        if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds
                                                 *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                            random_flags |= E_FLAG_STEALTH;
                        if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds
                                                 *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                            random_flags |= E_FLAG_ARMORED;
                        if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds
                                                 *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                            random_flags |= E_FLAG_SHIELD;

                        gs.created_enemies.push_back(enemy {
                            /* base_type:          */ set.base_type,
                            /* lock:               */ new std::mutex(),
                            /* route:              */ &current_map->paths[gs.last_route],
                            /* distance_travelled: */ 0.0,
                            /* pos:                */  current_map->paths[gs.last_route][0],
                            /* max_health:         */ enemy_types[set.base_type].base_health
                                                   *  gs.diff.enemy_health_modifier
                                                   *  gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier,
                            /* health:             */ enemy_types[set.base_type].base_health
                                                   *  gs.diff.enemy_health_modifier
                                                   *  gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier,
                            /* speed:              */ enemy_types[set.base_type].base_speed
                                                   *  gs.diff.enemy_speed_modifier
                                                   *  gs.diff.round_set.r[gs.cur_round].enemy_speed_multiplier,
                            /* kill_reward:        */ enemy_types[set.base_type].base_kill_reward
                                                   *  gs.diff.enemy_kill_reward_modifier
                                                   *  gs.diff.round_set.r[gs.cur_round].kill_cash_multiplier,
                            /* immunities:         */ (uint16_t)(enemy_types[set.base_type].immunities | gs.diff.enemy_base_immunities),
                            /* vulnerabilities:    */ enemy_types[set.base_type].vulnerabilities,
                            /* flags:              */ (uint8_t)(enemy_types[set.base_type].flags | set.flags | random_flags),
                            /* slowed_for:         */ 0.0,
                            /* frozen_for:         */ 0.0
                        });
                    }
                    update_targeting_priorities();
                    break;
                }
            }
        }

        std::vector<std::future<void>> enemy_cycles;
        for(auto i : iterate(serverthreads)) enemy_cycles.push_back(std::async(enemy_cycle));
    }
}

result<bool, int> handle_packets(packetstream packet, ENetPeer* peer) {
    if(!packet.size()) return false;
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
            ci.id = (uint32_t)clientinfos.size();

            clientinfos.push_back(ci);
            clients[clients.size() - 1].data = (void*)&clientinfos[clientinfos.size() - 1];

            conout(c.name + " connected");
            peer->data = &clients[clients.size() - 1];

            // SEND INFO BACK TO PLAYER
            return true;
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

#ifdef _DEBUG
        conout("packet_type: "_str + std::to_string(packet_type));
#endif

        if(!packet) break;

        uint32_t size = 0;
        packet >> size;

#ifdef _DEBUG
        conout("size: "_str + std::to_string(size));
        conout("expected size: "_str + std::to_string(msgsizes[packet_type][0]));
#endif

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

            case N_PING:            reply << N_PONG << 0_u32; break; // todo: send all connected clients' ping in response

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
            case N_SET_SPEED:       packet >> gs.double_speed; broadcast << N_SET_SPEED << 1_u32 << gs.double_speed; break;
            case N_PAUSE:           if(!gs.paused) { gs.paused =  true; broadcast << N_PAUSE  << 0_u32; } break;
            case N_RESUME:          if( gs.paused) { gs.paused = false; broadcast << N_RESUME << 0_u32; } break;

            case N_ROUNDOVER:       return DISC_MSGERR;
            case N_GAMEOVER:        return DISC_MSGERR;

            case N_RESTART:         if(gs.lives <= 0) { init(); broadcast << N_RESTART << 0_u32; } break;
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

    return reply.size() || broadcast.size();
}
