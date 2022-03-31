#include "../engine/server.hpp"
#include "../engine/command.hpp"
#include "../engine/map.hpp"
#include "game.hpp"

#include <random>
#include <future>

extern void conout(std::string);
extern void conerr(std::string);

std::mt19937_64 rng;
game_state gs;
difficulty diff;
ivarp(timeouttime, 30, 60, 600);
std::vector<clientinfo> clientinfos;
extern intmax_t maxclients;

bool can_place(std::vector<tower>& towers, tower_t& base_type, double x, double y) {
    if(!current_map) return false;

    if(x < base_type.hitbox_radius * 0.8 || x > 1620.0 - base_type.hitbox_radius * 0.8 ||
       y < base_type.hitbox_radius * 0.8 || y > 1080.0 - base_type.hitbox_radius * 0.8) return false;

    for(auto& p : current_map->paths)
        if(p.distance({ x, y }) < base_type.hitbox_radius * 0.8 + 20.0)
            return false;

    for(auto& c : current_map->clips)
        if(c.contains({ x, y }, base_type.hitbox_radius * 0.8))
            return false;

    bool in_water = false;
    for(auto& w : current_map->water)
        if(w.contains({ x, y }, base_type.hitbox_radius * 0.8)) { in_water = true; break; }

    if(in_water != base_type.place_on_water) return false;

    for(auto& t : towers) {
        double dx = (double)x - t.pos_x;
        double dy = (double)y - t.pos_y;
        double d = sqrt(dx * dx + dy * dy);
        if(d < (base_type.hitbox_radius + tower_types[t.base_type].hitbox_radius) * 0.8)
            return false;
    }

    return true;
}

constexpr uint32_t projectile_size = (uint32_t)(offsetof(projectile, hits) - offsetof(projectile, id));

iconst(DIFF_EASY,       DIFF_EASY);
iconst(DIFF_MEDIUM,     DIFF_MEDIUM);
iconst(DIFF_HARD,       DIFF_HARD);
iconst(DIFF_IMPOSSIBLE, DIFF_IMPOSSIBLE);

void do_disconnect(uint32_t cn) {
    for(auto ci = clientinfos.begin(); ci != clientinfos.end(); ci++)
        if(ci->cn == cn) { clientinfos.erase(ci); return; }
}

void init() {
    gs.cur_round           = 0;
    gs.last_round          = (size_t)-1;
    gs.last_route          = (size_t)-1;
    gs.lives               = (double)diff.lives;
    gs.double_speed        = false;
    gs.paused              = false;
    gs.running             = false;
    gs.diff                = diff;
    gs.spawned_enemies     = 0;
    gs.all_spawned_enemies = 0;
    gs.spawned_projectiles = 0;
    gs.spawned_towers      = 0;
    gs.done_spawning       = true;
    gs.created_enemies      .clear();
    gs.first                .clear();
    gs.last                 .clear();
    gs.strong               .clear();
    gs.weak                 .clear();
    gs.projectiles          .clear();
    gs.last_tick           = sc::now();
    gs.towers               .clear();
    gs.time_elapsed        = 0.0;
}

command(play, [](std::vector<std::string>& args) {
    if(args.size() < 3) {
        conerr("Usage: play <map> <difficulty>");
        return;
    }
    current_map = init_map(args[1]);
    if(current_map) {
        diff = difficulties[std::stoi(args[2])];
        init();
    } else conerr("Couldn't load map");
})

#ifdef __SERVER__
command(echo, [](std::vector<std::string>& args) {
    if(args.size() == 1) conout("Usage: echo \"message\";");
    else conout(args[1]);
})
#endif

void init_round() {
    gs.running             = true;
    gs.last_round          = gs.cur_round;
    gs.done_spawning       = false;
    gs.spawned_enemies     = 0;
    gs.spawned_projectiles = 0;
    gs.time_elapsed        = 0;
    gs.projectiles.clear();
}

