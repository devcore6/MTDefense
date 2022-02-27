#include "Game.hpp"
#include "Difficulty.hpp"
#include "Tower.hpp"

#include "../Engine/Engine.hpp"

bvarp(encountered_nano_matrioshka, false);
bvarp(encountered_micro_matrioshka, false);
bvarp(encountered_milli_matrioshka, false);
bvarp(encountered_centi_matrioshka, false);
bvarp(encountered_deci_matrioshka, false);
bvarp(encountered_matrioshka, false);
bvarp(encountered_volcanic_matrioshka, false);
bvarp(encountered_siberian_matrioshka, false);
bvarp(encountered_experimental_matrioshka, false);
bvarp(encountered_iron_matrioshka, false);
bvarp(encountered_giga_matrioshka, false);
bvarp(encountered_hardened_matrioshka, false);

enemy* nano_matrioshka = nullptr;
enemy* micro_matrioshka = nullptr;
enemy* milli_matrioshka = nullptr;
enemy* centi_matrioshka = nullptr;
enemy* deci_matrioshka = nullptr;
enemy* matrioshka = nullptr;
enemy* volcanic_matrioshka = nullptr;
enemy* siberian_matrioshka = nullptr;
enemy* experimental_matrioshka = nullptr;
enemy* iron_matrioshka = nullptr;
enemy* giga_matrioshka = nullptr;
enemy* hardened_matrioshka = nullptr;

enemy* chonk = nullptr;
enemy* chonkers = nullptr;
enemy* monster_chonk = nullptr;
enemy* dark_chonky = nullptr;
enemy* soviet_chonkers = nullptr;
enemy* big_momma = nullptr;


