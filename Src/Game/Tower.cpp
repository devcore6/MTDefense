#include <filesystem>
#include <random>

#include "../Engine/Engine.hpp"
#include "Game.hpp"

std::vector<tower> towers;

extern std::mt19937_64 rng;

tower::tower(tower_t* t, double c, double x, double y) : base_type(t->tower_type), cost(c), pos_x(x), pos_y(y), projectiles(t->projectiles) {
    rect.vertices[0] = { -t->hitbox_radius, -t->hitbox_radius };
    rect.vertices[1] = {  t->hitbox_radius, -t->hitbox_radius };
    rect.vertices[2] = {  t->hitbox_radius,  t->hitbox_radius };
    rect.vertices[3] = { -t->hitbox_radius,  t->hitbox_radius };
}

void tower::tick(double time) {
    remaining_cooldown = max(remaining_cooldown - time, 0.0);
}

bool tower::can_fire() { return remaining_cooldown <= 0.0; }

result<projectile, void> tower::fire(enemy* e) {
    // if(!can_fire()) return { };

    double dx = e->pos.x - pos_x;
    double dy = e->pos.y - pos_y;
    double d = sqrt(dx * dx + dy * dy);

    if(d > tower_types[base_type].range * range_mod) return { };
    
    size_t odds = 0;
    for(auto& p : projectiles)
        if((~e->flags & E_FLAG_STEALTH || (flags | p.flags) & T_FLAG_STEALTH_TAR)
        && (~e->flags & E_FLAG_ARMORED || (flags | p.flags) & T_FLAG_ARMORED_TAR))
            odds += p.odds;

    if(odds == 0) return { }; // No projectile can target this enemy

    size_t r = rng() % odds;
    size_t offset = 0;
    projectile_t* pt = nullptr;

    for(auto i : iterate(projectiles.size())) {
        if((~e->flags & E_FLAG_STEALTH || (flags | projectiles[i].flags) & T_FLAG_STEALTH_TAR)
        && (~e->flags & E_FLAG_ARMORED || (flags | projectiles[i].flags) & T_FLAG_ARMORED_TAR)) {
            offset += projectiles[i].odds;
            continue;
        }

        if(projectiles[i].odds + offset >= r) { pt = &projectiles[i]; break; }
        offset += projectiles[i].odds;
    }

    if(!pt) return { }; // Should never happen

    remaining_cooldown = 1.0 / (pt->fire_rate * fire_rate_mod);

    return projectile {
        /* texture:              */ pt->texture,
        /* id:                   */ pt->id,
        /* start:                */ { pos_x, pos_y },
        /* direction_vector:     */ (vertex_2d { pos_x, pos_y } - e->pos).normalize(),
        /* travelled:            */ 0.0,
        /* range:                */ pt->range * range_mod * gs.diff.tower_range_modifier + extra_damage_linear,
        /* remaining_lifetime:   */ pt->lifetime,
        /* remaining_hits:       */ pt->damage_maxhits + extra_damage_maxhits_linear,
        /* remaining_range_hits: */ pt->range_maxhits + extra_damage_maxhits_range,
        /* damage:               */ pt->base_damage * damage_mod,
        /* damage_range:         */ pt->damage_range + extra_damage_range,
        /* damage_type:          */ (uint16_t)(pt->damage_type | extra_damage_types),
        /* flags:                */ (uint08_t)(flags | pt->flags),
        /* armor_mod:            */ max(pt->armor_modifier, armor_mod)
    };
}

#ifndef __SERVER__
void tower::render() {
    std::string upgrade_path = std::to_string(upgrade_paths[0]) + '-'
                             + std::to_string(upgrade_paths[1]) + '-'
                             + std::to_string(upgrade_paths[2]);
    rect.anim = tower_types[base_type].animations[upgrade_path];
    rect.render({ pos_x, pos_y }, rot);
}
#endif

void tower::try_upgrade(uint8_t path, double price) {
    if(path > 2 || upgrade_paths[path] == 5) return;
    cost += price;
    const upgrade* u = &tower_types[base_type].upgrade_paths[path][upgrade_paths[path]];
    range_mod                   *= u->range_mod;
    fire_rate_mod               *= u->fire_rate_mod;
    speed_mod                   *= u->speed_mod;
    armor_mod                   *= u->armor_mod;
    damage_mod                  *= u->damage_mod;
    flags                       |= u->flags;
    extra_damage_maxhits_linear += u->extra_damage_maxhits_linear;
    extra_damage_maxhits_range  += u->extra_damage_maxhits_range;
    extra_damage_linear         += u->extra_damage_linear;
    extra_damage_range          += u->extra_damage_range;
    extra_damage_types          += u->extra_damage_types;
    upgrade_paths[path]++;
}
