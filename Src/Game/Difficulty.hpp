#pragma once
#include "Enemy.hpp"

struct enemy_set {
    enemy**         e;
    double          spacing                     = 0.0;
    double          amount                      = 0.0;

    bool            stealth                     = false;
    bool            armored                     = false;
    bool            shield                      = false;

    uint16_t        immunities                  = 0;
    uint16_t        vulnerabilities             = 0;
};

struct round_t {
    std::vector<enemy_set> e;

    double          kill_cash_multiplier        = 1.0;
    double          special_odds_multiplier     = 1.0;
    double          shield_strength_multiplier  = 1.0;
    double          enemy_speed_multiplier      = 1.0;
    double          enemy_health_multiplier     = 1.0;
};

struct rounds {
    round_t         r[150];

    double          late_game_enemy_amount_multiplier   = 1.50;
    double          late_game_multiplier_multiplier     = 1.25;
};

extern const rounds standard_rounds;

struct difficulty {
    size_t          rounds_to_win               = 60;
    size_t          lives                       = 250;

    double          enemy_speed_modifier        = 1.00;
    double          enemy_health_modifier       = 1.00;
    double          enemy_spawns_modifier       = 1.00;
    double          enemy_amount_modifier       = 1.00;
    double          enemy_random_stealth_odds   = 0.00;
    double          enemy_random_armored_odds   = 0.00;
    double          enemy_random_shield_odds    = 0.00;
    double          enemy_shield_strength       = 0.00;
    uint16_t        enemy_base_immunities       = 0;

    double          enemy_kill_reward_modifier  = 1.00;
    double          start_cash_modifier         = 1.00;
    double          cash_production_modifier    = 1.00;

    double          tower_cost_modifier         = 1.00;
    double          tower_range_modifier        = 1.00;
    double          tower_fire_rate_modifier    = 1.00;
    double          tower_sale_price_factor     = 0.85;

    double          projectile_speed_modifier   = 1.00;

    bool            knowledge_allowed           = true;
    bool            powers_allowed              = true;
    bool            continues_allowed           = true;

    double          reward_modifier             = 1.0;

    const rounds*   round_set                   = &standard_rounds;
};

constexpr difficulty easy       = {  40, 500, 0.9, 0.9, 0.9, 0.8, 0.00000, 0.00000, 0.00000, 1.0, 0, 1.20, 1.20, 1.20, 0.8, 1.10, 1.10, 0.90, 1.20, true, true, true, 1.0 };
constexpr difficulty medium     = {  60, 250, 1.0, 1.0, 1.0, 1.0, 0.00000, 0.00000, 0.00000, 1.5, 0, 1.00, 1.00, 1.00, 1.0, 1.00, 1.00, 0.85, 1.00, true, true, true, 2.0 };
constexpr difficulty hard       = {  80, 100, 1.2, 1.1, 1.3, 1.4, 0.00002, 0.00001, 0.00001, 2.0, 0, 0.92, 0.90, 0.95, 1.2, 0.96, 0.95, 0.80, 0.98, true, true, true, 3.0 };
constexpr difficulty impossible = { 100,   1, 1.3, 1.2, 1.4, 1.6, 0.00003, 0.00002, 0.00005, 3.0, 0, 0.82, 0.85, 0.88, 1.3, 0.94, 0.92, 0.75, 0.95, true, true, true, 4.0 };