void init_enemies() {
    // Base enemies
    nano_matrioshka = new enemy {
        { { "en_US"_str, "Nano Matrioshka"_str } },
        { { "en_US"_str, "The tiniest Matrioshka around. Pretty easy to break."_str } },
        1.0, 1.00, 1.00, 0.125, 0, 0,
        false, false, texture_t("Data/Enemies/nmatrioshka.png"), nullptr,
        { }
    };
    micro_matrioshka = new enemy {
        { { "en_US"_str, "Micro Matrioshka"_str } },
        { { "en_US"_str, "Barely larger than a Nano Matrioshka. Contains a Nano Matrioshka inside."_str } },
        1.0, 1.25, 1.01, 0.160, 0, 0,
        false, false, texture_t("Data/Enemies/umatrioshka.png"), nullptr,
        { nano_matrioshka }
    };
    milli_matrioshka = new enemy {
        { { "en_US"_str, "Milli Matrioshka"_str } },
        { { "en_US"_str, "Small Matrioshka. Contains a Micro Matrioshka inside."_str } },
        1.0, 1.50, 1.02, 0.200, 0, 0,
        false, false, texture_t("Data/Enemies/mmatrioshka.png"), nullptr,
        { micro_matrioshka }
    };
    centi_matrioshka = new enemy {
        { { "en_US"_str, "Centi Matrioshka"_str } },
        { { "en_US"_str, "Still fairly small. Contains a Milli Matrioshka inside."_str } },
        1.0, 2.00, 1.03, 0.240, 0, 0,
        false, false, texture_t("Data/Enemies/cmatrioshka.png"), nullptr,
        { milli_matrioshka }
    };
    deci_matrioshka = new enemy {
        { { "en_US"_str, "Deci Matrioshka"_str } },
        { { "en_US"_str, "Almost a full sized Matrioshka. Contains a Centi Matrioshka and a Nano Matrioshka inside for extra flavor."_str } },
        1.0, 2.00, 1.04, 0.280, 0, 0,
        false, false, texture_t("Data/Enemies/dmatrioshka.png"), nullptr,
        { centi_matrioshka, nano_matrioshka }
    };
    matrioshka = new enemy {
        { { "en_US"_str, "Matrioshka"_str } },
        { { "en_US"_str, "Full sized Matrioshka. Contains two Deci Matrioshkas inside."_str } },
        1.0, 3.00, 1.05, 0.333, 0, 0,
        false, false, texture_t("Data/Enemies/matrioshka.png"), nullptr,
        { deci_matrioshka, deci_matrioshka }
    };

    volcanic_matrioshka = new enemy {
        { { "en_US"_str, "Volcanic Matrioshka"_str } },
        { { "en_US"_str, "Special Matrioshka that is immune to heat and pressure damage. Contains 3 Matrioshkas inside, and has 1.5x the health of a regular Matrioshka"_str } },
        1.5, 2.00, 1.07, 0.250, DAMAGE_HEAT | DAMAGE_PRESSURE, 0,
        false, false, texture_t("Data/Enemies/vmatrioshka.png"), nullptr,
        { matrioshka, matrioshka, matrioshka }
    };
    siberian_matrioshka = new enemy {
        { { "en_US"_str, "Siberian Matrioshka"_str } },
        { { "en_US"_str, "Special Matrioshka that is immune to cold damage. Contains 3 Matrioshkas inside, and has 1.5x the health of a regular Matrioshka"_str } },
        1.5, 2.00, 1.07, 0.250, DAMAGE_COLD, 0,
        false, false, texture_t("Data/Enemies/smatrioshka.png"), nullptr,
        { matrioshka, matrioshka, matrioshka }
    };

    experimental_matrioshka = new enemy {
        { { "en_US"_str, "Experimental Matrioshka"_str } },
        { { "en_US"_str, "A new secret and experimental kind of Matrioshka. It is immune to chemical, magical and blunt-force damage, but is particularly vulnerable to biological damage. Contains 3 Matrioshkas inside, and has 0.5x the health of a regular Matrioshka"_str } },
        0.5, 3.25, 1.07, 0.225, DAMAGE_CHEMICAL | DAMAGE_MAGIC | DAMAGE_BLUNT, DAMAGE_BIOLOGICAL,
        false, false, texture_t("Data/Enemies/xmatrioshka.png"), nullptr,
        { matrioshka, matrioshka, matrioshka }
    };

    iron_matrioshka = new enemy {
        { { "en_US"_str, "Iron Matrioshka"_str } },
        { { "en_US"_str, "A tough and resiliant Matrioshka made out of iron. It is immune to blunt-force and sharp damage, but particularly vulnerable to heat and chemical damage. It has 2x the health of a regular Matrioshka, and contains a Volcanic and a Siberian Matrioshka inside, as well as a couple Nano Matrioshkas."_str } },
        2.0, 1.50, 1.08, 0.375, DAMAGE_BLUNT | DAMAGE_SHARP, DAMAGE_HEAT | DAMAGE_CHEMICAL,
        false, false, texture_t("Data/Enemies/imatrioshka.png"), nullptr,
        { volcanic_matrioshka, siberian_matrioshka, nano_matrioshka, nano_matrioshka, nano_matrioshka }
    };

    giga_matrioshka = new enemy {
        { { "en_US"_str, "Giga Matrioshka"_str } },
        { { "en_US"_str, "A tough Matrioshka for sure. It is immune to pressure damage, has 2x the health of a regular Matrioshka and contains a Volcanic, Siberian and Iron Matrioshka inside, as well as a couple of smaller ones."_str } },
        2.0, 2.50, 1.10, 0.340, DAMAGE_PRESSURE, 0,
        false, false, texture_t("Data/Enemies/gmatrioshka.png"), nullptr,
        { volcanic_matrioshka, siberian_matrioshka, iron_matrioshka, micro_matrioshka, nano_matrioshka }
    };

    hardened_matrioshka = new enemy {
        { { "en_US"_str, "Hardened Matrioshka"_str } },
        { { "en_US"_str, "A Matrioshka made out of hardened carbide steel. It is immune to sharp and pressure damage, has 10x the health of a regular Matrioshka and contains two Giga and one Iron Matrioshka inside. This is getting crazy!"_str } },
        10.0, 2.50, 1.10, 0.350, DAMAGE_SHARP | DAMAGE_PRESSURE, 0,
        false, false, texture_t("Data/Enemies/hmatrioshka.png"), nullptr,
        { giga_matrioshka, giga_matrioshka, iron_matrioshka }
    };

    // Chonkers

    chonk = new enemy { { }, { },  250.0, 1.50,  2.00, 1.0, 0, 0, false, false, texture_t("Data/Enemies/chonk.png"), micro_matrioshka, { hardened_matrioshka, hardened_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka, matrioshka, nano_matrioshka, nano_matrioshka } };
    chonkers = new enemy { { }, { }, 750.0, 1.20,  5.00, 1.0, 0, 0, false, false, texture_t("Data/Enemies/chonkers.png"), matrioshka, { chonk, chonk, chonk, chonk, chonk, hardened_matrioshka, giga_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka } };
    monster_chonk = new enemy { { }, { }, 2000.0, 0.80, 10.00, 1.0, 0, 0, false, false, texture_t("Data/Enemies/mchonk.png"), giga_matrioshka, { chonkers, chonkers, chonkers, chonkers, chonkers, chonkers, chonk, chonk, chonk, chonk } };
    dark_chonky = new enemy { { }, { }, 500.0, 2.65,  8.00, 1.0, DAMAGE_BLUNT | DAMAGE_SHARP, DAMAGE_CHEMICAL, true, false, texture_t("Data/Enemies/dchonky.png"), experimental_matrioshka, { hardened_matrioshka, hardened_matrioshka, hardened_matrioshka, hardened_matrioshka, hardened_matrioshka, chonk } };
    soviet_chonkers = new enemy { { }, { }, 50000.0, 0.40, 75.00, 1.0, DAMAGE_BLUNT, 0, false, false, texture_t("Data/Enemies/schonkers.png"), chonk, { monster_chonk, monster_chonk, monster_chonk, monster_chonk, dark_chonky, dark_chonky, dark_chonky, dark_chonky, iron_matrioshka, iron_matrioshka } };
    big_momma = new enemy { { }, { }, 750000.0, 0.33, 250.00, 1.0, DAMAGE_BLUNT | DAMAGE_SHARP | DAMAGE_RADIOACTIVE, 0, true, false, texture_t("Data/Enemies/schonkers.png"), dark_chonky, { soviet_chonkers, soviet_chonkers, soviet_chonkers, soviet_chonkers, soviet_chonkers, soviet_chonkers, dark_chonky, dark_chonky, dark_chonky, dark_chonky } };
}

