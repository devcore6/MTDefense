#include "Game.hpp"
#include "Difficulty.hpp"
#include "Tower.hpp"

#include "../Engine/Engine.hpp"
#include "../Engine/Map.hpp"

#include <thread>
#include <future>
#include <mutex>
#include <random>

using sc = std::chrono::system_clock;

std::mt19937_64 rng;

std::mutex lock;

extern std::vector<tower> towers;

struct game_state {
    size_t cur_round = 0;
    size_t last_round = -1;
    size_t last_route = 0;
    double lives = 0.0;
    double cash = 0.0;

    bool double_speed = false;
    std::chrono::system_clock::time_point last_tick = sc::now();
    std::chrono::system_clock::time_point last_spawned_tick = sc::now();

    difficulty diff;

    size_t spawned_enemies = 0;
    std::vector<spawned_enemy> created_enemies;
    std::vector<owned_tower> towers;

    bool running = false;

    std::vector<spawned_enemy*> first;
    std::vector<spawned_enemy*> last;
    std::vector<spawned_enemy*> strong;
    std::vector<spawned_enemy*> weak;

    struct projectile_t {
        texture_t* text       = nullptr;
        vertex_2d start       = { 0.0, 0.0 };
        vertex_2d vector      = { 0.0, 0.0 };
        double travelled      = 0.0;
        double remaining_life = 0.0;
    };
    std::vector<projectile_t> projectiles;
};

void update_targeting_priorities(game_state& gs) {
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

    std::sort(gs.first.begin(),  gs.first.end(),  [](spawned_enemy* e1, spawned_enemy* e2) -> bool { return e1->distance_travelled > e2->distance_travelled; });
    std::sort(gs.last.begin(),   gs.last.end(),   [](spawned_enemy* e1, spawned_enemy* e2) -> bool { return e1->distance_travelled < e2->distance_travelled; });
    std::sort(gs.strong.begin(), gs.strong.end(), [](spawned_enemy* e1, spawned_enemy* e2) -> bool { return e1->max_health > e2->max_health; });
    std::sort(gs.strong.begin(), gs.strong.end(), [](spawned_enemy* e1, spawned_enemy* e2) -> bool { return e1->max_health < e2->max_health; });
}

game_state gs;

extern void init_towers();
extern void init_enemies();
SDL_Cursor* cursor = nullptr;
void init_game() {
    rng.seed(std::random_device{}());
    init_towers();
    init_enemies();
    SDL_Surface* surface = IMG_Load("Data/UI/Cursor.png");
    if(!surface) { conerr("Could not load cursor"); return; }
    cursor = SDL_CreateColorCursor(surface, 1, 1);
    if(!cursor) { conerr("Could not create cursor"); return; }
    SDL_SetCursor(cursor);
    SDL_ShowCursor(SDL_TRUE);
    SDL_FreeSurface(surface);
}

texture_t* sidebar = nullptr;
texture_t* coin = nullptr;
texture_t* life = nullptr;
texture_t* upgrademenu = nullptr;
extern void deinit_enemies();
void deinit_game() {
    deinit_enemies();
    if(sidebar) delete sidebar;
    if(coin) delete coin;
    if(life) delete life;
    if(upgrademenu) delete upgrademenu;
    if(cursor) SDL_FreeCursor(cursor);
}

void init_match(map_t* map, difficulty diff) {
    current_map = map;
    gs = {
        0, (size_t)-1, (size_t)-1, (double)diff.lives, diff.start_cash_modifier * 750.0, false, sc::now(), sc::now(), diff, 0, { },  { }, true
    };
}

tower* dragging = nullptr;
owned_tower* selecting = nullptr;
owned_tower* selected = nullptr;
GLuint range_texture = 0;
GLuint hitbox_texture = 0;

