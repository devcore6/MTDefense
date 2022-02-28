#include <filesystem>
#include <random>

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

double tower::fire(enemy* e) {
    if(!can_fire()) return -1.0;
    double dx = e->pos.x - pos_x;
    double dy = e->pos.y - pos_y;
    double d  = sqrt(dx * dx + dy * dy);
    if(d > tower_types[base_type].range * range_mod) return -1.0;

    size_t odds = 0;
    for(auto& p : projectiles)
        if((~e->flags & E_FLAG_STEALTH || flags & T_FLAG_STEALTH_TAR || p.flags & P_FLAG_STEALTH_TAR)
        && (~e->flags & E_FLAG_ARMORED || flags & T_FLAG_ARMORED_TAR || p.flags & P_FLAG_ARMORED_TAR))
            odds += p.odds;
    if(odds == 0) return -1.0;

    size_t r = rng() % odds;
    size_t offset = 0;
    projectile_t* p = nullptr;
    for(auto i : iterate(projectiles.size())) {
        if((~e->flags & E_FLAG_STEALTH || flags & T_FLAG_STEALTH_TAR || projectiles[i].flags & P_FLAG_STEALTH_TAR)
            && (~e->flags & E_FLAG_ARMORED || flags & T_FLAG_ARMORED_TAR || projectiles[i].flags & P_FLAG_ARMORED_TAR)) {
            offset += projectiles[i].odds;
            continue;
        }

        if(projectiles[i].odds + offset >= r) { p = &projectiles[i]; break; }
        offset += projectiles[i].odds;
    }

    if(!p) return -1.0; // Should never happen
    last_projectile = p;
    uint16_t damage_type = p->damage_type | extra_damage_types;

    if(damage_type & ~e->immunities) {
        double dmg = p->base_damage * damage_mod;
        if(e->flags & E_FLAG_ARMORED)
            dmg *= armor_mod;
        if(damage_type & e->vulnerabilities)
            dmg *= 2;
        remaining_cooldown = 1.0 / (p->fire_rate * fire_rate_mod);
        return dmg;
    }
    return 0.0;
}

void tower::render() {
    std::string upgrade_path = std::to_string(upgrade_paths[0]) + '-'
                             + std::to_string(upgrade_paths[1]) + '-'
                             + std::to_string(upgrade_paths[2]);
    rect.anim = tower_types[base_type].animations[upgrade_path];
    rect.render({ pos_x, pos_y }, rot);
}

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
