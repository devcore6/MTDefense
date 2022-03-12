#include "../Engine/Engine.hpp"
#include "../Engine/Server.hpp"
#include "../Engine/Map.hpp"
#include "Game.hpp"
#include <enet/enet.h>
#include <future>
#include <cxxgui/cxxgui.hpp>

using namespace cxxgui;
svarp(language, "en_US"_str);

const char* get_entry(dictionary_entry& d) {
    for(auto& entry : d)
        if(language == entry.first)
            return entry.second;
    return d["en_US"];
}

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

tower* selecting  { nullptr };
tower* selected   { nullptr };
tower_t* dragging { nullptr };
uint32_t range_texture  { 0 };

texture_t sidebar;
texture_t coin;
texture_t life;

bvarp(autostart_rounds, true);

void deinit_game() {
    glDeleteTextures(1, &range_texture);
}

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
    sc::time_point          last_tick;
} cs;

void try_upgrade(uint8_t i) {
    packetstream p { };
    p << N_UPGRADETOWER
      << 5_u32
      << selected->id
      << i;
    send_packet(peer, 0, true, p);
}

void space_bar() {
    if(!current_map) return;
    if(!peer) return;
    packetstream p { };

    if(!cs.running)
        p << N_STARTROUND
          << 0_u32;
    else if(cs.double_speed) 
        p << N_SET_SPEED
          << 1_u32
          << false
          << N_PAUSE
          << 0_u32;
    else if(cs.paused)
        p << N_RESUME
          << 0_u32;
    else
        p << N_SET_SPEED
          << 1_u32
          << true;
        
    send_packet(peer, 0, true, p);
}

class game_ui: public cxxgui::embeddable {
private:
    bool clicking        = false;
    bool send_click      = false;
    int lastx            = 0;
    int lasty            = 0;
    double update_at     = DBL_MAX;

    void do_update() {
        last_selected = selected;
        update_at = DBL_MAX;

        if(!selected) return;

        std::string upgrade_path = std::to_string(selected->upgrade_paths[0]) + '-'
                                 + std::to_string(selected->upgrade_paths[1]) + '-'
                                 + std::to_string(selected->upgrade_paths[2]);

        std::string upgrade_paths[3] = { std::to_string(selected->upgrade_paths[0] + 1) + '-'
                                       + std::to_string(selected->upgrade_paths[1]    ) + '-'
                                       + std::to_string(selected->upgrade_paths[2]    ),
                                         std::to_string(selected->upgrade_paths[0]    ) + '-'
                                       + std::to_string(selected->upgrade_paths[1] + 1) + '-'
                                       + std::to_string(selected->upgrade_paths[2]    ),
                                         std::to_string(selected->upgrade_paths[0]    ) + '-'
                                       + std::to_string(selected->upgrade_paths[1]    ) + '-'
                                       + std::to_string(selected->upgrade_paths[2] + 1) };

        uint8_t upgrade_count = selected->upgrade_paths[0] + selected->upgrade_paths[1] + selected->upgrade_paths[2];

        float x = (selected->pos_x >= 710.0) ? 0.0f : 1088.0f;

        if(body) delete body;
        body = new vstack {
            new image(
                       tower_types[selected->base_type].animations[upgrade_path].frames[0].textid,
                (float)tower_types[selected->base_type].animations[upgrade_path].frames[0].width,
                (float)tower_types[selected->base_type].animations[upgrade_path].frames[0].height
            )
        };

        body->frame(500.0f,  500.0f,  500.0f,
                   1080.0f, 1080.0f, 1080.0f,
                   alignment_t::center);
        body->offset(x, 0.0f);
        body->padding(16.0f);
        body->background_color(color::background);

        vstack* s = new vstack {
            (new text(selected->custom_name != "" ? selected->custom_name.c_str() : get_entry(tower_types[selected->base_type].name)))
                ->font(title)
        };

        ((vstack*)body)->add(s);

        for(uint8_t i = 0_u8; i < 3_u8; i++) {
            if(upgrade_count == 8) {
                // todo
            } else if(selected->upgrade_paths[i] == 6) {
                // todo
            } else if(selected->upgrade_paths[i] == 0) {
                double cost = tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].base_price * cs.diff.tower_cost_modifier;
                bool owned = false;

                for(auto& id : playerinfo.towers)
                    if(id == selected->id) {
                        owned = true;
                        break;
                    }

                uint32_t c = owned ? (cost > playerinfo.cash ? color::red : color::green) : color::gray;
                
                if(cost > playerinfo.cash && cost < update_at) update_at = cost;

                s->add((new hstack {
                    (new vstack { })
                        ->frame(200.0f, 200.0f, 200.0f, 0.0f, 0.0f, 0.0f, alignment_t::center),
                    symbols::forward()
                        ->offset(-20.0f, 0.0f)
                        ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center),
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text("$"_str + to_string_digits(cost, 2)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::center)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f)
                        ->background_color(c)
                        ->hover_background_color(color_multiply(c, 0.8))
                        ->on_click([&](view*, float, float, void* data) {
                            send_click = false;
                            if(!owned) return;
                            uint8_t val = *(uint8_t*)data;
                            delete data;
                            if(playerinfo.cash >= cost) {
                                try_upgrade(val);
                                last_selected = nullptr;
                            }
                        }, new uint8_t(i))
                })
                    ->padding(16.0f, 0.0f)
                    ->align(alignment_t::center));
            } else {
                double cost = tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].base_price * cs.diff.tower_cost_modifier;

                bool owned = false;

                for(auto& id : playerinfo.towers)
                    if(id == selected->id) {
                        owned = true;
                        break;
                    }

                uint32_t c = owned ? (cost > playerinfo.cash ? color::red : color::green) : color::gray;

                if(cost > playerinfo.cash && cost < update_at) update_at = cost;

                s->add((new hstack {
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f),
                    symbols::forward()
                        ->offset(-20.0f, 0.0f)
                        ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center),
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text("$"_str + to_string_digits(cost, 2)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::center)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f)
                        ->background_color(c)
                        ->hover_background_color(color_multiply(c, 0.8))
                        ->on_click([&](view*, float, float, void* data) {
                            if(!owned) return;
                            send_click = false;
                            uint8_t val = *(uint8_t*)data;
                            delete data;
                            if(playerinfo.cash >= cost) {
                                try_upgrade(val);
                                last_selected = nullptr;
                            }
                        }, new uint8_t(i))
                })
                    ->padding(16.0f, 0.0f)
                    ->align(alignment_t::center));
            }
        }
    }

