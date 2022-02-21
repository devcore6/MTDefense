#include <filesystem>
#include <random>

#include "../Engine/Tools.hpp"
#include "Tower.hpp"
std::vector<tower> towers;

extern std::mt19937_64 rng;

owned_tower::owned_tower(tower* t, double c, double x, double y) : base_type(t), cost(c), pos_x(x), pos_y(y), projectiles(t->projectiles) {
    rect.vertices[0] = { -t->hitbox_radius, -t->hitbox_radius };
    rect.vertices[1] = {  t->hitbox_radius, -t->hitbox_radius };
    rect.vertices[2] = {  t->hitbox_radius,  t->hitbox_radius };
    rect.vertices[3] = { -t->hitbox_radius,  t->hitbox_radius };
}

void owned_tower::tick(double time) {
    remaining_cooldown = max(remaining_cooldown - time, 0.0);
}

bool owned_tower::can_fire() { return remaining_cooldown <= 0.0; }

double owned_tower::fire(spawned_enemy* e) {
    if(!can_fire()) return -1.0;
    double dx = e->pos.x - pos_x;
    double dy = e->pos.y - pos_y;
    double d  = sqrt(dx * dx + dy * dy);
    if(d > base_type->range * range_mod) return -1.0;
    size_t odds = 0;
    for(auto& p : projectiles) odds += p.odds;
    if(odds == 0) return -1.0; // Should never happen
    size_t r = rng() % odds;
    size_t offset = 0;
    projectile* p = nullptr;
    for(auto i : iterate(projectiles.size())) if(projectiles[i].odds + offset >= r) { p = &projectiles[i]; break; }
    if(!p) return -1.0; // Should never happen
    last_projectile = p;
    uint16_t damage_type = p->damage_type | extra_damage_types;
    if(damage_type & ~e->immunities) {
        double dmg = p->base_damage * damage_mod;
        if(e->armored) dmg *= armor_mod;
        if(damage_type & e->vulnerabilities) dmg *= 2;
        remaining_cooldown = 1.0 / (p->fire_rate * fire_rate_mod);
        return dmg;
    }
    return 0.0;
}

void owned_tower::render() {
    std::string upgrade_path = std::to_string(upgrade_paths[0]) + '-'
                             + std::to_string(upgrade_paths[1]) + '-'
                             + std::to_string(upgrade_paths[2]);
    rect.anim = base_type->animations[upgrade_path];
    rect.render({ pos_x, pos_y }, rot);
}

std::map<std::string, animation_t> map_animations(std::string path) {
    std::map<std::string, animation_t> ret;

    for(auto& dir : std::filesystem::directory_iterator(path)) {
        if(!dir.is_directory()) continue;
        std::string name = dir.path().filename().string();
        if(name.length() != 5) continue;
        if(!isdigit(name[0]) || name[1] != '-' || !isdigit(name[2]) || name[3] != '-' || !isdigit(name[4])) continue;
        ret.insert({ { name, animation_t(dir.path().string()) } });
    }

    return ret;
}

void owned_tower::try_upgrade(uint8_t path, double price) {
    if(path > 2 || upgrade_paths[path] == 5) return;
    cost += price;
    upgrade* u = &base_type->upgrade_paths[path][upgrade_paths[path]];
    range_mod                   *= u->range_mod;
    fire_rate_mod               *= u->fire_rate_mod;
    speed_mod                   *= u->speed_mod;
    armor_mod                   *= u->armor_mod;
    damage_mod                  *= u->damage_mod;

    can_hit_armored             |= u->can_hit_armored;
    can_hit_stealth             |= u->can_hit_stealth;

    extra_damage_maxhits_linear += u->extra_damage_maxhits_linear;
    extra_damage_maxhits_range  += u->extra_damage_maxhits_range;
    extra_damage_linear         += u->extra_damage_linear;
    extra_damage_range          += u->extra_damage_range;
    extra_damage_types          += u->extra_damage_types;

    upgrade_paths[path]++;
}

