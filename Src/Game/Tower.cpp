#include <filesystem>
#include <random>

#include "../Engine/Engine.hpp"
#include "Game.hpp"

std::vector<tower> towers;

extern std::mt19937_64 rng;

tower::tower(tower_t& t, double c, double x, double y) : base_type(t.tower_type), cost(c), pos_x(x), pos_y(y), projectiles(t.projectiles) {
    rect.vertices[0] = { -t.hitbox_radius, -t.hitbox_radius };
    rect.vertices[1] = {  t.hitbox_radius, -t.hitbox_radius };
    rect.vertices[2] = {  t.hitbox_radius,  t.hitbox_radius };
    rect.vertices[3] = { -t.hitbox_radius,  t.hitbox_radius };
}

void tower::tick(double time) {
    remaining_cooldown = max(remaining_cooldown - time, 0.0);
}

bool tower::can_fire() { return remaining_cooldown <= 0.0; }

result<projectile, void> tower::fire(enemy* e, std::vector<enemy*>* targets) {
    // if(!can_fire()) return { };

    vec2 epos = e->route->get_position_at(e->distance_traveled);
    double dx = epos.x - pos_x;
    double dy = epos.y - pos_y;
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

    for(auto& p : projectiles) {
        if((e->flags & E_FLAG_STEALTH && ~(flags | p.flags) & T_FLAG_STEALTH_TAR)
        || (e->flags & E_FLAG_ARMORED && ~(flags | p.flags) & T_FLAG_ARMORED_TAR)) {
            offset += p.odds;
            continue;
        }

        if(p.odds + offset >= r) { pt = &p; break; }
        offset += p.odds;
    }

    if(!pt) return { }; // Should never happen

    remaining_cooldown = 1.0 / (pt->fire_rate * fire_rate_mod);

    return projectile {
        /* texture:                   */ pt->texture,
        /* path:                      */ pt->path,
        /* id:                        */ pt->id,
        /* start:                     */ { pos_x, pos_y },
        /* direction_vector:          */ (epos - vec2 { pos_x, pos_y }).normalize(),
        /* travelled:                 */ 0.0,
        /* range:                     */ pt->range * range_mod * gs.diff.tower_range_modifier + extra_damage_linear,
        /* speed:                     */ pt->speed * speed_mod * gs.diff.projectile_speed_modifier,
        /* remaining_lifetime:        */ pt->lifetime,
        /* remaining_hits:            */ pt->damage_maxhits + extra_damage_maxhits_linear,
        /* remaining_range_hits:      */ pt->range_maxhits + extra_damage_maxhits_range,
        /* damage:                    */ pt->base_damage * damage_mod,
        /* damage_range:              */ pt->damage_range + extra_damage_range,
        /* damage_type:               */ (uint16_t)(pt->damage_type | extra_damage_types),
        /* flags:                     */ (uint08_t)(flags | pt->flags),
        /* armor_mod:                 */ max(pt->armor_modifier, armor_mod),
        /* pid:                       */ (uint32_t)-1,
        /* debuff_remaining_duration: */ 0.0,
        /* debuff_dps:                */ 0.0,
        /* debuff_speed_multiplier:   */ 1.0,
        /* hits:                      */ { },
        /* enemies:                   */ targets
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

bool tower::try_upgrade(uint8_t path, double price) {
    if(path > 2 || upgrade_paths[path] == 6) return false;
    cost += price;
    const upgrade& u = tower_types[base_type].upgrade_paths[path][upgrade_paths[path]];
    range_mod                   *= u.range_mod;
    fire_rate_mod               *= u.fire_rate_mod;
    speed_mod                   *= u.speed_mod;
    armor_mod                   *= u.armor_mod;
    damage_mod                  *= u.damage_mod;
    flags                       |= u.flags;
    extra_damage_maxhits_linear += u.extra_damage_maxhits_linear;
    extra_damage_maxhits_range  += u.extra_damage_maxhits_range;
    extra_damage_linear         += u.extra_damage_linear;
    extra_damage_range          += u.extra_damage_range;
    extra_damage_types          += u.extra_damage_types;
    upgrade_paths[path]++;
    return true;
}

void tower::set_upgrades(uint8_t bot, uint8_t mid, uint8_t top, double price) {
    cost                        += price;
    while(upgrade_paths[0] < bot) if(!try_upgrade(0, 0.0)) break;
    while(upgrade_paths[1] < mid) if(!try_upgrade(1, 0.0)) break;
    while(upgrade_paths[2] < top) if(!try_upgrade(2, 0.0)) break;
}