public:
    tower* last_selected = nullptr;
     game_ui() { }
    ~game_ui() { }

    void release_handler() {
        SDL_GetMouseState(&lastx, &lasty);
        lastx = (int)((float)lastx / (float)width  * 1920.0f);
        lasty = (int)((float)lasty / (float)height * 1080.0f);
        send_click  = true;
        clicking    = false;
    }

    void press_handler() {
        clicking = true;
    }

    void update() {
        if(selected != last_selected || playerinfo.cash >= update_at) do_update();
    }

    void ui() {
        float width_scale  = 1.0f / (float)width  * 1920.0f;
        float height_scale = 1.0f / (float)height * 1080.0f;
        render_gui(clicking, send_click, lastx, lasty, width_scale, height_scale);
    }
} ui;

void handle_packet(packetstream&);

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
                if(!e.packet->data) { enet_packet_destroy(e.packet); continue; }
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

                cs.last_tick = sc::now();

                cs.diff  = difficulties[diff_id];
                cs.towers.clear();

                p >> packet_type
                  >> size;

                while(p && packet_type == N_PLAYERINFO) {
                    uint32_t id { (uint32_t)-1 };
                    p >> id;

                    if(id == playerinfo.id) {
                        p >> playerinfo.cash;
                        p.read(playerinfo.name, size - 12_u32);
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

                if(p) handle_packet(p);

                connected = true;
            }
        }
    }
}

void mouse_press_handler(int _x, int _y) {
    if(!current_map) return;

    double x = (double)_x * 1920.0 / width;
    double y = (double)_y * 1080.0 / height;

    // Todo: There will be more towers than fit on the side bar at once. Too bad for now. Add scrolling later!
    if(x > 1620.0) {
        if(selected) return;

        for(auto i : iterate(sizeof(tower_types) / sizeof(tower_t))) {
            double tx = 1630.0 + 140.0 * (i % 2);
            double ty =  140.0 + 140.0 * (i / 2);

            if(x >= tx && x <= tx + 128.0 && y >= ty && y <= ty + 128.0) {
                if(playerinfo.cash >= tower_types[i].base_price * cs.diff.tower_cost_modifier) dragging = &tower_types[i];
                return;
            }
        }
    } else {
        for(auto& t : cs.towers) {
            double dx = x - t.pos_x;
            double dy = y - t.pos_y;
            if(sqrt(dx * dx + dy * dy) < tower_types[t.base_type].hitbox_radius * 0.8) { selecting = &t; return; }
        }
    }
    ui.press_handler();
}