void update_targeting_priorities() {
    gs.first .clear();
    gs.last  .clear();
    gs.strong.clear();
    gs.weak  .clear();

    for(auto& e : gs.created_enemies) {
        gs.first .push_back(&e);
        gs.last  .push_back(&e);
        gs.strong.push_back(&e);
        gs.weak  .push_back(&e);
    }

    std::sort(gs.first .begin(), gs.first .end(), [](enemy* e1, enemy* e2) -> bool { return e1->distance_traveled > e2->distance_traveled; });
    std::sort(gs.last  .begin(), gs.last  .end(), [](enemy* e1, enemy* e2) -> bool { return e1->distance_traveled < e2->distance_traveled; });
    std::sort(gs.strong.begin(), gs.strong.end(), [](enemy* e1, enemy* e2) -> bool { return e1->max_health         > e2->max_health;         });
    std::sort(gs.strong.begin(), gs.strong.end(), [](enemy* e1, enemy* e2) -> bool { return e1->max_health         < e2->max_health;         });
}

double count_lives(enemy_t e) {
    double lives = e.base_health * gs.diff.enemy_health_modifier * gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier;
    for(auto& s : e.spawns)
        lives += count_lives(enemy_types[s]);
    return lives;
};

ivarp(serverthreads, 1, std::thread::hardware_concurrency() - 1, INTMAX_MAX);

std::vector<enemy*> enemy_cycle(double dt, size_t i) {
    std::vector<enemy*> ret { };
    for(auto& e : iterate(gs.created_enemies, serverthreads, i)) {
        e.distance_traveled += e.speed * 150.0 * dt;
        if(e.distance_traveled >= e.route->length()) {
            double lives = count_lives(enemy_types[e.base_type]);
            ret.push_back(&e);
            gs.lives -= lives;
        }
    }
    return ret;
}

std::vector<projectile> tower_cycle(double dt, size_t i) {
    std::vector<projectile> ret { };
    for(auto& t : iterate(gs.towers, serverthreads, i)) {
        t.tick(dt);
        if(!t.can_fire()) continue;

        std::vector<enemy*>* enemies = &gs.first;
        switch(t.targeting_mode) {
         // case TARGETING_FIRST:  enemies = &gs.first;  break; not needed
            case TARGETING_LAST:   enemies = &gs.last;   break;
            case TARGETING_STRONG: enemies = &gs.strong; break;
            case TARGETING_WEAK:   enemies = &gs.weak;   break;
         // default:               enemies = &gs.first;  break; not needed
        }

        for(auto& e : *enemies) {
            auto p = t.fire(e, enemies);
            if(p) {
                ret.push_back(p.ok);
                break;
            }
        }
    }
    return ret;
}

struct projectile_cycle_data {
    std::vector<std::pair<uint32_t, bool>> projectiles_to_erase;
    std::vector<uint32_t>                  enemies_to_erase;
    struct e {
        enemy_t first;
        double second;
        line_strip_t* third;
        double fourth;
        projectile& fifth;
    };
    std::vector<e>                         enemies_to_add;
    double                                 extra_cash { 0.0 };
};

uint32_t queue_spawns(
    projectile_cycle_data& data,
    double excess_damage,
    enemy_t e,
    uint16_t flags,
    line_strip_t* p,
    projectile& pj,
    uint32_t id,
    double dist
) {
    uint32_t ret = id;

    data.extra_cash += e.base_kill_reward
                    *  gs.diff.enemy_kill_reward_modifier
                    *  gs.diff.round_set.r[gs.cur_round].kill_cash_multiplier;

    // todo: enemy spawn multiplier

    for(auto i : iterate(e.spawns.size())) {
        enemy_t s = enemy_types[*(e.spawns.begin() + i)];
        double health = s.base_health
                      * gs.diff.enemy_health_modifier
                      * gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier;

        if(health <= excess_damage) ret += queue_spawns(data, excess_damage - health, s, flags, p, pj, ret, dist);
        else {
            uint8_t random_flags = E_FLAG_NONE;

            if(gs.diff.enemy_random_stealth_odds) if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds
            *   gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                random_flags |= E_FLAG_STEALTH;
            if(gs.diff.enemy_random_armored_odds) if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds
            *   gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                random_flags |= E_FLAG_ARMORED;
            if(gs.diff.enemy_random_shield_odds) if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds
            *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                random_flags |= E_FLAG_SHIELD;
            
            s.flags |= (uint8_t)(flags | random_flags);
            data.enemies_to_add.push_back({ s, excess_damage, p, dist - ret * 16, pj });
            ret++;
        }
    }

    return ret;
}

