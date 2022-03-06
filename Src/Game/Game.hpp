#pragma once
#define GAME_NAME "Cuck TD"
#include <map>
#include <string>
#include <mutex>
#include <atomic>

#include "../Engine/Tools.hpp"
#include "../Engine/GL.hpp"

using dictionary_entry = std::map<const char*, const char*>;
using sc = std::chrono::system_clock;

inline const char* get_entry(dictionary_entry t, const char* lang) {
    for(auto& val : t)
        if(!strcmp(val.first, lang)) 
            return val.second;
    return nullptr;
}

enum {
    DAMAGE_NONE             = 0,
    DAMAGE_BLUNT            = 1 << 0,
    DAMAGE_SHARP            = 1 << 1,
    DAMAGE_HEAT             = 1 << 2,
    DAMAGE_COLD             = 1 << 3,
    DAMAGE_PRESSURE         = 1 << 4,
    DAMAGE_RADIOACTIVE      = 1 << 5,
    DAMAGE_ELECTRIC         = 1 << 6,
    DAMAGE_MAGIC            = 1 << 7,
    DAMAGE_CHEMICAL         = 1 << 8,
    DAMAGE_BIOLOGICAL       = 1 << 9
};

enum {
    TOWER_TYPE_PHYSICAL     = 0,
    TOWER_TYPE_CHEMICAL,
    TOWER_TYPE_BIOLOGICAL,
    TOWER_TYPE_MAGICAL,
    TOWER_TYPE_SPECIAL,
    TOWER_TYPE_SUPPORT
};

enum {
    TARGETING_FIRST         = 0,
    TARGETING_LAST,
    TARGETING_WEAK,
    TARGETING_STRONG
};

enum {
    TOWER_SENTRY            = 0,
    NUMTOWERS
};

enum {
    ENEMY_NONE              = -1,
    ENEMY_NANO,
    ENEMY_MICRO,
    ENEMY_MILLI,
    ENEMY_CENTI,
    ENEMY_DECI,
    ENEMY_REGULAR,
    ENEMY_VOLCANIC,
    ENEMY_SIBERIAN,
    ENEMY_EXPERIMENTAL,
    ENEMY_IRON,
    ENEMY_GIGA,
    ENEMY_HARDENED,
    NUMENEMIES
};

enum {
    E_FLAG_NONE             = 0,
    E_FLAG_STEALTH          = 1 << 0,
    E_FLAG_ARMORED          = 1 << 1,
    E_FLAG_SHIELD           = 1 << 2
};

enum {
    P_FLAG_NONE             = 0,
    P_FLAG_STEALTH_TAR      = 1 << 0,
    P_FLAG_ARMORED_TAR      = 1 << 1,
    P_FLAG_STRIP_STEALTH    = 1 << 2,
    P_FLAG_STRIP_ARMOR      = 1 << 3,
    P_FLAG_PLACE_ON_TRACK   = 1 << 4
};

enum {
    T_FLAG_NONE             = 0,
    T_FLAG_STEALTH_TAR      = 1 << 0,
    T_FLAG_ARMORED_TAR      = 1 << 1
};

enum {
    DIFF_EASY               = 0,
    DIFF_MEDIUM,
    DIFF_HARD,
    DIFF_IMPOSSIBLE,
    NUMDIFFS
};

enum {
    N_CONNECT               = 0,
    N_TEXT,
    N_PLACETOWER,
    N_UPGRADETOWER,
    N_UPDATETARGETING,
    N_SELLTOWER,
    N_UPDATE_CASH,
    N_UPDATE_LIVES,
    N_REQUEST_UPDATE,
    N_UPDATE_ENTITIES,
    N_PING,
    N_PONG,
    N_ROUNDINFO,
    N_STARTROUND,
    N_SET_SPEED,
    N_PAUSE,
    N_RESUME,
    N_ROUNDOVER,
    N_GAMEOVER,
    N_RESTART,
    N_CONTINUE,
    N_DISCONNECT,
    N_SPAWN_ENEMY,
    N_PROJECTILE,
    N_DETONATE,
    N_DELETE_PROJECTILE,
    N_ENEMY_SURVIVED,
    N_KILL_ENEMY,
    NUMMSG
};

// Message sizes in bytes, -1 for unchecked/variable sizes.
constexpr uint32_t msgsizes[NUMMSG] = {
    /* N_CONNECT:                   */   (uint32_t)-1,
    /* N_TEXT:                      */   (uint32_t)-1,
    /* N_PLACETOWER:                */             20,
    /* N_UPGRADETOWER:              */              5,
    /* N_UPDATETARGETING:           */              5,
    /* N_SELLTOWER:                 */              4,
    /* N_UPDATE_CASH:               */              8,
    /* N_UPDATE_LIVES:              */              8,
    /* N_REQUEST_UPDATE:            */              0,
    /* N_UPDATE_ENTITIES:           */   (uint32_t)-1,
    /* N_PING:                      */              0,
    /* N_PONG:                      */              0,
    /* N_ROUNDINFO:                 */              4,
    /* N_STARTROUND:                */              0,
    /* N_SET_SPEED:                 */              1,
    /* N_PAUSE:                     */              0,
    /* N_RESUME:                    */              0,
    /* N_ROUNDOVER:                 */              0,
    /* N_GAMEOVER:                  */              0, // This will probably send some stuff later like what enemy was leaked and some kind of score
    /* N_RESTART:                   */              0,
    /* N_CONTINUE:                  */              0,
    /* N_DISCONNECT:                */              0,
    /* N_SPAWN_ENEMY:               */              0,
    /* N_PROJECTILE:                */              0,
    /* N_DETONATE:                  */              0,
    /* N_DELETE_PROJECTILE:         */              0,
    /* N_ENEMY_SURVIVED:            */              0,
    /* N_KILL_ENEMY:                */              0
};

