#include "Game.hpp"
#include "Difficulty.hpp"
#include "Tower.hpp"

#include "../Engine/Engine.hpp"
#include "../Engine/Map.hpp"

#include <thread>
#include <random>

using sc = std::chrono::system_clock;

extern void init_towers();

std::mt19937_64 rng;

void init_game() {
    rng.seed(std::random_device{}());
    init_towers();
}

void render_sidebar() {

}

void render_menu() {

}

bvarp(encountered_shield, false);
bvarp(encountered_armor, false);
bvarp(encountered_stealth, false);

ivarp(threads, 2, std::thread::hardware_concurrency() - 1, INTMAX_MAX);

bool* threads_done = nullptr; // Could be atomic but no need as this is only ever written to by one thread and read by all others
bool double_speed = false;
auto last_spawned_tick = sc::now();
size_t cur_round = 0;
size_t last_route = 0;
difficulty diff;

std::vector<spawned_enemy>  spawned_enemies;
std::vector<owned_tower>    owned_towers;

void game_tick() {
    threads_done = new bool[threads](false);

    std::vector<std::thread> Ts;

    // Spawning can't really be done multithreaded or it would cause data races. Mutexes are an option but it's slower than just doing it in one thread
    size_t pos = 0;
    // todo: Only works until round 150 for now
    for(auto& set : diff.round_set->r[cur_round].e) {
        size_t n_set = (size_t)(set.amount * diff.enemy_amount_modifier);
        if(spawned_enemies.size() > pos + n_set) { pos += n_set; continue; }
        size_t n_left = pos + n_set - spawned_enemies.size();
        size_t n_spawn = (size_t)((sc::now() - last_spawned_tick).count() / set.spacing / (double_speed ? 2.0 : 1.0));
        if(n_spawn > 0) {
            last_spawned_tick = sc::now();
            for(auto i : iterate(min(n_spawn, n_left))) {
                last_route = limit(0, last_route + 1, current_map->paths.size() - 1);
                spawned_enemies.push_back(spawned_enemy {
                    &current_map->paths[last_route],
                    current_map->paths[last_route][0],
                    set.e->base_health * diff.enemy_health_modifier * diff.round_set->r[cur_round].enemy_health_multiplier,
                    set.e->base_health * diff.enemy_health_modifier * diff.round_set->r[cur_round].enemy_health_multiplier,
                    set.e->base_speed * diff.enemy_speed_modifier * diff.round_set->r[cur_round].enemy_speed_multiplier,
                    set.e->base_kill_reward * diff.enemy_kill_reward_modifier * diff.round_set->r[cur_round].kill_cash_multiplier,
                    set.e->scale,
                    (uint16_t)(set.e->immunities | diff.enemy_base_immunities), // Why is the explicit conversion required here?
                    set.e->vulnerabilities,
                    set.e->stealth || (rng() % (size_t)(1.0 / (diff.enemy_random_stealth_odds * diff.round_set->r[cur_round].special_odds_multiplier))) == 0,
                    set.e->armored || (rng() % (size_t)(1.0 / (diff.enemy_random_armored_odds * diff.round_set->r[cur_round].special_odds_multiplier))) == 0,
                    (rng() % (size_t)(1.0 / (diff.enemy_random_shield_odds * diff.round_set->r[cur_round].special_odds_multiplier))) == 0,
                    set.e->texture,
                    set.e->spawns_when_damaged,
                    set.e->spawns
                });
            }
            break;
        }
    }

    for(auto i : iterate(threads)) Ts.push_back(std::thread([](size_t i) {
        // do game update split into threads
        // already know how ill do this
        // im just lazy rn ill do it later
    }, i));

    for(auto& enemy : spawned_enemies) if(enemy.health > 0.0) {
        // render the enemy
    }

    for(auto& T : reverse(Ts)) {
        T.join();
        Ts.pop_back();
    }

    delete[] threads_done;
}
