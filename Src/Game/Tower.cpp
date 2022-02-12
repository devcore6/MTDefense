#include "../Engine/Tools.hpp"
#include "Tower.hpp"
std::vector<tower>       available_towers;

void init_towers() {
    available_towers.push_back({
        "Sentry"_str,
        "A simple sentry that shoots oncoming enemies."_str,
        50.0,
        200.00, 150.00, false,
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
                250.00, 175.00, 4.0,
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
                    0, 0.0, 0,
                    { }, { }
                },
                {
                    "Ported receiver"_str,
                    "Ported receiver allows for better cooling for even faster firing."_str,
                    240.00,
                    1.0, 1.33, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0.0, 0,
                    { }, { }
                },
                {
                    "Double barrel"_str,
                    "Two barrels shoot twice as fast."_str,
                    400.00,
                    1.0, 2.00, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0.0, 0,
                    { }, { }
                },
                {
                    "Hotter gunpowder"_str,
                    "More aggressive gunpowder burns hotter, which heats projectiles and releases more energy, increasing projectile speed, reach, damage, and allows it to damage more enemy types. Also shoots slightly faster."_str,
                    800.00,
                    1.25, 1.10, 1.8, 1.25, 2.00,
                    false, false,
                    0, 0.0, DAMAGE_HEAT,
                    { }, { }
                },
                {
                    "Explosive ammunition"_str,
                    "Adds minor explosive properties to ALL of this tower's ammunition."_str,
                    500.00,
                    1.00, 1.00, 1.0, 1.00, 1.25,
                    false, false,
                    8, 4.0, DAMAGE_PRESSURE,
                    { }, {
                        // Add explosion animation here later
                    }
                },
                {
                    "The SHREDDER"_str,
                    "Adds a third barrel, and DOUBLES the firing rate of each barrel for ultimate destruction."_str,
                    2500.00,
                    1.00, 3.00, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0.0, 0,
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
                    0, 0.0, 0,
                    { }, { }
                },
                {
                    "Sharpened bullets"_str,
                    "This sentry fires sharpened bullets which can target armored enemies."_str,
                    350.00,
                    1.00, 1.00, 1.0, 1.00, 1.00,
                    false, true,
                    0, 0.0, DAMAGE_SHARP,
                    { }, { }
                },
                {
                    "AP rounds"_str,
                    "This sentry frequently fires armor piercing rounds, which do more damage and do full damage to armored opponents."_str,
                    750.00,
                    1.00, 1.00, 1.0, 1.00, 1.00,
                    false, false,
                    0, 0.0, 0,
                    { }, {
                        {
                            texture_t("Data/Towers/Sentry/Projectiles/AP pellet.png"),
                            true,
                            false,
                            false,
                            false,
                            false,
                            2.0, 1.0, 1.0,
                            250.00, 175.00, 2.0,
                            1, 0.0, DAMAGE_SHARP,
                            { }
                        }
                    }
                },
                {

                },
                {

                },
                {

                }
            },
            { { }, { }, { }, { }, { }, { } }
        },
        "Data/Towers/Sentry/"_str
    });
}