void do_damage(projectile_cycle_data& data, projectile& p, enemy* e) {
    std::lock_guard l { e->lock };
    if(e->health <= 0.0)                                                                   return;

    for(auto& hit : p.hits) if(hit == e->id)                                               return;

    if(e->flags & E_FLAG_STEALTH && ~p.flags & P_FLAG_STEALTH_TAR)                         return;
    if(e->flags & E_FLAG_ARMORED && ~p.flags & P_FLAG_ARMORED_TAR) { p.remaining_hits = 0; return; }

    p.remaining_hits--;

    if(!(p.damage_type & ~e->immunities))                                                  return;

    double dmg = p.damage;
    if(p.damage_type & e->vulnerabilities) dmg *= 2;

    if( p.flags & P_FLAG_STRIP_ARMOR)   e->flags &= ~E_FLAG_ARMORED;
    if( p.flags & P_FLAG_STRIP_STEALTH) e->flags &= ~E_FLAG_STEALTH;
    if(e->flags & E_FLAG_SHIELD)      { e->flags &= ~E_FLAG_SHIELD; return; }
    
    e->health -= dmg;
    if(e->health <= 0.0) {
        data.enemies_to_erase.push_back(e->id);
        queue_spawns(data, abs(e->health), enemy_types[e->base_type], e->flags, e->route, p, 0, e->distance_traveled);
    }
}

void detonate (projectile_cycle_data& data, projectile& p) {
    vertex_2d pos = p.start + p.direction_vector * p.travelled;

    for(auto& e : *p.enemies) {
        if(!p.remaining_range_hits) return;

        bool c = false;
        for(auto& id : data.enemies_to_erase)
            if(e->id == id) { c = true; break; }
        if(c) continue;

        vec2 epos = e->route->get_position_at(e->distance_traveled);
        double dx = epos.x - pos.x;
        double dy = epos.y - pos.y;
        double d = sqrt(dx * dx + dy * dy);

        if(d <= p.damage_range) {
            std::lock_guard l { e->lock };

            if(e->health <= 0.0)                                           continue;

            if(e->flags & E_FLAG_STEALTH && ~p.flags & P_FLAG_STEALTH_TAR) continue;
            if(e->flags & E_FLAG_ARMORED && ~p.flags & P_FLAG_ARMORED_TAR) continue;

            if(!(p.damage_type & ~e->immunities))                          continue;

            p.remaining_range_hits--;

            double dmg = p.damage;
            if(p.damage_type & e->vulnerabilities) dmg *= 2;

            if( p.flags & P_FLAG_STRIP_ARMOR)   e->flags &= ~E_FLAG_ARMORED;
            if( p.flags & P_FLAG_STRIP_STEALTH) e->flags &= ~E_FLAG_STEALTH;
            if(e->flags & E_FLAG_SHIELD)      { e->flags &= ~E_FLAG_SHIELD; return; }

            e->health -= dmg;

            if(e->health <= 0.0) {
                data.enemies_to_erase.push_back(e->id);
                queue_spawns(data, abs(e->health), enemy_types[e->base_type], e->flags, e->route, p, 0, e->distance_traveled);
            }
        }
    }
}