void deinit_enemies() {
    if(nano_matrioshka)         { delete nano_matrioshka;         nano_matrioshka         = nullptr; }
    if(micro_matrioshka)        { delete micro_matrioshka;        micro_matrioshka        = nullptr; }
    if(milli_matrioshka)        { delete milli_matrioshka;        milli_matrioshka        = nullptr; }
    if(centi_matrioshka)        { delete centi_matrioshka;        centi_matrioshka        = nullptr; }
    if(deci_matrioshka)         { delete deci_matrioshka;         deci_matrioshka         = nullptr; }
    if(matrioshka)              { delete matrioshka;              matrioshka              = nullptr; }
    if(volcanic_matrioshka)     { delete volcanic_matrioshka;     volcanic_matrioshka     = nullptr; }
    if(siberian_matrioshka)     { delete siberian_matrioshka;     siberian_matrioshka     = nullptr; }
    if(experimental_matrioshka) { delete experimental_matrioshka; experimental_matrioshka = nullptr; }
    if(iron_matrioshka)         { delete iron_matrioshka;         iron_matrioshka         = nullptr; }
    if(giga_matrioshka)         { delete giga_matrioshka;         giga_matrioshka         = nullptr; }
    if(hardened_matrioshka)     { delete hardened_matrioshka;     hardened_matrioshka     = nullptr; }

    if(chonk)                   { delete chonk;                   chonk                   = nullptr; }
    if(chonkers)                { delete chonkers;                chonkers                = nullptr; }
    if(monster_chonk)           { delete monster_chonk;           monster_chonk           = nullptr; }
    if(dark_chonky)             { delete dark_chonky;             dark_chonky             = nullptr; }
    if(soviet_chonkers)         { delete soviet_chonkers;         soviet_chonkers         = nullptr; }
    if(big_momma)               { delete big_momma;               big_momma               = nullptr; }
}