void mouse_release_handler(int _x, int _y) {
    if(!current_map) return;

    double x = (double)_x * 1920.0 / width;
    double y = (double)_y * 1080.0 / height;

    if(selected) {
        double menux = (selected->pos_x >= 810.0) ? 0.0 : 1088.0;

        if(x >= menux && x <= menux + 532) { ui.release_handler(); return; }
    }

    selected = nullptr;
    if(dragging) {
        bool valid = can_place(cs.towers, *dragging, x, y);

        // todo: discounts?

        double cost = dragging->base_price * cs.diff.tower_cost_modifier;
        if(playerinfo.cash > cost) {
            // todo: localhost

            if(!peer) return;

            packetstream p { };
            p << N_PLACETOWER
              << 17_u32
              << x
              << y
              << dragging->tower_type;

            send_packet(peer, 0, true, p);
        }

        dragging = nullptr;
        if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
    } else if(selecting) {
        double dx = x - selecting->pos_x;
        double dy = y - selecting->pos_y;
        if(sqrt(dx * dx + dy * dy) < tower_types[selecting->base_type].hitbox_radius * 0.8) selected = selecting;
        selecting = nullptr;
    }
}

void render_enemies() {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for(auto& e : cs.enemies) {
        const auto& base = enemy_types[e.base_type];
        double w = base.scale * 512.0;
        double h = base.scale * 512.0;

        glPushMatrix();

            glColor4d(1.0, 1.0, 1.0, (e.flags & E_FLAG_STEALTH) ? 0.5 : 1.0);
            glTranslated(e.pos[0], e.pos[1], 0.0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, base.texture.textid);
            
            glBegin(GL_QUADS);

                glTexCoord2d(0.0, 0.0); glVertex2d(-w*0.55, -h*0.375);
                glTexCoord2d(1.0, 0.0); glVertex2d( w*0.45, -h*0.375);
                glTexCoord2d(1.0, 1.0); glVertex2d( w*0.45,  h*0.625);
                glTexCoord2d(0.0, 1.0); glVertex2d(-w*0.55,  h*0.625);

            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glColor4d(1.0, 1.0, 1.0, 1.0);

        glPopMatrix();
    }
}

void render_towers() {
    if(selected) {
        if(!range_texture) range_texture = render_circle(tower_types[selected->base_type].range * selected->range_mod, "#FFFFFF");

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, range_texture);
        glColor4d(0.0, 0.0, 0.0, 0.25);

        glBegin(GL_QUADS);

            glTexCoord2d(0.0, 0.0);
            glVertex2d(
                selected->pos_x - tower_types[selected->base_type].range * selected->range_mod,
                selected->pos_y - tower_types[selected->base_type].range * selected->range_mod
            );
            glTexCoord2d(1.0, 0.0);
            glVertex2d(
                selected->pos_x + tower_types[selected->base_type].range * selected->range_mod,
                selected->pos_y - tower_types[selected->base_type].range * selected->range_mod
            );
            glTexCoord2d(1.0, 1.0);
            glVertex2d(
                selected->pos_x + tower_types[selected->base_type].range * selected->range_mod,
                selected->pos_y + tower_types[selected->base_type].range * selected->range_mod
            );
            glTexCoord2d(0.0, 1.0);
            glVertex2d(
                selected->pos_x - tower_types[selected->base_type].range * selected->range_mod,
                selected->pos_y + tower_types[selected->base_type].range * selected->range_mod
            );

        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glColor4d(1.0, 1.0, 1.0, 1.0);
    }

    for(auto& t : cs.towers)
        t.render();
}

