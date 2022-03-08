#include "../Engine/Engine.hpp"
#include "../Engine/Server.hpp"
#include "../Engine/Map.hpp"
#include "Game.hpp"
#include <enet/enet.h>

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
    std::atomic<double>     lives;
    bool                    double_speed;
    bool                    paused;
    bool                    running;
    difficulty              diff;
    uint32_t                spawned_projectiles;
    std::vector<enemy>      enemies;
    std::vector<projectile> projectiles;
    std::vector<tower>      towers;
};

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
                double lives { 0.0 };
                p >> playerinfo.id
                  >> (uint32_t&)gs.cur_round
                  >> (uint32_t&)gs.last_round
                  >> lives
                  >> gs.double_speed
                  >> gs.paused
                  >> gs.running
                  >> diff_id;

                gs.lives               = lives;
                gs.diff                = difficulties[diff_id];
                gs.spawned_enemies     = 0;
                gs.spawned_projectiles = 0;
                gs.done_spawning       = true;
                gs.created_enemies      .clear();
                gs.first                .clear();
                gs.last                 .clear();
                gs.strong               .clear();
                gs.weak                 .clear();
                gs.projectiles          .clear();
                gs.last_tick           = sc::now();
                gs.last_spawned_tick   = sc::now();
                gs.towers               .clear();

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

void handle_packet(packetstream& p) {
    uint32_t msgtype = NUMMSG;
    uint32_t size = -1;
    p >> msgtype
      >> size;
    while(p) {
        switch(msgtype) {

            default: p.read(nullptr, (size_t)size); break;
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
