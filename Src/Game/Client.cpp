#include "../Engine/Engine.hpp"
#include "../Engine/Server.hpp"
#include "../Engine/Map.hpp"
#include "Game.hpp"
#include <enet/enet.h>


constexpr uint32_t projectile_size = (uint32_t)(sizeof  (projectile)
                                              - offsetof(projectile, id)
                                              - sizeof  (projectile)
                                              + offsetof(projectile, hits));

extern ENetHost* client;
extern ENetPeer* peer;

clientinfo playerinfo;
std::vector<clientinfo> playerinfos;
extern void render_map();
extern std::string name;

sc::time_point last_ping = sc::now();

extern bool can_place(std::vector<tower>& towers, tower_t& base_type, double x, double y);

struct client_state {
    size_t                  cur_round;
    size_t                  last_round;
    size_t                  last_route;
    double                  lives;
    bool                    double_speed;
    bool                    paused;
    bool                    running;
    difficulty              diff;
    std::vector<enemy>      enemies;
    std::vector<projectile> projectiles;
    std::vector<tower>      towers;
} cs;

void do_connect() {
    bool connected = false;
    ENetEvent e { };

    packetstream connection_packet;
    connection_packet << N_CONNECT << (uint32_t)name.length() << name;
    send_packet(peer, 0, true, connection_packet);

    enet_peer_ping_interval(peer, ENET_PEER_PING_INTERVAL);

    while(!connected) {
        packetstream p;
        p << N_PING << 0_u32;
        send_packet(peer, 0, true, p);
        if(enet_host_service(client, &e, 1000_u32) > 0) {
            if(e.type == ENET_EVENT_TYPE_DISCONNECT) {
                enet_peer_reset(peer);
                peer = nullptr;
                return;
            }

            if(e.type == ENET_EVENT_TYPE_RECEIVE) {
                packetstream p { (const char*)e.packet->data, e.packet->dataLength };
                enet_packet_destroy(e.packet);

                uint32_t packet_type = NUMMSG;
                uint32_t size        = 0;

                while(p && packet_type != N_SENDMAP) {
                    p.read(nullptr, size);
                    p >> packet_type
                      >> size;
                }

                if(!p)
                    continue;

                std::string str;
                p.read(str, size);
                std::stringstream data { str };
                
                current_map = init_map(data);

                p >> packet_type
                  >> size;

                playerinfo = { };
                uint32_t diff_id { 0 };
                p >> playerinfo.id
                  >> (uint32_t&)cs.cur_round
                  >> (uint32_t&)cs.last_round
                  >> cs.lives
                  >> cs.double_speed
                  >> cs.paused
                  >> cs.running
                  >> diff_id;

                cs.diff  = difficulties[diff_id];
                cs.towers.clear();

                p >> packet_type
                  >> size;

                while(p && packet_type == N_PLAYERINFO) {
                    uint32_t id { 0 };
                    p >> id;

                    if(id == playerinfo.id) {
                        p >> playerinfo.cash;
                        p.read(nullptr, size - 12_u32);
                    } else {
                        clientinfo ci { };
                        p >> ci.id;
                        p >> ci.cash;
                        p.read(ci.name, size - 12_u32);
                        playerinfos.push_back(ci);
                    }

                    p >> packet_type
                      >> size;
                }

                connected = true;
            }
        }
    }
}

void mouse_press_handler(int x, int y) { }
void mouse_release_handler(int x, int y) { }

void render_enemies() {

}

void render_towers() {

}

void render_projectiles() {

}

void render_ui() {

}

void render_frame() {
    if(current_map) {
        render_map();
        render_enemies();
        render_towers();
        render_projectiles();
        render_ui();
    } else {
    //    render_menu();
    }
    render_console();
    render_chat();
}

extern void add_to_chat(std::string msg);
void send_message(std::string msg) {
    if(!peer) return;

    packetstream p { };
    p << N_TEXT
      << msg.length()
      << msg;

    send_packet(peer, 0, false, p);
}