void render_projectiles() {
    for(auto& p : cs.projectiles) {
        vec2 pos = p.start + p.direction_vector * p.travelled;
        glPushMatrix();
            glTranslated(pos.x, pos.y, 0.0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, p.texture.textid);

            glBegin(GL_QUADS);

                glTexCoord2d(0.0, 0.0); glVertex2d(-(double)p.texture.width / 2.0, -(double)p.texture.height / 2.0);
                glTexCoord2d(1.0, 0.0); glVertex2d( (double)p.texture.width / 2.0, -(double)p.texture.height / 2.0);
                glTexCoord2d(1.0, 1.0); glVertex2d( (double)p.texture.width / 2.0,  (double)p.texture.height / 2.0);
                glTexCoord2d(0.0, 1.0); glVertex2d(-(double)p.texture.width / 2.0,  (double)p.texture.height / 2.0);

            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}

void render_sidebar() {
    if(!sidebar.textid) sidebar = std::move(texture_t("Data/UI/Sidebar.png"));
    if(!coin.textid)    coin    = std::move(texture_t("Data/UI/Coin.png"));
    if(!life.textid)    life    = std::move(texture_t("Data/UI/Life.png"));
    if(!sidebar.textid || !coin.textid || !life.textid) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sidebar.textid);
            
    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(1620.0,    0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1920.0,    0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1920.0, 1080.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1620.0, 1080.0);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, coin.textid);

    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(1540.0, 16.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1604.0, 16.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1604.0, 80.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1540.0, 80.0);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, life.textid);

    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(1540.0,  96.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1604.0,  96.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1604.0, 160.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1540.0, 160.0);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    render_text(std::to_string((size_t)playerinfo.cash),  1524,  48, 255, 255, 255, 255, false, RIGHT, true);
    render_text(std::to_string((size_t)cs.lives), 1524, 128, 255, 255, 255, 255, false, RIGHT, true);
    if(cs.cur_round < cs.diff.rounds_to_win - 1) render_text(std::to_string(cs.cur_round + 1) + '/' + std::to_string(cs.diff.rounds_to_win), 1524, 208, 255, 255, 255, 255, false, RIGHT, true);
    else render_text(std::to_string(cs.cur_round + 1), 1524, 208, 255, 255, 255, 255, false, RIGHT, true);

    render_text("Towers"_str, 1770, 70, 255, 255, 255, 255, false, CENTER, true);
    // Todo: There will be more towers than fit on the side bar at once. Too bad for now. Add scrolling later!

    for(auto i : iterate(sizeof(tower_types) / sizeof(tower_t))) {
        auto& t = tower_types[i];

        double x = 1630.0 + 140.0 * (i % 2);
        double y =  140.0 + 140.0 * (i / 2); // Some compilers might throw warning about truncated integer division result being cast to floating point... its fine its truncated on purpose

        glEnable(GL_TEXTURE_2D);

        glBegin(GL_LINE_LOOP);
            glVertex2d(x,         y);
            glVertex2d(x + 128.0, y);
            glVertex2d(x + 128.0, y + 128.0);
            glVertex2d(x,         y + 128.0);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, t.animations["0-0-0"].frames[0].textid);
            
        glBegin(GL_QUADS);

            glTexCoord2d(0.1, 0.1); glVertex2d(x,         y);
            glTexCoord2d(0.9, 0.1); glVertex2d(x + 128.0, y);
            glTexCoord2d(0.9, 0.9); glVertex2d(x + 128.0, y + 128.0);
            glTexCoord2d(0.1, 0.9); glVertex2d(x,         y + 128.0);

        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        size_t price = (size_t)(t.base_price * cs.diff.tower_cost_modifier);
        render_text("$"_str + std::to_string(price), (int)x + 64, (int)y + 110, 255, price > playerinfo.cash ? 0 : 255, price > playerinfo.cash ? 0 : 255, 255, false, CENTER);
    }
}

void render_ui() {
    if(selected) {
        ui.update();
        ui.ui();
    }
}

