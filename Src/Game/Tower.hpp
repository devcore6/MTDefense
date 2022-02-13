#pragma once
#include <cstdint>
#include <string>

#include "../Engine/GL.hpp"
#include "Enemy.hpp"

enum {
    DAMAGE_BLUNT        = 1 << 0,
    DAMAGE_SHARP        = 1 << 1,
    DAMAGE_HEAT         = 1 << 2,
    DAMAGE_COLD         = 1 << 3,
    DAMAGE_PRESSURE     = 1 << 4,
    DAMAGE_RADIOACTIVE  = 1 << 5,
    DAMAGE_ELECTRIC     = 1 << 6,
    DAMAGE_MAGIC        = 1 << 7,
    DAMAGE_CHEMICAL     = 1 << 8,
    DAMAGE_BIOLOGICAL   = 1 << 9
};

enum {
    TOWER_PHYSICAL      = 0,
    TOWER_CHEMICAL,
    TOWER_BIOLOGICAL,
    TOWER_MAGICAL,
    TOWER_SPECIAL,
    TOWER_SUPPORT
};

struct projectile {
    texture_t   texture;

    bool        can_hit_armored     = false;
    bool        can_hit_stealth     = false;
    bool        strip_armor         = false;
    bool        strip_stealth       = false;

    bool        place_on_track      = false;

    double      base_damage         = 0.0;
    double      armor_modifier      = 0.33;
    double      fire_rate           = 0.0;
    double      speed               = 0.0;
    double      range               = 0.0;              // Projectile range
    double      odds                = 1.0;

    size_t      damage_maxhits      = 0;
    double      damage_range        = 0.0;
    uint16_t    damage_type         = 0;

    animation_t hit_animation;
};

struct animation_set {
    animation_t                                         idle_animation;
    std::vector<std::pair<projectile*, animation_t>>    attack_animations;
};

struct upgrade {
    std::string name                = "";
    std::string desc                = "";
    
    double      base_price          = 0.0;
    double      range_mod           = 1.0;
    double      fire_rate_mod       = 1.0;
    double      speed_mod           = 1.0;
    double      armor_mod           = 1.0;
    double      damage_mod          = 1.0;

    bool        can_hit_armored     = false;
    bool        can_hit_stealth     = false;
    
    size_t      extra_damage_maxhits_linear = 0;
    size_t      extra_damage_maxhits_range  = 0;
    double      extra_damage_linear = 0.0;
    double      extra_damage_range  = 0.0;
    uint16_t    extra_damage_types  = 0;

    std::vector<animation_t> hit_animations;
    std::vector<projectile> projectiles;
};

struct tower {
    std::string name                = "";
    std::string desc                = "";

    double      hitbox_radius       = 0.0;

    double      base_price          = 0.0;
    double      range               = 0.0;              // Tareting range
    bool        place_on_water      = false;

    uint8_t     tower_type          = TOWER_PHYSICAL;

    std::vector<projectile> projectiles;

    upgrade     upgrade_paths[3][6];

    std::string tower_path          = "Data/Towers/";
};

class owned_tower {
private:
    double      remaining_cooldown  = 0.0;
public:
    tower*      base_type           = nullptr;
    rect_t      rect;

    double      cost                = 0.0;              // Used to calculate sale price

    double      pos_x               = 0.0;
    double      pos_y               = 0.0;
    double      rot                 = 0.0;

    double      range_mod           = 1.0;
    double      fire_rate_mod       = 1.0;
    double      speed_mod           = 1.0;
    double      armor_mod           = 1.0;
    double      damage_mod          = 1.0;

    size_t      extra_damage_maxhits_linear = 0;
    size_t      extra_damage_maxhits_range  = 0;
    double      extra_damage_linear = 0.0;
    double      extra_damage_range  = 0.0;
    uint16_t    extra_damage_types  = 0;
    std::vector<animation_t> hit_animations;

    uint8_t     upgrade_paths[3]    = { 0, 0, 0 };

    std::string custom_name         = "";

    void        tick(double time);
    bool        can_fire();
    double      fire(spawned_enemy& e);
    void        render();
};

extern std::vector<tower>       towers;