const rounds standard_rounds = {
    {
        { { { &nano_matrioshka,        10.0,    18.0 } } },                 // Round  1

        { { { &nano_matrioshka,        10.0,    18.0 },                     // Round  2
            { &nano_matrioshka,         5.0,     8.0 } } },                 // Round  2

        { { { &nano_matrioshka,        10.0,    22.0 },                     // Round  3
            { &micro_matrioshka,        9.0,     4.0 } } },                 // Round  3

        { { { &nano_matrioshka,         5.0,    14.0 },                     // Round  4
            { &micro_matrioshka,        8.0,    17.0 } } },                 // Round  4

        { { { &micro_matrioshka,        5.0,    18.0 },                     // Round  5
            { &milli_matrioshka,        8.0,     7.0 } } },                 // Round  5

        { { { &milli_matrioshka,        8.0,     8.0 },                     // Round  6
            { &micro_matrioshka,        4.0,    22.0 },                     // Round  6
            { &milli_matrioshka,        8.0,     8.0 } } },                 // Round  6

        { { { &nano_matrioshka,         3.0,    12.0 },                     // Round  7
            { &micro_matrioshka,        3.0,    12.0 },                     // Round  7
            { &milli_matrioshka,        3.0,    12.0 } } },                 // Round  7

        { { { &milli_matrioshka,        6.0,    28.0 } } },                 // Round  8

        { { { &micro_matrioshka,        9.0,    40.0 },                     // Round  9
            { &micro_matrioshka,        4.5,    20.0 },                     // Round  9
            { &micro_matrioshka,        2.3,    10.0 },                     // Round  9
            { &milli_matrioshka,        1.2,     5.0 } } },                 // Round  9

        { { { &nano_matrioshka,        10.0,     8.0 },                     // Round 10
            { &micro_matrioshka,       10.0,     8.0 },                     // Round 10
            { &milli_matrioshka,       10.0,     8.0 },                     // Round 10
            { &centi_matrioshka,       10.0,     8.0 } } },                 // Round 10

        { { { &micro_matrioshka,        8.0,    14.0 },                     // Round 11
            { &milli_matrioshka,        9.0,    12.0 },                     // Round 11
            { &centi_matrioshka,       10.0,    10.0 } } },                 // Round 11

        { { { &micro_matrioshka,        8.0,    32.0 },                     // Round 12
            { &centi_matrioshka,        7.5,    16.0 },                     // Round 12
            { &micro_matrioshka,        4.0,    24.0 },                     // Round 12
            { &deci_matrioshka,        10.0,     1.0 } } },                 // Round 12

        { { { &micro_matrioshka,        6.0,    16.0 },                     // Round 13
            { &nano_matrioshka,         2.5,    64.0 },                     // Round 13
            { &micro_matrioshka,        1.0,     8.0 },                     // Round 13
            { &centi_matrioshka,        7.0,    32.0 } } },                 // Round 13

        { { { &deci_matrioshka,         9.2,     8.0 },                     // Round 14
            { &centi_matrioshka,        8.0,    20.0 },                     // Round 14
            { &centi_matrioshka,        6.0,    10.0 },                     // Round 14
            { &centi_matrioshka,        4.0,     8.0 },                     // Round 14
            { &deci_matrioshka,         3.0,     4.0 } } },                 // Round 14

        { { { &micro_matrioshka,        2.0,    64.0 },                     // Round 15
            { &deci_matrioshka,         8.0,     9.0 } } },                 // Round 15

        { { { &nano_matrioshka,        10.0,     8.0, true } } },           // Round 16

        { { { &deci_matrioshka,         7.2,    16.0 },                     // Round 17
            { &centi_matrioshka,        4.0,     2.0, true },               // Round 17
            { &deci_matrioshka,         6.4,     8.0 },                     // Round 17
            { &centi_matrioshka,        4.0,     4.0, true },               // Round 17
            { &deci_matrioshka,         3.0,     6.0 },                     // Round 17
            { &centi_matrioshka,        4.0,     8.0, true },               // Round 17
            { &micro_matrioshka,        1.0,    32.0 } } },                 // Round 17

        { { { &matrioshka,             10.0,     8.0 } } },                 // Round 18

        { { { &matrioshka,              8.0,    12.0 },                     // Round 19
            { &matrioshka,              6.2,     6.0 } } },                 // Round 19

        { { { &volcanic_matrioshka,     8.0,     8.0 } } },                 // Round 20

        { { { &matrioshka,              7.2,    16.0 },                     // Round 21
            { &volcanic_matrioshka,     4.0,     4.0 },                     // Round 21
            { &matrioshka,              6.4,    12.0 } } },                 // Round 21

        { { { &siberian_matrioshka,     8.0,     8.0 } } },                 // Round 22

        { { { &volcanic_matrioshka,     7.0,     9.0 },                     // Round 23
            { &siberian_matrioshka,     7.0,     7.0 } } },                 // Round 23

        { { { &matrioshka,             10.0,     1.0, true } } },           // Round 24

        { { { &deci_matrioshka,         5.2,    12.0 },                     // Round 25
            { &volcanic_matrioshka,     7.0,     4.0, true },               // Round 25
            { &deci_matrioshka,         6.4,     8.0 },                     // Round 25
            { &centi_matrioshka,        4.0,    16.0, true },               // Round 25
            { &deci_matrioshka,         3.0,     6.0 },                     // Round 25
            { &siberian_matrioshka,     6.5,     8.0, true },               // Round 25
            { &experimental_matrioshka, 8.0,     2.0 } } },                 // Round 25

        { { { &iron_matrioshka,         8.0,     8.0 } } },                 // Round 26

        { { { &iron_matrioshka,         7.4,    12.0 } } },                 // Round 27

        { { { &experimental_matrioshka, 8.0,    16.0 } } },                 // Round 28

        { { { &volcanic_matrioshka,     6.0,    12.0 },                     // Round 29
            { &siberian_matrioshka,     6.0,    12.0 },                     // Round 29
            { &volcanic_matrioshka,     4.2,     8.0 },                     // Round 29
            { &siberian_matrioshka,     4.2,     8.0 } } },                 // Round 29

        { { { &matrioshka,              6.5,    24.0 },                     // Round 30
            { &volcanic_matrioshka,     4.0,     8.0 },                     // Round 30
            { &giga_matrioshka,         8.0,     4.0 } } },                 // Round 30
    }
};