void mouse_hover_handler(int _x, int _y) {
    double x = (double)_x * 1920.0 / width;
    double y = (double)_y * 1080.0 / height;
    if(dragging && x < 1620.0) {
        bool valid_position = true;

        if(x < dragging->hitbox_radius * 0.8 || x > 1620.0 - dragging->hitbox_radius * 0.8 ||
            y < dragging->hitbox_radius * 0.8 || y > 1080.0 - dragging->hitbox_radius * 0.8) {
            valid_position = false;
        }

        if(valid_position)
            for(auto& p : current_map->paths)
                if(p.distance({ x, y }) < dragging->hitbox_radius * 0.8 + 20.0) { valid_position = false; break; }

        if(valid_position)
            for(auto& c : current_map->clips) if(c.contains({ (double)x, (double)y }, dragging->hitbox_radius * 0.8)) { valid_position = false; break; }

        if(valid_position) {
            if(dragging->place_on_water) {
                bool in_water = false;
                for(auto& w : current_map->water) if(w.contains({ (double)x, (double)y }, dragging->hitbox_radius * 0.8)) { in_water = true; break; }
                if(!in_water) valid_position = false;
            } else for(auto& w : current_map->water) if(w.contains({ (double)x, (double)y }, dragging->hitbox_radius * 0.8)) { valid_position = false; break; }
        }

        if(valid_position) for(auto& t : gs.towers) {
            double dx = (double)x - t.pos_x;
            double dy = (double)y - t.pos_y;
            double d = sqrt(dx * dx + dy * dy);
            if(d < (dragging->hitbox_radius + t.base_type->hitbox_radius) * 0.8) { valid_position = false; break; }
        }

        if(valid_position) glColor4d(1.0, 1.0, 1.0, 0.4);
        else glColor4d(1.0, 0.0, 0.0, 0.4);
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
        if(valid_position) glColor4d(1.0, 1.0, 1.0, 0.75);
        else glColor4d(1.0, 0.0, 0.0, 0.75);
        
        glBegin(GL_QUADS);

            glTexCoord2d(0.0, 0.0); glVertex2d(x - dragging->hitbox_radius, y - dragging->hitbox_radius);
            glTexCoord2d(1.0, 0.0); glVertex2d(x + dragging->hitbox_radius, y - dragging->hitbox_radius);
            glTexCoord2d(1.0, 1.0); glVertex2d(x + dragging->hitbox_radius, y + dragging->hitbox_radius);
            glTexCoord2d(0.0, 1.0); glVertex2d(x - dragging->hitbox_radius, y + dragging->hitbox_radius);

        glEnd();

        glColor4d(1.0, 1.0, 1.0, 1.0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    } else {
        for(auto& t : gs.towers) {
            double dx = x - t.pos_x;
            double dy = y - t.pos_y;
            if(sqrt(dx * dx + dy * dy) < t.base_type->hitbox_radius * 0.8) {
                if(!hitbox_texture) hitbox_texture = render_circle(t.base_type->hitbox_radius, "#FFFFFF");

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, hitbox_texture);
                glColor4d(1.0, 1.0, 1.0, 0.5);

                glBegin(GL_QUADS);

                    glTexCoord2d(0.0, 0.0);
                    glVertex2d(t.pos_x - t.base_type->hitbox_radius * 0.8, t.pos_y - t.base_type->hitbox_radius * 0.8);
                    glTexCoord2d(1.0, 0.0);
                    glVertex2d(t.pos_x + t.base_type->hitbox_radius * 0.8, t.pos_y - t.base_type->hitbox_radius * 0.8);
                    glTexCoord2d(1.0, 1.0);
                    glVertex2d(t.pos_x + t.base_type->hitbox_radius * 0.8, t.pos_y + t.base_type->hitbox_radius * 0.8);
                    glTexCoord2d(0.0, 1.0);
                    glVertex2d(t.pos_x - t.base_type->hitbox_radius * 0.8, t.pos_y + t.base_type->hitbox_radius * 0.8);

                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_TEXTURE_2D);
                glColor4d(1.0, 1.0, 1.0, 1.0);
                t.render();
                return;
            }
        }
        if(hitbox_texture) { glDeleteTextures(1, &hitbox_texture); hitbox_texture = 0; }
        if(range_texture && !selected) { glDeleteTextures(1, &range_texture); range_texture = 0; }
    }
}