projectile_cycle_data projectile_cycle(double dt, size_t i) {
    projectile_cycle_data ret;
    for(auto& p : iterate(gs.projectiles, serverthreads, i)) {
        p.travelled += p.speed * dt;

        if(p.travelled > p.range) {
            ret.projectiles_to_erase.push_back(std::make_pair(p.pid, false));
            continue;
        }

        vertex_2d pos = p.start + p.direction_vector * p.travelled;
        for(auto& e : gs.created_enemies) {
            bool previously_hit = false;
            for(auto& ptr : p.hits)
                if(ptr == e.id) {
                    previously_hit = true;
                    break;
                }
            if(previously_hit) continue;

            const auto& base = enemy_types[e.base_type];
            double w = base.scale * 512.0;
            double h = base.scale * 512.0;

            vec2 epos = e.route->get_position_at(e.distance_traveled);
            base_rect_t bounding_box { {
                { epos + vec2 { -w * 0.55, -h * 0.375 } },
                { epos + vec2 {  w * 0.45, -h * 0.375 } },
                { epos + vec2 {  w * 0.45,  h * 0.625 } },
                { epos + vec2 { -w * 0.55,  h * 0.625 } }
            } };

            if(bounding_box.contains(pos, 4.0)) {
                do_damage(ret, p, &e);

                if(!p.remaining_hits) {
                    detonate(ret, p);
                    ret.projectiles_to_erase.push_back(std::make_pair(p.pid, true));
                    break;
                }
            }
        }
    }
    return ret;
}

uint32_t get_route_id(enemy& e) {
    for(uint32_t i = 0; i < (uint32_t)current_map->paths.size(); i++)
        if(e.route == &current_map->paths[i])
            return i;
    return -1;
}

clientinfo* get_client(uint32_t cn) {
    for(auto& cl : clientinfos)
        if(cl.cn == cn)
            return &cl;
    return nullptr;
}

