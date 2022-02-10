#pragma once
#include "../Engine/GL.hpp"

struct enemy {
    std::string                             name                = "";
    std::string                             description         = "";

    double                                  base_health         = 1.0;
    double                                  base_speed          = 1.0;
    double                                  base_kill_reward    = 1.0;

    double                                  scale               = 1.0;
    
    uint16_t                                immunities          = 0;
    uint16_t                                vulnerabilities     = 0;

    bool                                    stealth             = false;
    bool                                    armored             = false;

    texture_t                               texture;

    enemy*                                  spawns_when_damaged = nullptr;

    std::vector<enemy*>                     spawns;
};

extern enemy         nano_matrioshka;
extern enemy        micro_matrioshka;
extern enemy        milli_matrioshka;
extern enemy        centi_matrioshka;
extern enemy         deci_matrioshka;
extern enemy              matrioshka;

extern enemy     volcanic_matrioshka;
extern enemy     siberian_matrioshka;

extern enemy experimental_matrioshka;

extern enemy         iron_matrioshka;

extern enemy         giga_matrioshka;

extern enemy     hardened_matrioshka;

extern enemy                   chonk;
extern enemy                chonkers;
extern enemy           monster_chonk;
extern enemy             dark_chonky;
extern enemy         soviet_chonkers;

struct spawned_enemy {
    line_strip_t*                           route               = nullptr;
    vertex_2d                               pos                 = { 0.0, 0.0 };

    double                                  max_health          = 1.0;
    double                                  health              = 1.0;
    double                                  speed               = 1.0;
    double                                  kill_reward         = 1.0;

    double                                  scale               = 1.0;
    
    uint16_t                                immunities          = 0;
    uint16_t                                vulnerabilities     = 0;

    bool                                    stealth             = false;
    bool                                    armored             = false;
    bool                                    shield              = false;

    texture_t                               texture;

    enemy*                                  spawns_when_damaged = nullptr;
    std::vector<enemy*>                     spawns;
};