void mouse_press_handler(int _x, int _y) {
    double x = (double)_x * 1920.0 / width;
    double y = (double)_y * 1080.0 / height;
    // Todo: There will be more towers than fit on the side bar at once. Too bad for now. Add scrolling later!
    if(x > 1620.0) {
        for(auto i : iterate(towers.size())) {
            double tx = 1630.0 + 140.0 * (i % 2);
            double ty =  140.0 + 140.0 * (i / 2);

            if(x >= tx && x <= tx + 128.0 && y >= ty && y <= ty + 128.0) {
                if(gs.cash >= towers[i].base_price * gs.diff.tower_cost_modifier) dragging = &towers[i];
                return;
            }
        }
    } else {
        for(auto& t : gs.towers) {
            double dx = x - t.pos_x;
            double dy = y - t.pos_y;
            if(sqrt(dx * dx + dy * dy) < t.base_type->hitbox_radius * 0.8) { selecting = &t; return; }
        }
    }
}

void mouse_release_handler(int _x, int _y) {
    double x = (double)_x * 1920.0 / width;
    double y = (double)_y * 1080.0 / height;
    selected = nullptr;
    if(dragging) {
        if(x < dragging->hitbox_radius * 0.8 || x > 1620.0 - dragging->hitbox_radius * 0.8 ||
           y < dragging->hitbox_radius * 0.8 || y > 1080.0 - dragging->hitbox_radius * 0.8) {
            dragging = nullptr;
            if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
            return;
        }

        for(auto& p : current_map->paths)
            if(p.distance({ x, y }) < dragging->hitbox_radius * 0.8 + 20.0) {
                dragging = nullptr;
                if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
                return;
            }

        for(auto& c : current_map->clips) if(c.contains({ (double)x, (double)y }, dragging->hitbox_radius * 0.8)) {
            dragging = nullptr;
            if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
            return;
        }

        if(dragging->place_on_water) {
            bool in_water = false;
            for(auto& w : current_map->water) if(w.contains({ (double)x, (double)y }, dragging->hitbox_radius * 0.8)) { in_water = true; break; }
            if(!in_water) {
                dragging = nullptr;
                if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
                return;
            }
        } else for(auto& w : current_map->water) if(w.contains({ (double)x, (double)y }, dragging->hitbox_radius * 0.8)) {
            dragging = nullptr;
            if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
            return;
        }

        for(auto& t : gs.towers) {
            double dx = (double)x - t.pos_x;
            double dy = (double)y - t.pos_y;
            double d  = sqrt(dx * dx + dy * dy);
            if(d < (dragging->hitbox_radius + t.base_type->hitbox_radius) * 0.8) {
                dragging = nullptr;
                if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
                return;
            }
        }

        // todo: discounts?
        double cost = dragging->base_price * gs.diff.tower_cost_modifier;
        if(gs.cash > cost) {
            gs.towers.push_back(owned_tower(dragging, cost, (double)x, (double)y));
            gs.cash -= cost;
        }

        dragging = nullptr;
        if(range_texture) { glDeleteTextures(1, &range_texture); range_texture = 0; }
    } else if(selecting) {
        double dx = x - selecting->pos_x;
        double dy = y - selecting->pos_y;
        if(sqrt(dx * dx + dy * dy) < selecting->base_type->hitbox_radius * 0.8) selected = selecting;
        selecting = nullptr;
    }
}