void servertick() {
    double dt = std::chrono::duration<double>(sc::now() - gs.last_tick).count() * (gs.double_speed ? 2.0 : 1.0);
    gs.last_tick = sc::now();

    if(gs.lives > 0 && gs.running && !gs.paused) {
        packetstream broadcast { };

        if(!gs.done_spawning) {
            size_t pos = 0;
            gs.time_elapsed += dt;
            
            for(auto& set : gs.diff.round_set.r[gs.cur_round].enemies) {
                size_t n_set = (size_t)(set.amount * gs.diff.enemy_amount_modifier);
                if(gs.spawned_enemies >= pos + n_set) { pos += n_set; continue; }
                size_t n_left = pos + n_set - gs.spawned_enemies;
                size_t n_spawn = (size_t)(gs.time_elapsed * 25.0 / set.spacing);
                if(n_spawn == 0) break;
                if(n_left > 0) {
                    for(auto i : iterate(min(n_spawn, n_left))) {
                        gs.last_route = limit(0, gs.last_route + 1, current_map->paths.size() - 1);

                        uint8_t random_flags = E_FLAG_NONE;

                        if(gs.diff.enemy_random_stealth_odds) if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_stealth_odds
                                                 *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                            random_flags |= E_FLAG_STEALTH;
                        if(gs.diff.enemy_random_armored_odds) if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_armored_odds
                                                 *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                            random_flags |= E_FLAG_ARMORED;
                        if(gs.diff.enemy_random_shield_odds) if((rng() % (size_t)(1.0 / (gs.diff.enemy_random_shield_odds
                                                 *  gs.diff.round_set.r[gs.cur_round].special_odds_multiplier))) == 0)
                            random_flags |= E_FLAG_SHIELD;

                        gs.created_enemies.push_back({
                            /* base_type:          */ set.base_type,
                            /* route:              */ &current_map->paths[gs.last_route],
                            /* distance_traveled: */ 0.0,
                            /* pos:                */ current_map->paths[gs.last_route][0],
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
                            /* frozen_for:         */ 0.0,
                            /* id:                 */ gs.all_spawned_enemies
                        });

                        broadcast << N_SPAWN_ENEMY
                                  << 38_u32
                                  << set.base_type
                                  << (uint32_t)gs.last_route
                                  << enemy_types[set.base_type].base_health
                                  *  gs.diff.enemy_health_modifier
                                  *  gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier
                                  << enemy_types[set.base_type].base_speed
                                  *  gs.diff.enemy_speed_modifier
                                  *  gs.diff.round_set.r[gs.cur_round].enemy_speed_multiplier
                                  << (uint16_t)(enemy_types[set.base_type].immunities | gs.diff.enemy_base_immunities)
                                  << enemy_types[set.base_type].vulnerabilities
                                  << (uint08_t)(enemy_types[set.base_type].flags | set.flags | random_flags)
                                  << 0.0
                                  << gs.all_spawned_enemies++;

                        gs.spawned_enemies++;
                    }

                    update_targeting_priorities();
                    gs.time_elapsed = 0.0;
                    break;
                }
            }
        }

        std::vector<std::future<std::vector<enemy*>>> ecycles;
        for(auto  i : iterate(serverthreads)) ecycles.push_back(std::async(enemy_cycle, dt, i));
        for(auto& T : ecycles) T.wait();
        for(auto& T : ecycles)
            for(const auto& ptr : T.get())
                for(size_t i = 0; i < gs.created_enemies.size(); i++)
                    if(&gs.created_enemies[i] == ptr) {
                        broadcast << N_ENEMY_SURVIVED
                                  << 4_u32
                                  << gs.created_enemies[i].id
                                  << N_UPDATE_LIVES
                                  << 8_u32
                                  << gs.lives;

                        gs.created_enemies.erase(gs.created_enemies.begin() + i);
                        i--;
                        break;
                    }

        std::vector<std::future<std::vector<projectile>>> tcycles;
        for(auto  i : iterate(serverthreads)) tcycles.push_back(std::async(tower_cycle, dt, i));
        for(auto& T : tcycles) T.wait();

        std::vector<std::future<projectile_cycle_data>> pcycles;
        for(auto  i : iterate(serverthreads)) pcycles.push_back(std::async(projectile_cycle, dt, i));
        for(auto& T : pcycles) T.wait();

        bool update = false;

        for(auto& T : pcycles) {
            const auto& data = T.get();

            for(auto& id : data.enemies_to_erase)
                for(size_t i = 0; i < gs.created_enemies.size(); i++)
                    if(gs.created_enemies[i].id == id) {
                        broadcast << N_KILL_ENEMY
                                  << 4_u32
                                  << gs.created_enemies[i].id;

                        gs.created_enemies.erase(gs.created_enemies.begin() + i);
                        break;
                    }

            for(auto& s : data.enemies_to_add) {
                auto& e = s.first;
                gs.created_enemies.push_back({
                    /* base_type:          */ e.base_type,
                    /* route:              */ s.third,
                    /* distance_traveled:  */ s.fourth,
                    /* pos:                */ (*s.third)[0],
                    /* max_health:         */ e.base_health
                                           *  gs.diff.enemy_health_modifier
                                           *  gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier,
                    /* health:             */ e.base_health
                                           *  gs.diff.enemy_health_modifier
                                           *  gs.diff.round_set.r[gs.cur_round].enemy_health_multiplier
                                           -  s.second,
                    /* speed:              */ e.base_speed
                                           *  gs.diff.enemy_speed_modifier
                                           *  gs.diff.round_set.r[gs.cur_round].enemy_speed_multiplier,
                    /* kill_reward:        */ e.base_kill_reward
                                           *  gs.diff.enemy_kill_reward_modifier
                                           *  gs.diff.round_set.r[gs.cur_round].kill_cash_multiplier,
                    /* immunities:         */ (uint16_t)(e.immunities | gs.diff.enemy_base_immunities),
                    /* vulnerabilities:    */ e.vulnerabilities,
                    /* flags:              */ e.flags,
                    /* slowed_for:         */ 0.0,
                    /* frozen_for:         */ 0.0,
                    /* id:                 */ gs.all_spawned_enemies
                });

                s.fifth.hits.push_back(gs.all_spawned_enemies);

                broadcast << N_SPAWN_ENEMY
                          << 38_u32
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].base_type
                          << get_route_id(gs.created_enemies[gs.created_enemies.size() - 1])
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].max_health
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].speed
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].immunities
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].vulnerabilities
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].flags
                          <<              gs.created_enemies[gs.created_enemies.size() - 1].distance_traveled
                          <<              gs.all_spawned_enemies++;
            }

            
            for(auto& id : data.projectiles_to_erase)
                for(size_t i = 0; i < gs.projectiles.size(); i++)
                    if(gs.projectiles[i].pid == id.first) {
                        if(id.second)
                            broadcast << N_DETONATE
                                      << 4_u32
                                      << gs.projectiles[i].pid;

                            broadcast << N_DELETE_PROJECTILE
                                      << 4_u32
                                      << gs.projectiles[i].pid;

                        gs.projectiles.erase(gs.projectiles.begin() + i);
                        break;
                    }

            if(data.enemies_to_add.size() || data.enemies_to_erase.size())
                update = true;

            if(data.extra_cash)
                for(auto& c : clientinfos)
                    broadcast << N_UPDATE_CASH
                              << 12_u32
                              << c.id
                              << (c.cash += data.extra_cash / (double)maxclients);
        }

        if(update)
            update_targeting_priorities();

        for(auto& T : tcycles)
            for(auto& p : T.get()) {
                p.pid = gs.spawned_projectiles;
                gs.spawned_projectiles++;
                gs.projectiles.push_back(p);
                broadcast << N_PROJECTILE
                          << (uint32_t)(projectile_size + p.path.length())
                          << p.path;
                broadcast.write(((const char*)&p) + offsetof(projectile, id), projectile_size);
            }

        size_t total_enemies = 0;
        for(auto& set : gs.diff.round_set.r[gs.cur_round].enemies)
            total_enemies += (size_t)(set.amount * gs.diff.enemy_amount_modifier);

        if(gs.created_enemies.size() == 0 && gs.spawned_enemies == total_enemies) {
            gs.running = false;
            gs.cur_round++;

            double cash = 100.0 + gs.last_round;
            for(auto& c : clientinfos)
                broadcast << N_UPDATE_CASH
                          << 12_u32
                          << c.id
                          << (c.cash += cash / (double)maxclients);

            broadcast << N_ROUNDOVER
                      << 0_u32;

            gs.spawned_enemies = 0;
            if(gs.cur_round == gs.diff.rounds_to_win) {
                // todo: win screen
            }
        }

        send_packet(nullptr, 0, true, broadcast);
    }
}