static const struct enemy_t {
    dictionary_entry                name;
    dictionary_entry                description;
    int                             base_type;
    double                          base_health;
    double                          base_speed;
    double                          base_kill_reward;
    double                          scale;
    uint16_t                        immunities;
    uint16_t                        vulnerabilities;
    uint8_t                         flags;
    texture_t                       texture;
    int                             spawns_when_damaged;
    std::initializer_list<int>      spawns;
} enemy_types[] = {
    {
        /* name: */ {
            { "en_US", "Nano Matrioshka" }
        },
        /* description: */ {
            { "en_US", "The tiniest Matrioshka around. Pretty easy to break." }
        },
        /* base_type: */            ENEMY_NANO,
        /* base_health: */          1.0,
        /* base_speed: */           1.00,
        /* base_kill_reward: */     1.00,
        /* scale: */                0.125,
        /* immunities: */           DAMAGE_NONE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/nmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */               { }
    },
    {
        /* description: */ {
            { "en_US", "Micro Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Barely larger than a Nano Matrioshka. Contains a Nano Matrioshka inside." } 
        },
        /* base_type: */            ENEMY_MICRO,
        /* base_health: */          1.0,
        /* base_speed: */           1.25,
        /* base_kill_reward: */     1.01,
        /* scale: */                0.160,
        /* immunities: */           DAMAGE_NONE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/umatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_NANO
        }
    },
    {
        /* description: */ {
            { "en_US", "Milli Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Small Matrioshka. Contains a Micro Matrioshka inside." } 
        },
        /* base_type: */            ENEMY_MILLI,
        /* base_health: */          1.0,
        /* base_speed: */           1.50,
        /* base_kill_reward: */     1.02,
        /* scale: */                0.200,
        /* immunities: */           DAMAGE_NONE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/mmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_MICRO
        }
    },
    {
        /* description: */ {
            { "en_US", "Centi Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Still fairly small. Contains a Milli Matrioshka inside." } 
        },
        /* base_type: */            ENEMY_CENTI,
        /* base_health: */          1.0,
        /* base_speed: */           2.00,
        /* base_kill_reward: */     1.03,
        /* scale: */                0.240,
        /* immunities: */           DAMAGE_NONE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/cmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_MILLI
        }
    },
    {
        /* description: */ {
            { "en_US", "Deci Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Almost a full sized Matrioshka. Contains a Centi Matrioshka and a Nano Matrioshka inside for extra flavor." } 
        },
        /* base_type: */            ENEMY_DECI,
        /* base_health: */          1.0,
        /* base_speed: */           2.00,
        /* base_kill_reward: */     1.04,
        /* scale: */                0.280,
        /* immunities: */           DAMAGE_NONE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/dmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_CENTI,
                                    ENEMY_NANO
        }
    },
    {
        /* description: */ {
            { "en_US", "Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Full sized Matrioshka. Contains two Deci Matrioshkas inside." } 
        },
        /* base_type: */            ENEMY_REGULAR,
        /* base_health: */          1.0,
        /* base_speed: */           3.00,
        /* base_kill_reward: */     1.05,
        /* scale: */                0.333,
        /* immunities: */           DAMAGE_NONE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/matrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_DECI,
                                    ENEMY_DECI
        }
    },
    {
        /* description: */ {
            { "en_US", "Volcanic Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Special Matrioshka that is immune to heat and pressure damage. Contains 3 Matrioshkas inside, and has 1.5x the health of a regular Matrioshka" } 
        },
        /* base_type: */            ENEMY_VOLCANIC,
        /* base_health: */          1.5,
        /* base_speed: */           2.00,
        /* base_kill_reward: */     1.07,
        /* scale: */                0.250,
        /* immunities: */           DAMAGE_HEAT | DAMAGE_PRESSURE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/vmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_REGULAR,
                                    ENEMY_REGULAR,
                                    ENEMY_REGULAR
        }
    },
    {
        /* description: */ {
            { "en_US", "Siberian Matrioshka" }
        },
        /* name: */ {
            { "en_US", "Special Matrioshka that is immune to cold damage. Contains 3 Matrioshkas inside, and has 1.5x the health of a regular Matrioshka" } 
        },
        /* base_type: */            ENEMY_SIBERIAN,
        /* base_health: */          1.5,
        /* base_speed: */           2.00,
        /* base_kill_reward: */     1.07,
        /* scale: */                0.250,
        /* immunities: */           DAMAGE_COLD,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/smatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_REGULAR,
                                    ENEMY_REGULAR,
                                    ENEMY_REGULAR
        }
    },
    {
        /* description: */ {
            { "en_US", "Experimental Matrioshka" }
        },
        /* name: */ {
            { "en_US", "A new secret and experimental kind of Matrioshka. It is immune to chemical, magical and blunt-force damage, but is particularly vulnerable to biological damage. Contains 3 Matrioshkas inside, and has 0.5x the health of a regular Matrioshka" } 
        },
        /* base_type: */            ENEMY_EXPERIMENTAL,
        /* base_health: */          0.5,
        /* base_speed: */           3.25,
        /* base_kill_reward: */     1.07,
        /* scale: */                0.225,
        /* immunities: */           DAMAGE_CHEMICAL | DAMAGE_MAGIC | DAMAGE_BLUNT,
        /* vulnerabilities: */      DAMAGE_BIOLOGICAL,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/xmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_REGULAR,
                                    ENEMY_REGULAR,
                                    ENEMY_REGULAR
        }
    },
    {
        /* description: */ {
            { "en_US", "Iron Matrioshka" }
        },
        /* name: */ {
            { "en_US", "A tough and resiliant Matrioshka made out of iron. It is immune to blunt-force and sharp damage, but particularly vulnerable to heat and chemical damage. It has 2x the health of a regular Matrioshka, and contains a Volcanic and a Siberian Matrioshka inside, as well as a couple Nano Matrioshkas." } 
        },
        /* base_type: */            ENEMY_IRON,
        /* base_health: */          2.0,
        /* base_speed: */           1.50,
        /* base_kill_reward: */     1.08,
        /* scale: */                0.375,
        /* immunities: */           DAMAGE_BLUNT | DAMAGE_SHARP,
        /* vulnerabilities: */      DAMAGE_HEAT | DAMAGE_CHEMICAL,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/imatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_VOLCANIC,
                                    ENEMY_SIBERIAN,
                                    ENEMY_NANO,
                                    ENEMY_NANO,
                                    ENEMY_NANO
        }
    },
    {
        /* description: */ {
            { "en_US", "Giga Matrioshka" }
        },
        /* name: */ {
            { "en_US", "A tough Matrioshka for sure. It is immune to pressure damage, has 2x the health of a regular Matrioshka and contains a Volcanic, Siberian and Iron Matrioshka inside, as well as a couple of smaller ones." } 
        },
        /* base_type: */            ENEMY_GIGA,
        /* base_health: */          2.0,
        /* base_speed: */           2.50,
        /* base_kill_reward: */     1.10,
        /* scale: */                0.340,
        /* immunities: */           DAMAGE_PRESSURE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/gmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_VOLCANIC,
                                    ENEMY_SIBERIAN,
                                    ENEMY_IRON,
                                    ENEMY_MICRO,
                                    ENEMY_NANO
        }
    },
    {
        /* description: */ {
            { "en_US", "Hardened Matrioshka" }
        },
        /* name: */ {
            { "en_US", "A Matrioshka made out of hardened carbide steel. It is immune to sharp and pressure damage, has 10x the health of a regular Matrioshka and contains two Giga and one Iron Matrioshka inside. This is getting crazy!" } 
        },
        /* base_type: */            ENEMY_HARDENED,
        /* base_health: */          10.0,
        /* base_speed: */           2.50,
        /* base_kill_reward: */     1.10,
        /* scale: */                0.350,
        /* immunities: */           DAMAGE_SHARP | DAMAGE_PRESSURE,
        /* vulnerabilities: */      DAMAGE_NONE,
        /* flags: */                E_FLAG_NONE,
        /* texture: */              texture_t("Data/Enemies/hmatrioshka.png"),
        /* spawns_when_damaged: */  ENEMY_NONE,
        /* spawns: */ {
                                    ENEMY_GIGA,
                                    ENEMY_GIGA,
                                    ENEMY_IRON
        }
    }
};