void render_sidebar() {

    if(!sidebar)     sidebar     = new texture_t("Data/UI/Sidebar.png");
    if(!coin)        coin        = new texture_t("Data/UI/Coin.png");
    if(!life)        life        = new texture_t("Data/UI/Life.png");
    if(!upgrademenu) upgrademenu = new texture_t("Data/UI/Upgrades.png");
    if(!sidebar || !coin || !life || !upgrademenu) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sidebar->textid);
            
    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(1620.0,    0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1920.0,    0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1920.0, 1080.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1620.0, 1080.0);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, coin->textid);

    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(1540.0, 16.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1604.0, 16.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1604.0, 80.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1540.0, 80.0);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, life->textid);

    glBegin(GL_QUADS);

        glTexCoord2d(0.0, 0.0); glVertex2d(1540.0,  96.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1604.0,  96.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1604.0, 160.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(1540.0, 160.0);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    render_text(std::to_string((size_t)gs.cash),  1524,  48, 255, 255, 255, 255, false, RIGHT, true);
    render_text(std::to_string((size_t)gs.lives), 1524, 128, 255, 255, 255, 255, false, RIGHT, true);
    if(gs.cur_round < gs.diff.rounds_to_win - 1) render_text(std::to_string(gs.cur_round + 1) + '/' + std::to_string(gs.diff.rounds_to_win), 1524, 208, 255, 255, 255, 255, false, RIGHT, true);
    else render_text(std::to_string(gs.cur_round + 1), 1524, 208, 255, 255, 255, 255, false, RIGHT, true);

    render_text("Towers"_str, 1770, 70, 255, 255, 255, 255, false, CENTER, true);
    // Todo: There will be more towers than fit on the side bar at once. Too bad for now. Add scrolling later!

    for(auto i : iterate(towers.size())) {
        auto& t = towers[i];

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

        size_t price = (size_t)(t.base_price * gs.diff.tower_cost_modifier);
        render_text("$"_str + std::to_string(price), (int)x + 64, (int)y + 110, 255, price > gs.cash ? 0 : 255, price > gs.cash ? 0 : 255, 255, false, CENTER);
    }

    if(selected) {
        double x = (selected->pos_x >= 810.0) ? 0.0 : 1220.0;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, upgrademenu->textid);
            
        glBegin(GL_QUADS);

            glTexCoord2d(0.0, 0.0); glVertex2d(x,         280.0);
            glTexCoord2d(1.0, 0.0); glVertex2d(x + 400.0, 280.0);
            glTexCoord2d(1.0, 1.0); glVertex2d(x + 400.0, 980.0);
            glTexCoord2d(0.0, 1.0); glVertex2d(x,         980.0);

        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void render_menu() {

}

bvarp(encountered_shield, false);
bvarp(encountered_armor, false);
bvarp(encountered_stealth, false);
bvarp(autostart_rounds, true);

ivarp(threads, 2, std::thread::hardware_concurrency() - 1, INTMAX_MAX);

void init_round() {
    gs.last_spawned_tick = sc::now();
    gs.last_round = gs.cur_round;
}

double count_lives(enemy* e) {
    double lives = e->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier;
    for(auto& s : e->spawns)
        lives += count_lives(s);
    return lives;
};

double schedule_spawns(std::vector<spawned_enemy>& scheduled_additions, enemy* t, spawned_enemy& e, double excess_damage) {
    double ret = t->base_kill_reward * gs.diff.enemy_kill_reward_modifier * gs.diff.round_set->r[gs.cur_round].kill_cash_multiplier;
    for(auto s : t->spawns) {
        double health = s->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier;
        if(health <= excess_damage) ret += schedule_spawns(scheduled_additions, s, e, excess_damage - health);
        else scheduled_additions.push_back({
            new std::mutex(),
            false,
            0.0,
            e.route,
            e.pos,
            e.distance_travelled,
            health,
            health - excess_damage,
            s->base_speed * gs.diff.enemy_speed_modifier * gs.diff.round_set->r[gs.cur_round].enemy_speed_multiplier,
            s->base_kill_reward * gs.diff.enemy_kill_reward_modifier * gs.diff.round_set->r[gs.cur_round].kill_cash_multiplier,
            s->scale,
            (uint16_t)(s->immunities | gs.diff.enemy_base_immunities), // Why is the explicit conversion required here? Its uint16_t | uint16_t yet VisualStudio says the result is int?
            s->vulnerabilities,
            s->stealth || e.stealth || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
            s->armored || e.armored || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
            (rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
            s->texture,
            s->spawns_when_damaged,
            s->spawns,
            false,
            s
        });
    }
    return ret;
}

void game_tick() {
    if(gs.lives > 0) {
        std::vector<std::future<void>> Ts;
        if(gs.running) {
            if(gs.last_round != gs.cur_round) init_round();

            // Spawning can't really be done multithreaded or it would cause data races. Mutexes are an option but it's slower than just doing it in one thread
            // todo: Only works until round 150 for now

            size_t pos = 0;
            double time_elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - gs.last_spawned_tick).count();
            for(auto& set : gs.diff.round_set->r[gs.cur_round].e) {
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
                        gs.created_enemies.push_back(spawned_enemy {
                            new std::mutex(),
                            false,
                            0.0,
                            &current_map->paths[gs.last_route],
                            current_map->paths[gs.last_route][0],
                            0.0,
                            (*(set.e))->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier,
                            (*(set.e))->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier,
                            (*(set.e))->base_speed * gs.diff.enemy_speed_modifier * gs.diff.round_set->r[gs.cur_round].enemy_speed_multiplier,
                            (*(set.e))->base_kill_reward * gs.diff.enemy_kill_reward_modifier * gs.diff.round_set->r[gs.cur_round].kill_cash_multiplier,
                            (*(set.e))->scale,
                            (uint16_t)((*(set.e))->immunities | gs.diff.enemy_base_immunities), // Why is the explicit conversion required here? Its uint16_t | uint16_t yet VisualStudio says the result is int?
                            (*(set.e))->vulnerabilities,
                            (*(set.e))->stealth || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                            (*(set.e))->armored || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                            (rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                            (*(set.e))->texture,
                            (*(set.e))->spawns_when_damaged,
                            (*(set.e))->spawns,
                            false,
                            (*(set.e))
                        });
                    }
                    update_targeting_priorities(gs);
                    break;
                }
            }
    
            double time_diff = std::chrono::duration<double>(sc::now() - gs.last_tick).count();
            gs.last_tick = sc::now();
            // Update enemy positions
            for(auto i : iterate(threads)) Ts.push_back(std::async([](size_t i, double time_diff) {
                for(auto& e : iterate<std::vector, spawned_enemy>(gs.created_enemies, threads, i)) {
                    if(e.survived) continue;
                    e.distance_travelled += e.speed * (gs.double_speed ? 300.0 : 150.0) * time_diff;
                    e.pos = e.route->get_position_at(e.distance_travelled);
                    if(e.pos.x == e.route->vertices[e.route->vertices.size() - 1].x && e.pos.y == e.route->vertices[e.route->vertices.size() - 1].y) {
                        e.survived = true;
                        double lives = count_lives(e.base_enemy);
                        lock.lock();
                        gs.lives -= lives;
                        lock.unlock();
                    }
                }
            }, i, time_diff));

            while(Ts.size() > 0) {
                Ts[Ts.size() - 1].wait();
                Ts.pop_back();
            }

            std::vector<spawned_enemy>* scheduled_additions = new std::vector<spawned_enemy>[threads];
            std::vector<game_state::projectile_t>* scheduled_projectiles = new std::vector<game_state::projectile_t>[threads];
            double* cash_added = new double[threads](0.0);

            // Do towers update cycles
            for(auto i : iterate(threads)) Ts.push_back(std::async([scheduled_additions, scheduled_projectiles, cash_added](size_t i, double time_diff)  {
                for(auto& t : iterate<std::vector, owned_tower>(gs.towers, threads, i)) {
                    t.tick(time_diff);
                    if(!t.can_fire()) continue;
                    std::vector<spawned_enemy*>* enemies = nullptr;
                    switch(t.targeting_mode) {
                        case TARGETING_FIRST:  { enemies = &gs.first;  break; }
                        case TARGETING_LAST:   { enemies = &gs.last;   break; }
                        case TARGETING_STRONG: { enemies = &gs.strong; break; }
                        case TARGETING_WEAK:   { enemies = &gs.weak;   break; }
                        default:               { enemies = &gs.first;  break; }
                    }
                    for(auto& e : *enemies) {
                        e->lock->lock();
                        if(e->schedule_removal) { e->lock->unlock(); continue; }
                        double dx   = e->pos.x - t.pos_x;
                        double dy   = e->pos.y - t.pos_y;
                        double dist = sqrt(dx * dx + dy * dy);
                        double d = 0.0;
                        if((d = t.fire(e)) != -1.0) {
                            if(t.last_projectile) scheduled_projectiles[i].push_back({
                                &t.last_projectile->texture,
                                { t.pos_x, t.pos_y },
                                e->pos - vertex_2d { t.pos_x, t.pos_y },
                                0.0,
                                dist / t.last_projectile->speed
                            });
                            double excess_damage = e->health - d;
                            if(excess_damage >= 0.0) {
                                e->remove_in = dist / t.last_projectile->speed;
                                e->schedule_removal = true;
                                cash_added[i] += schedule_spawns(scheduled_additions[i], e->base_enemy, *e, excess_damage);
                            } else if(e->spawns_when_damaged) scheduled_additions[i].push_back({
                                new std::mutex(),
                                false,
                                0.0,
                                e->route,
                                e->pos,
                                e->distance_travelled,
                                e->spawns_when_damaged->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier,
                                e->spawns_when_damaged->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier,
                                e->spawns_when_damaged->base_speed * gs.diff.enemy_speed_modifier * gs.diff.round_set->r[gs.cur_round].enemy_speed_multiplier,
                                e->spawns_when_damaged->base_kill_reward * gs.diff.enemy_kill_reward_modifier * gs.diff.round_set->r[gs.cur_round].kill_cash_multiplier,
                                e->spawns_when_damaged->scale,
                                (uint16_t)(e->spawns_when_damaged->immunities | gs.diff.enemy_base_immunities), // Why is the explicit conversion required here? Its uint16_t | uint16_t yet VisualStudio says the result is int?
                                e->spawns_when_damaged->vulnerabilities,
                                e->spawns_when_damaged->stealth || e->stealth || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                                e->spawns_when_damaged->armored || e->armored || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                                (rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                                e->spawns_when_damaged->texture,
                                e->spawns_when_damaged->spawns_when_damaged,
                                e->spawns_when_damaged->spawns,
                                false,
                                e->spawns_when_damaged
                            });
                            e->lock->unlock();
                            break;
                        }
                        e->lock->unlock();
                    }
                }
            }, i, time_diff));

            while(Ts.size() > 0) {
                Ts[Ts.size() - 1].wait();
                Ts.pop_back();
            }

            bool need_update = false;
            for(size_t i = 0; i < gs.created_enemies.size(); i++)
                if(gs.created_enemies[i].schedule_removal) {
                    gs.created_enemies[i].remove_in -= time_diff;
                    if(gs.created_enemies[i].remove_in > 0.0) continue;
                    need_update = true;
                    delete gs.created_enemies[i].lock;
                    gs.created_enemies.erase(gs.created_enemies.begin() + i);
                }

            for(auto i : iterate(threads))
                for(auto e : scheduled_additions[i]) {
                    need_update = true;
                    gs.created_enemies.push_back(e);
                }

            // Todo: Make the projectiles do the actual damage instead of the towers. Also, allow projectiles to penetrate through enemies and damage multiple targets
            for(auto i : iterate(threads))
                for(auto p : scheduled_projectiles[i]) {
                    gs.projectiles.push_back(p);
                }

            for(auto i : iterate(threads))
                gs.cash += cash_added[i];

            delete[] cash_added;
            delete[] scheduled_projectiles;
            delete[] scheduled_additions;

            // Render the enemies
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            for(size_t i = 0; i < gs.created_enemies.size(); i++) {
                auto& e = gs.created_enemies[i];
                if(!e.survived) {
                    double w = e.texture.width * e.scale * 2.0;
                    double h = e.texture.height * e.scale * 2.0;
                    glPushMatrix();

                        glTranslated(e.pos[0], e.pos[1], 0.0);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, e.texture.textid);
            
                        glBegin(GL_QUADS);

                            glTexCoord2d(0.0, 0.0); glVertex2d(-w*0.55, -h*0.375);
                            glTexCoord2d(1.0, 0.0); glVertex2d( w*0.45, -h*0.375);
                            glTexCoord2d(1.0, 1.0); glVertex2d( w*0.45,  h*0.625);
                            glTexCoord2d(0.0, 1.0); glVertex2d(-w*0.55,  h*0.625);

                        glEnd();

                        glBindTexture(GL_TEXTURE_2D, 0);
                        glDisable(GL_TEXTURE_2D);

                    glPopMatrix();
                } else {
                    need_update = true;
                    delete gs.created_enemies[i].lock;
                    gs.created_enemies.erase(gs.created_enemies.begin() + i);
                    i--;
                }
            }

            if(need_update) update_targeting_priorities(gs);

            if(selected) {
                if(!range_texture) range_texture = render_circle(selected->base_type->range * selected->range_mod, "#FFFFFF");

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, range_texture);
                glColor4d(0.0, 0.0, 0.0, 0.25);

                glBegin(GL_QUADS);

                    glTexCoord2d(0.0, 0.0);
                    glVertex2d(
                        selected->pos_x - selected->base_type->range * selected->range_mod,
                        selected->pos_y - selected->base_type->range * selected->range_mod
                    );
                    glTexCoord2d(1.0, 0.0);
                    glVertex2d(
                        selected->pos_x + selected->base_type->range * selected->range_mod,
                        selected->pos_y - selected->base_type->range * selected->range_mod
                    );
                    glTexCoord2d(1.0, 1.0);
                    glVertex2d(
                        selected->pos_x + selected->base_type->range * selected->range_mod,
                        selected->pos_y + selected->base_type->range * selected->range_mod
                    );
                    glTexCoord2d(0.0, 1.0);
                    glVertex2d(
                        selected->pos_x - selected->base_type->range * selected->range_mod,
                        selected->pos_y + selected->base_type->range * selected->range_mod
                    );

                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_TEXTURE_2D);
                glColor4d(1.0, 1.0, 1.0, 1.0);
            }

            // Render the towers
            for(auto t : gs.towers)
                t.render();

            // todo: round 150 again
            size_t total_enemies = 0;
            for(auto& set : gs.diff.round_set->r[gs.cur_round].e)
                total_enemies += (size_t)(set.amount * gs.diff.enemy_amount_modifier);

            if(gs.created_enemies.size() == 0 && gs.spawned_enemies == total_enemies) {
                gs.running = autostart_rounds;
                gs.cur_round++;
                gs.cash += 100.0 + gs.last_round;
                gs.spawned_enemies = 0;
                gs.created_enemies.clear();
            }

            // Update projectiles
            for(size_t i = 0; i < gs.projectiles.size(); i++) {
                gs.projectiles[i].remaining_life -= time_diff;
                gs.projectiles[i].travelled += time_diff;
                if(gs.projectiles[i].remaining_life <= 0.0) { gs.projectiles.erase(gs.projectiles.begin() + i); i--; }
            }

            // Render projectiles
            for(auto p : gs.projectiles) {
                vertex_2d pos = p.start + p.vector * (p.travelled / (p.remaining_life + p.travelled));
                glPushMatrix();
                    glTranslated(pos.x, pos.y, 0.0);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, p.text->textid);

                    glBegin(GL_QUADS);

                        glTexCoord2d(0.0, 0.0); glVertex2d(-(double)p.text->width / 2.0, -(double)p.text->height / 2.0);
                        glTexCoord2d(1.0, 0.0); glVertex2d( (double)p.text->width / 2.0, -(double)p.text->height / 2.0);
                        glTexCoord2d(1.0, 1.0); glVertex2d( (double)p.text->width / 2.0,  (double)p.text->height / 2.0);
                        glTexCoord2d(0.0, 1.0); glVertex2d(-(double)p.text->width / 2.0,  (double)p.text->height / 2.0);

                    glEnd();

                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            }
        }

        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        mouse_hover_handler(x, y);
    }
}