void init_towers() {
    towers.push_back({
        "Sentry"_str,
        "A simple sentry that shoots oncoming enemies."_str,
        50.0,
        200.00, 250.00, false,
        TOWER_PHYSICAL,
        {
            {
                texture_t("Data/Towers/Sentry/Projectiles/Basic pellet.png"),
                false,
                false,
                false,
                false,
                false,
                1.0, 0.33, 1.0,
                2000.00, 175.00, 4,
                1, 0.0, DAMAGE_BLUNT,
                { }
            }
        },
        {
            {
                {
                    "Faster firing"_str,
                    "Lighter bolt moves faster to allow for 15% faster firing rate."_str,
                    80.00,
                    1.0, 1.15, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Ported receiver"_str,
                    "Ported receiver allows for better cooling for even faster firing."_str,
                    240.00,
                    1.0, 1.33, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Double barrel"_str,
                    "Two barrels shoot twice as fast."_str,
                    400.00,
                    1.0, 2.00, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Hotter gunpowder"_str,
                    "More aggressive gunpowder burns hotter, increasing firing rate, projectile speed, reach, damage, and allows it to damage more enemy types."_str,
                    800.00,
                    1.25, 1.10, 1.8, 1.25, 2.00,
                    false, false,
                    0, 0, 0.0, 0.0, DAMAGE_HEAT,
                    { }, { }
                },
                {
                    "Explosive ammunition"_str,
                    "Adds minor explosive properties to ALL of this tower's ammunition."_str,
                    2500.00,
                    1.00, 1.00, 1.0, 1.00, 1.25,
                    false, false,
                    0, 8, 0.0, 4.0, DAMAGE_PRESSURE,
                    { }, {
                        // Add explosion animation here later
                    }
                },
                {
                    "The SHREDDER"_str,
                    "Adds a third barrel, and DOUBLES the firing rate of each barrel for ultimate destruction."_str,
                    12500.00,
                    1.00, 3.00, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }
                }
            },
            {
                {
                    "Stealth detection"_str,
                    "Allows this sentry to target and hit stealthed enemies."_str,
                    200.00,
                    1.00, 1.00, 1.0, 1.00, 1.00,
                    true, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Sharpened bullets"_str,
                    "This sentry fires sharpened bullets which can target armored enemies."_str,
                    350.00,
                    1.00, 1.00, 1.0, 1.00, 1.00,
                    false, true,
                    0, 0, 0.0, 0.0, DAMAGE_SHARP,
                    { }, { }
                },
                {
                    "AP rounds"_str,
                    "This sentry frequently fires armor piercing rounds, which do more damage and do full damage to armored opponents."_str,
                    750.00,
                    1.00, 1.00, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, {
                        {
                            texture_t("Data/Towers/Sentry/Projectiles/AP pellet.png"),
                            true,
                            false,
                            false,
                            false,
                            false,
                            2.0, 1.0, 1.0,
                            250.00, 175.00, 2,
                            1, 0.0, DAMAGE_SHARP,
                            { }
                        }
                    }
                },
                {
                    "Longer barrel"_str,
                    "Replaces this sentry's barrels with longer ones, for additional range, bullet velocity and slightly larger damage."_str,
                    800.00,
                    1.30, 1.00, 1.2, 1.00, 1.44,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Rifled barrel"_str,
                    "Replaces this sentry's barrels with rifled ones, for even longer range and slightly better armor penetration."_str,
                    1200.00,
                    2.50, 1.00, 1.0, 1.20, 1.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Special ammunition"_str,
                    "Adds different types of special ammunition that do extra damage to specific enemy classes. Also slightly increases all stats."_str,
                    12000.00,
                    1.10, 1.05, 1.1, 1.10, 1.13,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, {
                        {
                            texture_t("Data/Towers/Sentry/Projectiles/Chemical pellet.png"),
                            true,
                            false,
                            false,
                            false,
                            false,
                            2.5, 0.5, 1.0,
                            250.00, 175.00, 1,
                            1, 0.0, DAMAGE_BLUNT | DAMAGE_CHEMICAL,
                            { }
                        },
                        {
                            texture_t("Data/Towers/Sentry/Projectiles/Biological pellet.png"),
                            true,
                            false,
                            false,
                            false,
                            false,
                            2.5, 0.5, 1.0,
                            250.00, 175.00, 1,
                            1, 0.0, DAMAGE_BLUNT | DAMAGE_BIOLOGICAL,
                            { }
                        },
                        {
                            texture_t("Data/Towers/Sentry/Projectiles/Stripping pellet.png"),
                            true,
                            true,
                            true,
                            true,
                            true,
                            2.5, 0.5, 1.0,
                            250.00, 175.00, 1,
                            1, 0.0, DAMAGE_BLUNT | DAMAGE_MAGIC,
                            { }
                        }
                    }
                }
            },
            {
                {
                    "Copper jacket"_str,
                    "Adds a tough copper jacket to the projectiles which doubles the projectile's damage."_str,
                    300.00,
                    1.00, 1.00, 1.0, 1.00, 2.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Larger caliber"_str,
                    "Replaces the projectiles with much stronger larger caliber ones, which doubles the damage once more."_str,
                    1200.00,
                    1.00, 1.00, 1.0, 1.00, 2.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Hardened steel tripod"_str,
                    "Replaces the tripod with a studier hardened steel one, which allows for higher accuracy and thus higher damage. Doubles total damage output once more, and slightly increases fire rate."_str,
                    2100.00,
                    1.00, 1.20, 1.0, 1.00, 2.00,
                    false, false,
                    0, 0, 0.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Steel penetrator cores"_str,
                    "Gives all projectiles steel penetrator cores, which slightly increase the damage output and allow all projectiles to go through one additional enemy."_str,
                    3500.00,
                    1.00, 1.00, 1.0, 1.00, 1.25,
                    false, false,
                    1, 0, 16.0, 0.0, 0,
                    { }, { }
                },
                {
                    "Special concoction"_str,
                    "Coats all bullets in a special biological and chemical concoction for assured destruction."_str,
                    1500.00,
                    1.00, 1.00, 1.0, 1.00, 1.25,
                    false, false,
                    0, 0, 0.0, 0.0, DAMAGE_CHEMICAL | DAMAGE_BIOLOGICAL,
                    { }, { }
                },
                {
                    "Mana infusion"_str,
                    "Infuses bullets with pure mana. Greatly increases damage output."_str,
                    12500.00,
                    1.00, 1.00, 1.0, 1.00, 1.75,
                    false, false,
                    0, 0, 0.0, 0.0, DAMAGE_MAGIC,
                    { }, { }
                }
            }
        },
        map_animations("Data/Towers/Sentry/"_str)
    });
}