void handle_packet(packetstream& p) {
    uint32_t msgtype = NUMMSG;
    uint32_t size = -1;
    p >> msgtype
      >> size;
    while(p) {
        switch(msgtype) {
            case N_CONNECT:           break;
            case N_TEXT: {
                uint32_t cid { 0 };
                p >> cid;
                std::string str { };
                p.read(str, size);
                for(auto& ci : playerinfos)
                    if(ci.id == cid) {
                        add_to_chat(ci.name + ": " + str);
                        break;
                    }
                break;
            }
            case N_PLACETOWER: {
                uint32_t cid { 0 };
                uint32_t tid { 0 };
                uint08_t base_type { NUMTOWERS };
                double x { 0.0 };
                double y { 0.0 };
                double cost { 0.0 };

                p >> cid
                  >> tid
                  >> base_type
                  >> x
                  >> y
                  >> cost;

                if(base_type >= NUMTOWERS) break;

                tower t { tower_types[base_type], cost, x, y };
                t.id = tid;
                cs.towers.push_back(t);

                if(cid == playerinfo.id)
                    playerinfo.towers.push_back(&cs.towers[cs.towers.size() - 1]);
                else
                    for(auto& pi : playerinfos)
                        if(cid == pi.id) {
                            pi.towers.push_back(&cs.towers[cs.towers.size() - 1]);
                            break; 
                        }

                break;
            }
            case N_UPGRADETOWER: {
                uint32_t tid  { 0   };
                uint08_t top  { 0   },
                         mid  { 0   },
                         bot  { 0   };
                double   cost { 0.0 };

                p >> tid
                  >> top
                  >> mid
                  >> bot
                  >> cost;

                for(auto& t : cs.towers)
                    if(t.id == tid) {
                        t.upgrade_paths[0] = top;
                        t.upgrade_paths[1] = mid;
                        t.upgrade_paths[2] = bot;
                        t.cost += cost;
                        break;
                    }

                break;
            }
            case N_UPDATETARGETING:   break; // todo
            case N_SELLTOWER:         break; // todo
            case N_UPDATE_CASH: {
                uint32_t cid { 0 };
                double cash { 0.0 };
                p >> cid
                  >> cash;

                if(cid == playerinfo.id)
                    playerinfo.cash = cash;
                else
                    for(auto& pi : playerinfos)
                        if(pi.id == cid) {
                            pi.cash = cash;
                            break;
                        }

                break;
            }
            case N_UPDATE_LIVES: p >> cs.lives; break;
            case N_REQUEST_UPDATE:    break;
            case N_UPDATE_ENTITIES: {
                cs.enemies.clear();
                cs.projectiles.clear();
                cs.towers.clear();
                playerinfo.towers.clear();
                for(auto& pi : playerinfos)
                    pi.towers.clear();
                break;
            }
            case N_PING:              break;
            case N_PONG:              break; // todo
            case N_ROUNDINFO: {
                p >> cs.cur_round;
                cs.last_round = cs.cur_round;
                break;
            }
            case N_STARTROUND: {
                cs.running = true;
                cs.projectiles.clear();
                cs.enemies.clear();
                break;
            }
            case N_SET_SPEED: p >> cs.double_speed; break;
            case N_PAUSE:     cs.paused = true;  break;
            case N_RESUME:    cs.paused = false; break;
            case N_ROUNDOVER: cs.running = false; cs.cur_round++; break;
            case N_GAMEOVER:          break; // todo
            case N_RESTART:           break; // todo
            case N_CONTINUE:          break; // todo
            case N_DISCONNECT:        break; // todo
            case N_SPAWN_ENEMY: {
                uint08_t base_type       { NUMENEMIES };
                uint32_t route           { 0 };
                double   health          { 0.0 };
                double   speed           { 0.0 };
                uint16_t immunities      { 0 };
                uint16_t vulnerabilities { 0 };
                uint08_t flags           { 0 };
                uint32_t eid             { 0 };

                p >> base_type
                  >> route
                  >> health
                  >> speed
                  >> immunities
                  >> vulnerabilities
                  >> flags
                  >> eid;

                cs.enemies.push_back(enemy {
                    /* base_type:          */ base_type,
                    /* lock:               */ nullptr,
                    /* route:              */ &current_map->paths[route],
                    /* distance_travelled: */ 0.0,
                    /* pos:                */  current_map->paths[route][0],
                    /* max_health:         */ health,
                    /* health:             */ health,
                    /* speed:              */ speed,
                    /* kill_reward:        */ 0.0,
                    /* immunities:         */ immunities,
                    /* vulnerabilities:    */ vulnerabilities,
                    /* flags:              */ flags,
                    /* slowed_for:         */ 0.0,
                    /* frozen_for:         */ 0.0,
                    /* id:                 */ eid
                });

                break;
            }
            case N_PROJECTILE: {
                projectile pj { };
                p.read(pj.path, size - projectile_size);
                p.read((char*)(&pj + offsetof(projectile, id)), projectile_size);
                pj.texture = { pj.path };
                cs.projectiles.push_back(pj);
                break;
            }
            case N_DETONATE: {
                uint32_t pid { (uint32_t)-1 };
                p >> pid;
                break;
                
                // todo: play projectile explosion animation
                /*for(auto& pj : cs.projectiles)
                    if(pj.pid == pid) {

                        break;
                    }*/
                break;
            }
            case N_DELETE_PROJECTILE: {
                uint32_t pid { (uint32_t)-1 };
                p >> pid;
                break;

                for(size_t i = 0; i < cs.projectiles.size(); i++)
                    if(cs.projectiles[i].pid == pid) {
                        cs.projectiles.erase(cs.projectiles.begin() + i);
                        break;
                    }
                break;
            }
            case N_ENEMY_SURVIVED:
            case N_KILL_ENEMY: {
                uint32_t pid { (uint32_t)-1 };
                p >> pid;
                for(size_t i = 0; i < cs.enemies.size(); i++)
                    if(cs.enemies[i].id == pid) {
                        cs.enemies.erase(cs.enemies.begin() + i);
                        break;
                    }
                break;
            }
            case N_SENDMAP:           break;
            case N_GAMEINFO:          break;
            case N_PLAYERINFO:        break;
            default: {
#ifdef _DEBUG
                conout("Unrecognized packet type ("_str + std::to_string(msgtype) + ')');
#endif
                p.read(nullptr, (size_t)size);
                break;
            }
        }
        p >> msgtype
          >> size;
    }
}

void main_loop() {
    if(peer) {
        if(sc::now() - last_ping > 500_ms) {
            packetstream p;
            p << N_PING << 0_u32;
            send_packet(peer, 0, true, p);
        }
        ENetEvent e;
        while(enet_host_service(client, &e, maxfps ? 1000_u32 / (uint32_t)maxfps : 1_u32) > 0) {
            if(e.type == ENET_EVENT_TYPE_RECEIVE) {
                packetstream stream { (const char*)e.packet->data, e.packet->dataLength };
                enet_packet_destroy(e.packet);
                handle_packet(stream);
            }
            if(e.type == ENET_EVENT_TYPE_DISCONNECT) {
                enet_peer_reset(peer);
                peer = nullptr;
                break;
            }
        }
    }
}