struct enemy {
    int             base_type;
    std::mutex*     lock;
    line_strip_t*   route;
    double          distance_travelled;
    vertex_2d       pos;
    double          max_health;
    double          health;
    double          speed;
    double          kill_reward;
    uint16_t        immunities;
    uint16_t        vulnerabilities;
    uint8_t         flags;
    double          slowed_for;
    double          frozen_for;
    uint32_t        id;

    ~enemy() { if(lock) delete lock; }
};

struct enemy_set {
    uint8_t         base_type;
    double          spacing;
    double          amount;
    uint8_t         flags;
    uint16_t        immunities;
    uint16_t        vulnerabilities;
};

struct round_t {
    std::initializer_list<enemy_set> enemies;
    double          kill_cash_multiplier                = 1.00;
    double          special_odds_multiplier             = 1.00;;
    double          shield_strength_multiplier          = 1.00;;
    double          enemy_speed_multiplier              = 1.00;;
    double          enemy_health_multiplier             = 1.00;;
};

static const struct rounds {
    round_t         r[150];
    double          late_game_enemy_amount_multiplier   = 1.50;
    double          late_game_multiplier_multiplier     = 1.25;
} standard_rounds = {
    /* round_set: */ {
        /* Round 1: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          10.0,
                    /* amount: */           18.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 2: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          10.0,
                    /* amount: */           18.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          5.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 3: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          10.0,
                    /* amount: */           22.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          9.0,
                    /* amount: */           4.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 4: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          5.0,
                    /* amount: */           14.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          8.0,
                    /* amount: */           17.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 5: */ {
            {
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          5.0,
                    /* amount: */           18.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          8.0,
                    /* amount: */           7.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 6: */ {
            {
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          8.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          4.0,
                    /* amount: */           22.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          8.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 7: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          3.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          3.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          3.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 8: */ {
            {
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          6.0,
                    /* amount: */           28.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                    }
            }
        },
        /* Round 9: */ {
            {
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          9.0,
                    /* amount: */           40.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          4.5,
                    /* amount: */           20.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          2.3,
                    /* amount: */           10.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          1.2,
                    /* amount: */           5.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 10: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          10.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          10.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          10.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          10.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 11: */ {
            {
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          8.0,
                    /* amount: */           14.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MILLI,
                    /* spacing: */          9.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          10.0,
                    /* amount: */           10.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 12: */ {
            {
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          8.0,
                    /* amount: */           32.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          7.5,
                    /* amount: */           16.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          4.0,
                    /* amount: */           24.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          10.0,
                    /* amount: */           1.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 13: */ {
            {
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          6.0,
                    /* amount: */           16.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          2.5,
                    /* amount: */           64.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          1.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          7.0,
                    /* amount: */           32.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 14: */ {
            {
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          9.2,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          8.0,
                    /* amount: */           20.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          6.0,
                    /* amount: */           10.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          4.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          3.0,
                    /* amount: */           4.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 15: */ {
            {
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          2.0,
                    /* amount: */           64.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          8.0,
                    /* amount: */           9.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 16: */ {
            {
                {
                    /* base_type: */        ENEMY_NANO,
                    /* spacing: */          10.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 17: */ {
            {
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          7.2,
                    /* amount: */           16.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          4.0,
                    /* amount: */           2.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          6.4,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          4.0,
                    /* amount: */           4.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          3.0,
                    /* amount: */           6.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          4.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_MICRO,
                    /* spacing: */          1.0,
                    /* amount: */           32.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 18: */ {
            {
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          10.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                    }
            }
        },
        /* Round 19: */ {
            {
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          8.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          6.2,
                    /* amount: */           6.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 20: */ {
            {
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          8.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                    }
            }
        },
        /* Round 21: */ {
            {
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          7.2,
                    /* amount: */           16.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          4.0,
                    /* amount: */           4.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          6.4,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 22: */ {
            {
                {
                    /* base_type: */        ENEMY_SIBERIAN,
                    /* spacing: */          8.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                    }
            }
        },
        /* Round 23: */ {
            {
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          7.0,
                    /* amount: */           9.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_SIBERIAN,
                    /* spacing: */          7.0,
                    /* amount: */           7.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 24: */ {
            {
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          10.0,
                    /* amount: */           1.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 25: */ {
            {
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          5.2,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          7.0,
                    /* amount: */           4.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          6.4,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_CENTI,
                    /* spacing: */          4.0,
                    /* amount: */           16.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_DECI,
                    /* spacing: */          3.0,
                    /* amount: */           6.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_SIBERIAN,
                    /* spacing: */          6.5,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_STEALTH,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_EXPERIMENTAL,
                    /* spacing: */          8.0,
                    /* amount: */           2.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 26: */ {
            {
                {
                    /* base_type: */        ENEMY_IRON,
                    /* spacing: */          8.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                    }
            }
        },
        /* Round 27: */ {
            {
                {
                    /* base_type: */        ENEMY_IRON,
                    /* spacing: */          7.4,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 28: */ {
            {
                {
                    /* base_type: */        ENEMY_EXPERIMENTAL,
                    /* spacing: */          8.0,
                    /* amount: */           16.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 29: */ {
            {
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          6.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_SIBERIAN,
                    /* spacing: */          6.0,
                    /* amount: */           12.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          4.2,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_SIBERIAN,
                    /* spacing: */          4.2,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
        /* Round 30: */ {
            {
                {
                    /* base_type: */        ENEMY_REGULAR,
                    /* spacing: */          6.5,
                    /* amount: */           24.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_VOLCANIC,
                    /* spacing: */          4.0,
                    /* amount: */           8.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                },
                {
                    /* base_type: */        ENEMY_GIGA,
                    /* spacing: */          8.0,
                    /* amount: */           4.0,
                    /* flags: */            E_FLAG_NONE,
                    /* immunities: */       DAMAGE_NONE,
                    /* vulnerabilities: */  DAMAGE_NONE
                }
            }
        },
    }
};

static const struct difficulty {
    size_t          rounds_to_win;
    size_t          lives;
    double          enemy_speed_modifier;
    double          enemy_health_modifier;
    double          enemy_spawns_modifier;
    double          enemy_amount_modifier;
    double          enemy_random_stealth_odds;
    double          enemy_random_armored_odds;
    double          enemy_random_shield_odds;
    double          enemy_shield_strength;
    uint16_t        enemy_base_immunities;
    double          enemy_kill_reward_modifier;
    double          start_cash_modifier;
    double          cash_production_modifier;
    double          tower_cost_modifier;
    double          tower_range_modifier;
    double          tower_fire_rate_modifier;
    double          tower_sale_price_factor;
    double          projectile_speed_modifier;
    bool            disable_knowledge_and_powers;
    double          reward_modifier;
    rounds          round_set;
} difficulties[] = {
    /* DIFF_EASY: */ {
        /* rounds_to_win: */                40,
        /* lives: */                        500,
        /* enemy_speed_modifier: */         0.9,
        /* enemy_health_modifier: */        0.9,
        /* enemy_spawns_modifier: */        0.9,
        /* enemy_amount_modifier: */        0.8,
        /* enemy_random_stealth_odds: */    0.00000,
        /* enemy_random_armored_odds: */    0.00000,
        /* enemy_random_shield_odds: */     0.00000,
        /* enemy_shield_strength: */        1.0,
        /* enemy_base_immunities: */        0,
        /* enemy_kill_reward_modifier: */   1.20,
        /* start_cash_modifier: */          1.20,
        /* cash_production_modifier: */     1.20,
        /* tower_cost_modifier: */          0.8,
        /* tower_range_modifier: */         1.10,
        /* tower_fire_rate_modifier: */     1.10,
        /* tower_sale_price_factor: */      0.90,
        /* projectile_speed_modifier: */    1.20,
        /* disable_knowledge_and_powers: */ false,
        /* reward_modifier: */              1.0,
        /* round_set: */                    standard_rounds
    },
    /* DIFF_MEDIUM: */ {
        /* rounds_to_win: */                60,
        /* lives: */                        250,
        /* enemy_speed_modifier: */         1.0,
        /* enemy_health_modifier: */        1.0,
        /* enemy_spawns_modifier: */        1.0,
        /* enemy_amount_modifier: */        1.0,
        /* enemy_random_stealth_odds: */    0.00000,
        /* enemy_random_armored_odds: */    0.00000,
        /* enemy_random_shield_odds: */     0.00000,
        /* enemy_shield_strength: */        1.5,
        /* enemy_base_immunities: */        0,
        /* enemy_kill_reward_modifier: */   1.00,
        /* start_cash_modifier: */          1.00,
        /* cash_production_modifier: */     1.00,
        /* tower_cost_modifier: */          1.0,
        /* tower_range_modifier: */         1.00,
        /* tower_fire_rate_modifier: */     1.00,
        /* tower_sale_price_factor: */      0.85,
        /* projectile_speed_modifier: */    1.00,
        /* disable_knowledge_and_powers: */ false,
        /* reward_modifier: */              2.0,
        /* round_set: */                    standard_rounds
    },
    /* DIFF_HARD: */ {
        /* rounds_to_win: */                80,
        /* lives: */                        100,
        /* enemy_speed_modifier: */         1.2,
        /* enemy_health_modifier: */        1.1,
        /* enemy_spawns_modifier: */        1.3,
        /* enemy_amount_modifier: */        1.4,
        /* enemy_random_stealth_odds: */    0.00002,
        /* enemy_random_armored_odds: */    0.00001,
        /* enemy_random_shield_odds: */     0.00001,
        /* enemy_shield_strength: */        2.0,
        /* enemy_base_immunities: */        0,
        /* enemy_kill_reward_modifier: */   0.92,
        /* start_cash_modifier: */          0.90,
        /* cash_production_modifier: */     0.95,
        /* tower_cost_modifier: */          1.2,
        /* tower_range_modifier: */         0.96,
        /* tower_fire_rate_modifier: */     0.95,
        /* tower_sale_price_factor: */      0.80,
        /* projectile_speed_modifier: */    0.98,
        /* disable_knowledge_and_powers: */ false,
        /* reward_modifier: */              3.0,
        /* round_set: */                    standard_rounds
    },
    /* DIFF_IMPOSSIBLE: */ {
        /* rounds_to_win: */                100,
        /* lives: */                        1,
        /* enemy_speed_modifier: */         1.3,
        /* enemy_health_modifier: */        1.2,
        /* enemy_spawns_modifier: */        1.4,
        /* enemy_amount_modifier: */        1.6,
        /* enemy_random_stealth_odds: */    0.00003,
        /* enemy_random_armored_odds: */    0.00002,
        /* enemy_random_shield_odds: */     0.00005,
        /* enemy_shield_strength: */        3.0,
        /* enemy_base_immunities: */        0,
        /* enemy_kill_reward_modifier: */   0.82,
        /* start_cash_modifier: */          0.85,
        /* cash_production_modifier: */     0.88,
        /* tower_cost_modifier: */          1.3,
        /* tower_range_modifier: */         0.94,
        /* tower_fire_rate_modifier: */     0.92,
        /* tower_sale_price_factor: */      0.75,
        /* projectile_speed_modifier: */    0.95,
        /* disable_knowledge_and_powers: */ false,
        /* reward_modifier: */              4.0,
        /* round_set: */                    standard_rounds
    }
};

struct projectile_t {
    texture_t   texture;
    uint32_t    id;
    uint8_t     flags;
    double      base_damage;
    double      armor_modifier;
    double      fire_rate;
    double      speed;
    double      range;
    size_t      odds;
    double      lifetime;
    size_t      damage_maxhits;
    size_t      range_maxhits;
    double      damage_range;
    uint16_t    damage_type;
    animation_t hit_animation;
};

struct projectile {
    texture_t   texture;
    uint32_t    id;
    vertex_2d   start;
    vertex_2d   direction_vector;
    double      travelled;
    double      range;
    double      remaining_lifetime;
    size_t      remaining_hits;
    size_t      remaining_range_hits;
    double      damage;
    double      damage_range;
    uint16_t    damage_type;
    uint8_t     flags;
    double      armor_mod;
    uint32_t    pid;
    std::vector<enemy*> hits;
};

struct upgrade {
    dictionary_entry                    name;
    dictionary_entry                    desc;
    double                              base_price;
    double                              range_mod;
    double                              fire_rate_mod;
    double                              speed_mod;
    double                              armor_mod;
    double                              damage_mod;
    uint8_t                             flags;
    size_t                              extra_damage_maxhits_linear;
    size_t                              extra_damage_maxhits_range;
    double                              extra_damage_linear;
    double                              extra_damage_range;
    uint16_t                            extra_damage_types;
    std::vector<animation_t>            hit_animations;
    std::initializer_list<projectile_t> projectiles;
};

static struct tower_t {
    uint8_t                             tower_type;
    dictionary_entry                    name;
    dictionary_entry                    desc;
    double                              hitbox_radius;
    double                              base_price;
    double                              range;
    bool                                place_on_water;
    uint8_t                             tower_family;
    std::initializer_list<projectile_t> projectiles;
    upgrade                             upgrade_paths[3][6];
    std::map<std::string, animation_t>  animations;
} tower_types[] = {
    {
        /* tower_type: */                               TOWER_SENTRY,
        /* name: */ {
            { "en_US", "Sentry" }
        },
        /* desc: */ {
            { "en_US", "A simple sentry that shoots oncoming enemies." }
        },
        /* hitbox_radius: */                            50.0,
        /* base_price: */                               185.00,
        /* range: */                                    250.00,
        /* place_on_water: */                           false,
        /* tower_family: */                             TOWER_TYPE_PHYSICAL,
        /* projectiles: */ {
            {
                /* texture: */                          texture_t("Data/Towers/Sentry/Projectiles/Basic pellet.png"),
                /* id: */                               0,
                /* flags: */                            P_FLAG_NONE,
                /* base_damage: */                      1.0,
                /* armor_modifier: */                   0.33,
                /* fire_rate: */                        1.0,
                /* speed: */                            2000.00,
                /* range: */                            175.00,
                /* odds: */                             4,
                /* lifetime: */                         0.0,
                /* damage_maxhits: */                   1,
                /* range_maxhits: */                    0,
                /* damage_range: */                     0.0,
                /* damage_type: */                      DAMAGE_BLUNT,
                /* hit_animation: */                    { }
            }
        },
        /* upgrade_paths: */ {
           {
                /* 1-x-x: */ {
                    /* name: */ {
                        { "en_US", "Faster firing" }
                    },
                    /* desc: */ {
                        { "en_US", "Lighter bolt moves faster to allow for 15% faster firing rate." }
                    },
                    /* base_price: */                   75.00,
                    /* range_mod: */                    1.0,
                    /* fire_rate_mod: */                1.15,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Ported receiver" }
                    },
                    /* desc: */ {
                        { "en_US", "Ported receiver allows for better cooling for even faster firing." }
                    },
                    /* base_price: */                   200.00,
                    /* range_mod: */                    1.0,
                    /* fire_rate_mod: */                1.38,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Double barrel" }
                    },
                    /* desc: */ {
                        { "en_US", "Two barrels shoot twice as fast." }
                    },
                    /* base_price: */                   400.00,
                    /* range_mod: */                    1.0,
                    /* fire_rate_mod: */                2.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Hotter gunpowder" }
                    },
                    /* desc: */ {
                        { "en_US", "More aggressive gunpowder burns hotter, increasing firing rate, projectile speed, reach, damage, and allows it to damage more enemy types." }
                     },
                    /* base_price: */                   640.00,
                    /* range_mod: */                    1.25,
                    /* fire_rate_mod: */                1.10,
                    /* speed_mod: */                    1.8,
                    /* armor_mod: */                    1.25,
                    /* damage_mod: */                   2.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_HEAT,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Explosive ammunition" }
                    },
                    /* desc: */ {
                        { "en_US", "Adds minor explosive properties to ALL of this tower's ammunition." }
                    },
                    /* base_price: */                   2900.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.25,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   12,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           6.0,
                    /* extra_damage_types: */           DAMAGE_PRESSURE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { /* Add explosion animation here later */ }
                },
                {
                    /* name: */ {
                        { "en_US", "The SHREDDER" }
                    },
                    /* desc: */ {
                        { "en_US", "Adds a third barrel, and DOUBLES the firing rate of each barrel for ultimate destruction." }
                    },
                    /* base_price: */                   12500.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                3.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                }
            },
            {
                {
                    /* name: */ {
                        { "en_US", "Stealth detection" }
                    },
                    /* desc: */ {
                        { "en_US", "Allows this sentry to target and hit stealthed enemies." }
                    },
                    /* base_price: */                   200.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_STEALTH_TAR,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Sharpened bullets" }
                    },
                    /* desc: */ {
                        { "en_US", "This sentry fires sharpened bullets which can target armored enemies." }
                    },
                    /* base_price: */                   350.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_ARMORED_TAR,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_SHARP,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "AP rounds" }
                    },
                    /* desc: */ {
                        { "en_US", "This sentry frequently fires armor piercing rounds, which do more damage and do full damage to armored opponents." }
                     },
                    /* base_price: */                   725.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */ {
                        {
                            /* texture: */              texture_t("Data/Towers/Sentry/Projectiles/AP pellet.png"),
                            /* id: */                   1,
                            /* flags: */                P_FLAG_ARMORED_TAR,
                            /* base_damage: */          2.0,
                            /* armor_modifier: */       1.0,
                            /* fire_rate: */            1.0,
                            /* speed: */                250.00,
                            /* range: */                175.00,
                            /* odds: */                 2,
                            /* lifetime: */             0.0,
                            /* damage_maxhits: */       1,
                            /* range_maxhits: */        0,
                            /* damage_range: */         0.0,
                            /* damage_type: */          DAMAGE_SHARP,
                            /* hit_animation: */        { }
                        }
                    }
                },
                {
                    /* name: */ {
                        { "en_US", "Longer barrel" }
                    },
                    /* desc: */ {
                        { "en_US", "Replaces this sentry's barrels with longer ones, for additional range, bullet velocity and slightly larger damage." }
                     },
                    /* base_price: */                   700.00,
                    /* range_mod: */                    1.30,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.2,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.44,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Rifled barrel" }
                    },
                    /* desc: */ {
                        { "en_US", "Replaces this sentry's barrels with rifled ones, for even longer range and slightly better armor penetration." }
                     },
                    /* base_price: */                   1150.00,
                    /* range_mod: */                    2.50,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.20,
                    /* damage_mod: */                   1.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Special ammunition" }
                    },
                    /* desc: */ {
                        { "en_US", "Adds different types of special ammunition that do extra damage to specific enemy classes. Also slightly increases all stats." }
                     },
                    /* base_price: */                   15000.00,
                    /* range_mod: */                    1.10,
                    /* fire_rate_mod: */                1.10,
                    /* speed_mod: */                    1.1,
                    /* armor_mod: */                    1.10,
                    /* damage_mod: */                   1.25,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  {
                        {
                            /* texture: */              texture_t("Data/Towers/Sentry/Projectiles/Chemical pellet.png"),
                            /* id: */                   2,
                            /* flags: */                P_FLAG_NONE,
                            /* base_damage: */          2.5,
                            /* armor_modifier: */       0.5,
                            /* fire_rate: */            1.0,
                            /* speed: */                250.00,
                            /* range: */                175.00,
                            /* odds: */                 1,
                            /* lifetime: */             0.0,
                            /* damage_maxhits: */       1,
                            /* range_maxhits: */        0,
                            /* damage_range: */         0.0,
                            /* damage_type: */          DAMAGE_BLUNT | DAMAGE_CHEMICAL,
                            /* hit_animation: */        { }
                        },
                        {
                            /* texture: */              texture_t("Data/Towers/Sentry/Projectiles/Biological pellet.png"),
                            /* id: */                   3,
                            /* flags: */                P_FLAG_NONE,
                            /* base_damage: */          2.5,
                            /* armor_modifier: */       0.5,
                            /* fire_rate: */            1.0,
                            /* speed: */                250.00,
                            /* range: */                175.00,
                            /* odds: */                 1,
                            /* lifetime: */             0.0,
                            /* damage_maxhits: */       1,
                            /* range_maxhits: */        0,
                            /* damage_range: */         0.0,
                            /* damage_type: */          DAMAGE_BLUNT | DAMAGE_BIOLOGICAL,
                            /* hit_animation: */        { }
                        },
                        {
                            /* texture: */              texture_t("Data/Towers/Sentry/Projectiles/Stripping pellet.png"),
                            /* id: */                   4,
                            /* flags: */                P_FLAG_STEALTH_TAR | P_FLAG_ARMORED_TAR | P_FLAG_STRIP_STEALTH | P_FLAG_STRIP_ARMOR,
                            /* base_damage: */          2.5,
                            /* armor_modifier: */       0.5,
                            /* fire_rate: */            1.0,
                            /* speed: */                250.00,
                            /* range: */                175.00,
                            /* odds: */                 1,
                            /* lifetime: */             0.0,
                            /* damage_maxhits: */       1,
                            /* range_maxhits: */        0,
                            /* damage_range: */         0.0,
                            /* damage_type: */          DAMAGE_BLUNT | DAMAGE_MAGIC,
                            /* hit_animation: */        { }
                        }
                    }
                }
            },
            {
                {
                    /* name: */ {
                        { "en_US", "Copper jacket" }
                    },
                    /* desc: */ {
                        { "en_US", "Adds a tough copper jacket to the projectiles which doubles the projectile's damage." }
                    },
                    /* base_price: */                   275.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   2.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Larger caliber" }
                    },
                    /* desc: */ {
                        { "en_US", "Replaces the projectiles with much stronger larger caliber ones, which doubles the damage once more." }
                    },
                    /* base_price: */                   1200.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   2.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Hardened steel tripod" }
                    },
                    /* desc: */ {
                        { "en_US", "Replaces the tripod with a studier hardened steel one, which allows for higher accuracy and thus higher damage. Doubles total damage output once more, and slightly increases fire rate." }
                     },
                    /* base_price: */                   3100.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.20,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   2.00,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Steel penetrator cores" }
                    },
                    /* desc: */ {
                        { "en_US", "Gives all projectiles steel penetrator cores, which slightly increase the damage output and allow all projectiles to go through one additional enemy." }
                     },
                    /* base_price: */                   4500.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.25,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  1,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          128.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_NONE,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Special concoction" }
                    },
                    /* desc: */ {
                        { "en_US", "Coats all bullets in a special biological and chemical concoction for assured destruction." }
                    },
                    /* base_price: */                   1500.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.25,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_CHEMICAL | DAMAGE_BIOLOGICAL,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                },
                {
                    /* name: */ {
                        { "en_US", "Mana infusion" }
                    },
                    /* desc: */ {
                        { "en_US", "Infuses bullets with pure mana. Greatly increases damage output." }
                    },
                    /* base_price: */                   11500.00,
                    /* range_mod: */                    1.00,
                    /* fire_rate_mod: */                1.00,
                    /* speed_mod: */                    1.0,
                    /* armor_mod: */                    1.00,
                    /* damage_mod: */                   1.75,
                    /* flags: */                        T_FLAG_NONE,
                    /* extra_damage_maxhits_linear: */  0,
                    /* extra_damage_maxhits_range: */   0,
                    /* extra_damage_linear: */          0.0,
                    /* extra_damage_range: */           0.0,
                    /* extra_damage_types: */           DAMAGE_MAGIC,
                    /* hit_animations: */               { },
                    /* projectiles: */                  { }
                }
            }
        },
        /* animations: */                               map_animations("Data/Towers/Sentry/")
    }
};

class tower {
public:
    double          remaining_cooldown          = 0.0;
    uint8_t         base_type                   = NUMTOWERS;
    rect_t          rect;
    double          cost                        = 0.0; // Used to calculate sale price
    double          pos_x                       = 0.0;
    double          pos_y                       = 0.0;
    double          rot                         = 0.0;
    double          range_mod                   = 1.0;
    double          fire_rate_mod               = 1.0;
    double          speed_mod                   = 1.0;
    double          armor_mod                   = 1.0;
    double          damage_mod                  = 1.0;
    uint8_t         flags                       = T_FLAG_NONE;
    size_t          extra_damage_maxhits_linear = 0;
    size_t          extra_damage_maxhits_range  = 0;
    double          extra_damage_linear         = 0.0;
    double          extra_damage_range          = 0.0;
    uint16_t        extra_damage_types          = 0;
    std::vector<animation_t> hit_animations;
    std::vector<projectile_t> projectiles;
    uint8_t         upgrade_paths[3]            = { 0, 0, 0 };
    uint8_t         targeting_mode              = TARGETING_FIRST;
    std::string     custom_name                 = "";

                             tower(tower_t* t, double c, double x, double y);
    void                     tick(double time);
    bool                     can_fire();
    result<projectile, void> fire(enemy* e);
    void                     render();
    void                     try_upgrade(uint8_t path, double price);
};

struct game_state {
    size_t                  cur_round;
    size_t                  last_round;
    size_t                  last_route;
    std::atomic<double>     lives;
    bool                    double_speed;
    bool                    paused;
    bool                    running;
    difficulty              diff;
    uint32_t                spawned_enemies;
    uint32_t                spawned_projectiles;
    bool                    done_spawning;
    std::vector<enemy>      created_enemies;
    std::vector<enemy*>     first;
    std::vector<enemy*>     last;
    std::vector<enemy*>     strong;
    std::vector<enemy*>     weak;
    std::vector<projectile> projectiles;
    sc::time_point          last_tick;
    sc::time_point          last_spawned_tick;
    std::vector<tower>      towers;
};

extern game_state gs;

struct clientinfo {
    uint32_t id = 0;
    void* cn = nullptr;
    sc::time_point last_message = sc::now();
    double cash = 0.0;
    std::vector<tower*> towers;
};

#ifndef __SERVER__
extern clientinfo playerinfo;
#endif