std::string update_entities() {
    packetstream p;

    p << N_UPDATE_ENTITIES
      << 0_u32;

    for(auto& e : gs.created_enemies)
        p << N_SPAWN_ENEMY
          << 30_u32
          << e.base_type
          << get_route_id(e)
          << e.max_health
          << e.speed
          << e.immunities
          << e.vulnerabilities
          << e.flags
          << e.id;

    for(auto& pj : gs.projectiles) {
        p << N_PROJECTILE
          << projectile_size;
        p.write((const char*)&pj + offsetof(projectile, id), projectile_size);
    }

    for(auto& t : gs.towers) {
        p << N_PLACETOWER
          << 33_u32
          << t.cid
          << t.id
          << t.base_type
          << t.pos_x
          << t.pos_y
          << t.cost
          << N_UPGRADETOWER
          << 15_u32
          << t.id
          << t.upgrade_paths[0]
          << t.upgrade_paths[1]
          << t.upgrade_paths[2]
          << 0.0
          << N_UPDATETARGETING
          << 5_u32
          << t.id
          << t.targeting_mode;
    }

    std::string data { };
    p.read(data, p.size());

    return data;
}

result<void, int> handle_packets(packetstream packet, ENetPeer* peer) {
    if(!packet.size()) return false;
    if((size_t)peer->data == -1) {
        uint32_t packet_type = NUMMSG;
        packet >> packet_type;
        if(packet_type == N_CONNECT) {
            if(clients.size() == maxclients)
                return DISC_SERVER_FULL;

            uint32_t size = 0;
            packet >> size;

            client_t c;
            c.peer = peer;
            c.cn = 0;
            packet.read(c.name, size);

_loop:
            for(auto& cl : clients)
                if(cl.cn == c.cn) {
                    c.cn++;
                    goto _loop;
                }

            clients.push_back(c);

            clientinfo ci;
            ci.cn = c.cn;
            ci.id = (uint32_t)clientinfos.size();
            ci.cash = 950.0 * gs.diff.start_cash_modifier / (double)maxclients + 25.0 * (double)(maxclients - 1);

            clientinfos.push_back(ci);
            clients[clients.size() - 1].data = (void*)&clientinfos[clientinfos.size() - 1];

            conout(c.name + " connected (" + std::to_string(ci.cn) + ')');
            peer->data = (void*)(size_t)c.cn;

            packetstream reply { };
            reply << N_SENDMAP
                  << (uint32_t)current_map->data.length()
                  << current_map->data;

            reply << N_GAMEINFO
                  << 27_u32
                  << ci.id
                  << (uint32_t)gs.cur_round
                  << (uint32_t)gs.last_round
                  << gs.lives
                  << gs.double_speed
                  << gs.paused
                  << gs.running
                  << gs.diff.id;

            for(auto& c : clientinfos)
                reply << N_PLAYERINFO
                      << (uint32_t)(12_u32 + get_raw_client(c.cn)->name.length())
                      << c.id
                      << c.cash
                      << get_raw_client(c.cn)->name;

            reply << update_entities();

            send_packet(peer, 0, true, reply);

            if(clients.size() != 1) {
                packetstream p;
                p << N_PLAYERINFO
                  << (uint32_t)(12_u32 + get_raw_client(c.cn)->name.length())
                  << ci.id
                  << ci.cash
                  << get_raw_client(c.cn)->name;
            
                for(size_t i = 0; i < clientinfos.size() - 1; i++)
                    send_packet(nullptr, 0, true, p);
            }

            return { };
        }

        return DISC_MSGERR;
    }

    auto cn = (size_t)peer->data;
    auto client = get_client((uint32_t)cn);
    if(!client) return { };
    client->last_message = sc::now();

    packetstream reply { };
    packetstream broadcast { };

    while(true) {
        uint32_t packet_type = NUMMSG;
        packet >> packet_type;

        if(!packet) break;

        uint32_t size = 0;
        packet >> size;

        if(msgsizes[packet_type] != -1 && msgsizes[packet_type] != size) return DISC_MSGERR;

        switch(packet_type) {
            case N_CONNECT:         return DISC_MSGERR;
            case N_TEXT: {
                char* s = new char[size];
                packet.read(s, size);
                broadcast << N_TEXT
                          << size + 4_u32
                          << client->id;
                broadcast.write(s, size);
                delete[] s;
                break;
            }

            case N_PLACETOWER: {
                double x { 0.0 }, y { 0.0 };
                uint8_t base_type { NUMTOWERS };
                packet >> x
                       >> y
                       >> base_type;
                
                // todo: discounts?

                if(base_type >= NUMTOWERS) break;
                double cost = tower_types[base_type].base_price * gs.diff.tower_cost_modifier;
                if(cost > client->cash) break;
                if(!can_place(gs.towers, tower_types[base_type], x, y)) break;

                tower t { tower_types[base_type], cost, x, y };
                t.cid = (uint32_t)cn;
                t.id = gs.spawned_towers++;
                gs.towers.push_back(t);
                client->towers.push_back(t.id);
                client->cash -= cost;

                broadcast << N_PLACETOWER
                          << 33_u32
                          << client->id
                          << t.id
                          << t.base_type
                          << x
                          << y
                          << cost
                          << N_UPDATE_CASH
                          << 12_u32
                          << client->id
                          << client->cash;

                break;
            }

            case N_UPGRADETOWER: {
                uint32_t tid { 0 };
                uint8_t path { 0 };
                packet >> tid
                       >> path;
                tower* t = nullptr;

                if(path > 2) break;

                bool owned = false;

                for(auto& id : client->towers)
                    if(id == tid) {
                        owned = true;
                        break;
                    }

                if(!owned) break;

                for(auto& twr : gs.towers)
                    if(twr.id == tid) {
                        t = &twr;
                        break;
                    }

                if(!t) break; 
                uint8_t upgrade_count = t->upgrade_paths[0] + t->upgrade_paths[1] + t->upgrade_paths[2];
                if(t->upgrade_paths[path] == 6 || upgrade_count == 8) break;

                upgrade& up = tower_types[t->base_type].upgrade_paths[path][t->upgrade_paths[path]];
                double cost = up.base_price * gs.diff.tower_cost_modifier;

                if(cost > client->cash) break;
                t->try_upgrade(path, cost);
                client->cash -= cost;

                broadcast << N_UPGRADETOWER
                          << 15_u32
                          << t->id
                          << t->upgrade_paths[0]
                          << t->upgrade_paths[1]
                          << t->upgrade_paths[2]
                          << cost
                          << N_UPDATE_CASH
                          << 12_u32
                          << client->id
                          << client->cash;

                reply << N_REFRESHMENU
                      << 0_u32;

                break;
            }
            case N_UPDATETARGETING: {
                uint32_t tid { 0 };
                uint8_t targeting { 0 };
                packet >> tid
                       >> targeting;
                tower* t = nullptr;

                if(targeting > NUMTARGETINGS) break;

                bool owned = false;

                for(auto& id : client->towers)
                    if(id == tid) {
                        owned = true;
                        break;
                    }

                if(!owned) break;

                for(auto& twr : gs.towers)
                    if(twr.id == tid) {
                        t = &twr;
                        break;
                    }

                if(!t) break; 
                t->targeting_mode = targeting;

                broadcast << N_UPDATETARGETING
                          << 5_u32
                          << t->id
                          << t->targeting_mode;

                reply << N_REFRESHMENU
                      << 0_u32;
                break;
            }
            case N_SELLTOWER:       break;                            // todo

            case N_UPDATE_CASH:
            case N_UPDATE_LIVES:    return DISC_MSGERR;

            case N_REQUEST_UPDATE: {
                for(auto& ci : clientinfos) {
                    reply << N_UPDATE_CASH
                          << 12_u32
                          << ci.id
                          << ci.cash;
                    }

                reply << N_UPDATE_LIVES
                      << 8_u32
                      << gs.lives
                      << update_entities();
                break;
            }

            case N_UPDATE_ENTITIES: return DISC_MSGERR;

            case N_PING:            reply << N_PONG << 0_u32; break; // todo: send all connected clients' ping in response

            case N_PONG:
            case N_ROUNDINFO:       return DISC_MSGERR;

            case N_STARTROUND: {
                if(!gs.running && gs.last_round != gs.cur_round) {
                    init_round();
                    broadcast << N_STARTROUND << 0_u32 << N_ROUNDINFO << 4_u32 << gs.cur_round;
                }
                break;
            }
            case N_SET_SPEED:       packet >> gs.double_speed; broadcast << N_SET_SPEED << 1_u32 << gs.double_speed; break;
            case N_PAUSE:           if(!gs.paused) { gs.paused =  true; broadcast << N_PAUSE  << 0_u32; } break;
            case N_RESUME:          if( gs.paused) { gs.paused = false; broadcast << N_RESUME << 0_u32; } break;

            case N_ROUNDOVER:
            case N_GAMEOVER:        return DISC_MSGERR;

            case N_RESTART:         if(gs.lives <= 0) { init(); broadcast << N_RESTART << 0_u32; } break;
            case N_CONTINUE:        break;                            // todo

            case N_DISCONNECT:      return DISC_OK;

            case N_SPAWN_ENEMY:
            case N_PROJECTILE:
            case N_DETONATE:
            case N_ENEMY_SURVIVED:
            case N_KILL_ENEMY:
            case N_SENDMAP:
            case N_GAMEINFO:
            case N_PLAYERINFO:
            case N_REFRESHMENU:     return DISC_MSGERR;

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

    if(    reply.size()) send_packet(peer,     0, true, reply);
    if(broadcast.size()) send_packet(nullptr,  0, true, broadcast);

    return { };
}
