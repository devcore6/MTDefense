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
};

game_state gs;

extern void init_towers();
extern void init_enemies();
void init_game() {
    rng.seed(std::random_device{}());
    init_towers();
    init_enemies();
}

extern void deinit_enemies();
void deinit_game() {
    deinit_enemies();
}

void init_match(map_t* map, difficulty diff) {
    current_map = map;
    gs = {
        0, (size_t)-1, (size_t)-1, (double)diff.lives, diff.start_cash_modifier * 750.0, false, sc::now(), sc::now(), diff, 0, { },  { }, true
    };
}

void render_sidebar() {

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

void schedule_spawns(std::vector<spawned_enemy>& scheduled_additions, spawned_enemy& e, double excess_damage) {
    for(auto s : e.spawns) {
        double health = s->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier <= excess_damage;
        if(health > excess_damage) schedule_spawns(scheduled_additions, e, excess_damage - health);
        else scheduled_additions.push_back({
            new std::mutex(),
            false,
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
                            {},
                            false,
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

            // Do towers update cycles
            for(auto i : iterate(threads)) Ts.push_back(std::async([scheduled_additions](size_t i, double time_diff) {
                for(auto t : iterate<std::vector, owned_tower>(gs.towers, threads, i)) {
                    t.tick(time_diff);
                    if(!t.can_fire()) continue;
                    for(auto& e : gs.created_enemies) {
                        e.lock->lock();
                        if(e.schedule_removal) { e.lock->unlock(); continue; }
                        double d = 0.0;
                        if((d = t.fire(e)) != -1.0) {
                            double excess_damage = e.health - d;
                            if(excess_damage >= 0.0) {
                                e.schedule_removal = true;
                                schedule_spawns(scheduled_additions[i], e, excess_damage);
                            } else if(e.spawns_when_damaged) scheduled_additions[i].push_back({
                                new std::mutex(),
                                false,
                                e.route,
                                e.pos,
                                e.distance_travelled,
                                e.spawns_when_damaged->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier,
                                e.spawns_when_damaged->base_health * gs.diff.enemy_health_modifier * gs.diff.round_set->r[gs.cur_round].enemy_health_multiplier,
                                e.spawns_when_damaged->base_speed * gs.diff.enemy_speed_modifier * gs.diff.round_set->r[gs.cur_round].enemy_speed_multiplier,
                                e.spawns_when_damaged->base_kill_reward * gs.diff.enemy_kill_reward_modifier * gs.diff.round_set->r[gs.cur_round].kill_cash_multiplier,
                                e.spawns_when_damaged->scale,
                                (uint16_t)(e.spawns_when_damaged->immunities | gs.diff.enemy_base_immunities), // Why is the explicit conversion required here? Its uint16_t | uint16_t yet VisualStudio says the result is int?
                                e.spawns_when_damaged->vulnerabilities,
                                e.spawns_when_damaged->stealth || e.stealth || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                                e.spawns_when_damaged->armored || e.armored || (rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                                (rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds * gs.diff.round_set->r[gs.cur_round].special_odds_multiplier))) == 0,
                                e.spawns_when_damaged->texture,
                                e.spawns_when_damaged->spawns_when_damaged,
                                e.spawns_when_damaged->spawns,
                                false,
                                e.spawns_when_damaged
                            });
                            e.lock->unlock();
                            break;
                        }
                        e.lock->unlock();
                    }
                }
            }, i, time_diff));

            while(Ts.size() > 0) {
                Ts[Ts.size() - 1].wait();
                Ts.pop_back();
            }

            for(size_t i = 0; i < gs.created_enemies.size(); i++)
                if(gs.created_enemies[i].schedule_removal) gs.created_enemies.erase(gs.created_enemies.begin() + i);

            for(auto i : iterate(threads))
                gs.created_enemies.insert(gs.created_enemies.end(), scheduled_additions[i].begin(), scheduled_additions[i].end());

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
                    delete gs.created_enemies[i].lock;
                    gs.created_enemies.erase(gs.created_enemies.begin() + i);
                }
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
        }
    }
}