void render_dragging() {
    int _x { 0 }, _y { 0 };
    SDL_GetMouseState(&_x, &_y);
    double x = ((double)_x / (double)width  * 1920.0);
    double y = ((double)_y / (double)height * 1080.0);

    bool valid = can_place(cs.towers, *dragging, x, y);

    if(valid) glColor4d(1.0, 1.0, 1.0, 0.4);
    else      glColor4d(1.0, 0.0, 0.0, 0.4);
    if(!range_texture) range_texture = render_circle(dragging->range, "#FFFFFF");

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, range_texture);

    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(x - dragging->range, y - dragging->range);
        glTexCoord2d(1.0, 0.0); glVertex2d(x + dragging->range, y - dragging->range);
        glTexCoord2d(1.0, 1.0); glVertex2d(x + dragging->range, y + dragging->range);
        glTexCoord2d(0.0, 1.0); glVertex2d(x - dragging->range, y + dragging->range);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, dragging->animations["0-0-0"].frames[0].textid);
    if(valid) glColor4d(1.0, 1.0, 1.0, 0.75);
    else      glColor4d(1.0, 0.0, 0.0, 0.75);
        
    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(x - dragging->hitbox_radius, y - dragging->hitbox_radius);
        glTexCoord2d(1.0, 0.0); glVertex2d(x + dragging->hitbox_radius, y - dragging->hitbox_radius);
        glTexCoord2d(1.0, 1.0); glVertex2d(x + dragging->hitbox_radius, y + dragging->hitbox_radius);
        glTexCoord2d(0.0, 1.0); glVertex2d(x - dragging->hitbox_radius, y + dragging->hitbox_radius);

    glEnd();

    glColor4d(1.0, 1.0, 1.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void render_frame() {
    if(current_map) {
        render_map();
        render_enemies();
        render_towers();
        render_projectiles();
        render_sidebar();
        render_ui();
        if(dragging)
            render_dragging();
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

                uint32_t last_selected = -1;
                if(selected)
                    last_selected = selected->id;

                tower t { tower_types[base_type], cost, x, y };
                t.id = tid;
                cs.towers.push_back(t);

                if(cid == playerinfo.id)
                    playerinfo.towers.push_back(tid);
                else
                    for(auto& pi : playerinfos)
                        if(cid == pi.id) {
                            pi.towers.push_back(tid);
                            break; 
                        }

                bool found = false;

                if(selected) {
                    for(auto& t : cs.towers)
                        if(t.id == last_selected) {
                            found = true;
                            selected = &t;
                            break;
                        }
                    if(!found) // should never happen
                        selected = nullptr;
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
            case N_ROUNDOVER: {
                cs.running = false;
                cs.cur_round++;
                if(autostart_rounds) {
                    packetstream p { };
                    p << N_STARTROUND
                      << 0_u32;
                    send_packet(peer, 0, true, p);
                }
                break;
            }
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
                double   dist            { 0.0 };
                uint32_t eid             { 0 };

                p >> base_type
                  >> route
                  >> health
                  >> speed
                  >> immunities
                  >> vulnerabilities
                  >> flags
                  >> dist
                  >> eid;

                cs.enemies.push_back(std::move<enemy>({
                    /* base_type:          */ base_type,
                    /* route:              */ &current_map->paths[route],
                    /* distance_traveled: */ dist,
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
                }));

                break;
            }
            case N_PROJECTILE: {
                projectile pj { };
                p.read(pj.path, size - projectile_size);
                p.read(((char*)&pj) + offsetof(projectile, id), projectile_size);
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
            case N_PLAYERINFO: {
                uint32_t id { (uint32_t)-1 };
                p >> id;

                if(id == playerinfo.id) {
                    p >> playerinfo.cash;
                    p.read(playerinfo.name, size - 12_u32);
                } else {
                    clientinfo ci { };
                    p >> ci.id;
                    p >> ci.cash;
                    p.read(ci.name, size - 12_u32);
                    playerinfos.push_back(ci);
                }
                break;
            }
            case N_REFRESHMENU: ui.last_selected = nullptr; break;
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

ivarp(clientthreads, 1, std::thread::hardware_concurrency() / 2, INTMAX_MAX);

void update_cycles(size_t i, double dt) {
    if(cs.running && !cs.paused)
        for(auto& e : iterate(cs.enemies, clientthreads, i)) {
            e.distance_traveled += e.speed * (cs.double_speed ? 300.0 : 150.0) * dt;
            e.pos = e.route->get_position_at(e.distance_traveled);
        }

    if(!cs.paused)
        for(auto& p : iterate(cs.projectiles, clientthreads, i))
            p.travelled += p.speed * dt;
    // todo: tower idle animations
}

ivarp(maxframetime, 0, 250, 30000);

void main_loop() {
    if(current_map) {
        double dt { std::chrono::duration<double>(sc::now() - cs.last_tick).count() };
        cs.last_tick = sc::now();

        if(peer) {
            if(sc::now() - last_ping > 500_ms) {
                packetstream p;
                p << N_PING << 0_u32;
                send_packet(peer, 0, true, p);
            }
            ENetEvent e;
            do if(enet_host_service(client, &e, 1)) {
                if(e.type == ENET_EVENT_TYPE_RECEIVE) {
                    if(!e.packet->data) { enet_packet_destroy(e.packet); continue; }
                    packetstream stream { (const char*)e.packet->data, e.packet->dataLength };
                    enet_packet_destroy(e.packet);
                    handle_packet(stream);
                }
                if(e.type == ENET_EVENT_TYPE_DISCONNECT) {
                    enet_peer_reset(peer);
                    peer = nullptr;
                    break;
                }
            } while(maxfps ? ((1000_ms / maxfps - std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - cs.last_tick)).count() > 0) : false);

            if(sc::now() - cs.last_tick > std::chrono::milliseconds(maxframetime)) {
                packetstream p;
                p << N_REQUEST_UPDATE << 0_u32;
                send_packet(peer, 0, true, p);
            }
        }

        std::vector<std::future<void>> Ts;
        for(size_t i = 0; i < (size_t)clientthreads; i++) Ts.push_back(std::async(update_cycles, i, dt));
        for(auto& T : Ts) T.get();
    }
}
