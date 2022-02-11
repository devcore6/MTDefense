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
	    "Nano Matrioshka"_str, "The tiniest Matrioshka around. Pretty easy to break."_str,
	    1.0, 1.00, 1.00, 0.125, 0, 0,
	    false, false, texture_t("Data/Enemies/nmatrioshka.png"), nullptr,
	    { }
    };
    micro_matrioshka = new enemy {
        "Micro Matrioshka"_str, "Barely larger than a Nano Matrioshka. Contains a Nano Matrioshka inside."_str,
        1.0, 1.25, 1.01, 0.160, 0, 0,
        false, false, texture_t("Data/Enemies/umatrioshka.png"), nullptr,
        { nano_matrioshka }
    };
    milli_matrioshka = new enemy {
        "Milli Matrioshka"_str, "Small Matrioshka. Contains a Micro Matrioshka inside."_str,
        1.0, 1.50, 1.02, 0.200, 0, 0,
        false, false, texture_t("Data/Enemies/mmatrioshka.png"), nullptr,
        { micro_matrioshka }
    };
    centi_matrioshka = new enemy {
        "Centi Matrioshka"_str, "Still fairly small. Contains a Milli Matrioshka inside."_str,
        1.0, 2.00, 1.03, 0.240, 0, 0,
        false, false, texture_t("Data/Enemies/cmatrioshka.png"), nullptr,
        { milli_matrioshka }
    };
    deci_matrioshka = new enemy {
        "Deci Matrioshka"_str, "Almost a full sized Matrioshka. Contains a Centi Matrioshka and a Nano Matrioshka inside for extra flavor."_str,
        1.0, 2.00, 1.04, 0.280, 0, 0,
        false, false, texture_t("Data/Enemies/dmatrioshka.png"), nullptr,
        { centi_matrioshka, nano_matrioshka }
    };
    matrioshka = new enemy {
        "Matrioshka"_str, "Full sized Matrioshka. Contains two Deci Matrioshkas inside."_str,
        1.0, 3.00, 1.05, 0.333, 0, 0,
        false, false, texture_t("Data/Enemies/matrioshka.png"), nullptr,
        { deci_matrioshka, deci_matrioshka }
    };

    volcanic_matrioshka = new enemy {
        "Volcanic Matrioshka"_str, "Special Matrioshka that is immune to heat and pressure damage. Contains 3 Matrioshkas inside, and has 1.5x the health of a regular Matrioshka"_str,
        1.5, 2.00, 1.07, 0.250, DAMAGE_HEAT | DAMAGE_PRESSURE, 0,
        false, false, texture_t("Data/Enemies/vmatrioshka.png"), nullptr,
        { matrioshka, matrioshka, matrioshka }
    };
    siberian_matrioshka = new enemy {
        "Siberian Matrioshka"_str, "Special Matrioshka that is immune to cold damage. Contains 3 Matrioshkas inside, and has 1.5x the health of a regular Matrioshka"_str,
        1.5, 2.00, 1.07, 0.250, DAMAGE_COLD, 0,
        false, false, texture_t("Data/Enemies/smatrioshka.png"), nullptr,
        { matrioshka, matrioshka, matrioshka }
    };

    experimental_matrioshka = new enemy {
        "Experimental Matrioshka"_str, "A new secret and experimental kind of Matrioshka. It is immune to chemical, magical and blunt-force damage, but is particularly vulnerable to biological damage. Contains 3 Matrioshkas inside, and has 0.5x the health of a regular Matrioshka"_str,
        0.5, 3.25, 1.07, 0.225, DAMAGE_CHEMICAL | DAMAGE_MAGIC | DAMAGE_BLUNT, DAMAGE_BIOLOGICAL,
        false, false, texture_t("Data/Enemies/xmatrioshka.png"), nullptr,
        { matrioshka, matrioshka, matrioshka }
    };

    iron_matrioshka = new enemy {
        "Iron Matrioshka"_str, "A tough and resiliant Matrioshka made out of iron. It is immune to blunt-force and sharp damage, but particularly vulnerable to heat and chemical damage. It has 2x the health of a regular Matrioshka, and contains a Volcanic and a Siberian Matrioshka inside, as well as a couple Nano Matrioshkas."_str,
        2.0, 1.50, 1.08, 0.375, DAMAGE_BLUNT | DAMAGE_SHARP, DAMAGE_HEAT | DAMAGE_CHEMICAL,
        false, false, texture_t("Data/Enemies/imatrioshka.png"), nullptr,
        { volcanic_matrioshka, siberian_matrioshka, nano_matrioshka, nano_matrioshka, nano_matrioshka }
    };

    giga_matrioshka = new enemy {
        "Giga Matrioshka"_str, "A tough Matrioshka for sure. It is immune to pressure damage, has 2x the health of a regular Matrioshka and contains a Volcanic, Siberian and Iron Matrioshka inside, as well as a couple of smaller ones."_str,
        2.0, 2.50, 1.10, 0.340, DAMAGE_PRESSURE, 0,
        false, false, texture_t("Data/Enemies/gmatrioshka.png"), nullptr,
        { volcanic_matrioshka, siberian_matrioshka, iron_matrioshka, micro_matrioshka, nano_matrioshka }
    };

    hardened_matrioshka = new enemy {
        "Hardened Matrioshka"_str, "A Matrioshka made out of hardened carbide steel. It is immune to sharp and pressure damage, has 10x the health of a regular Matrioshka and contains two Giga and one Iron Matrioshka inside. This is getting crazy!"_str,
        10.0, 2.50, 1.10, 0.350, DAMAGE_SHARP | DAMAGE_PRESSURE, 0,
        false, false, texture_t("Data/Enemies/hmatrioshka.png"), nullptr,
        { giga_matrioshka, giga_matrioshka, iron_matrioshka }
    };

    // Chonkers

	chonk = new enemy { ""_str, ""_str,  250.0, 1.50,  2.00, 1.0, 0, 0, false, false, texture_t("Data/Enemies/chonk.png"), micro_matrioshka, { hardened_matrioshka, hardened_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka, matrioshka, nano_matrioshka, nano_matrioshka } };
	chonkers = new enemy { ""_str, ""_str, 750.0, 1.20,  5.00, 1.0, 0, 0, false, false, texture_t("Data/Enemies/chonkers.png"), matrioshka, { chonk, chonk, chonk, chonk, chonk, hardened_matrioshka, giga_matrioshka, experimental_matrioshka, experimental_matrioshka, experimental_matrioshka } };
	monster_chonk = new enemy { ""_str, ""_str, 2000.0, 0.80, 10.00, 1.0, 0, 0, false, false, texture_t("Data/Enemies/mchonk.png"), giga_matrioshka, { chonkers, chonkers, chonkers, chonkers, chonkers, chonkers, chonk, chonk, chonk, chonk } };
	dark_chonky = new enemy { ""_str, ""_str, 500.0, 2.65,  8.00, 1.0, DAMAGE_BLUNT | DAMAGE_SHARP, DAMAGE_CHEMICAL, true, false, texture_t("Data/Enemies/dchonky.png"), experimental_matrioshka, { hardened_matrioshka, hardened_matrioshka, hardened_matrioshka, hardened_matrioshka, hardened_matrioshka, chonk } };
	soviet_chonkers = new enemy { ""_str, ""_str, 50000.0, 0.40, 75.00, 1.0, DAMAGE_BLUNT, 0, false, false, texture_t("Data/Enemies/schonkers.png"), chonk, { monster_chonk, monster_chonk, monster_chonk, monster_chonk, dark_chonky, dark_chonky, dark_chonky, dark_chonky, iron_matrioshka, iron_matrioshka } };
	big_momma = new enemy { ""_str, ""_str, 750000.0, 0.33, 250.00, 1.0, DAMAGE_BLUNT | DAMAGE_SHARP | DAMAGE_RADIOACTIVE, 0, true, false, texture_t("Data/Enemies/schonkers.png"), dark_chonky, { soviet_chonkers, soviet_chonkers, soviet_chonkers, soviet_chonkers, soviet_chonkers, soviet_chonkers, dark_chonky, dark_chonky, dark_chonky, dark_chonky } };
}

void deinit_enemies() {
    if(nano_matrioshka)         { delete nano_matrioshka;         nano_matrioshka = nullptr; }
    if(micro_matrioshka)        { delete micro_matrioshka;        micro_matrioshka = nullptr; }
    if(milli_matrioshka)        { delete milli_matrioshka;        milli_matrioshka = nullptr; }
    if(centi_matrioshka)        { delete centi_matrioshka;        centi_matrioshka = nullptr; }
    if(deci_matrioshka)         { delete deci_matrioshka;         deci_matrioshka = nullptr; }
    if(matrioshka)              { delete matrioshka;              matrioshka = nullptr; }
    if(volcanic_matrioshka)     { delete volcanic_matrioshka;     volcanic_matrioshka = nullptr; }
    if(siberian_matrioshka)     { delete siberian_matrioshka;     siberian_matrioshka = nullptr; }
    if(experimental_matrioshka) { delete experimental_matrioshka; experimental_matrioshka = nullptr; }
    if(iron_matrioshka)         { delete iron_matrioshka;         iron_matrioshka = nullptr; }
    if(giga_matrioshka)         { delete giga_matrioshka;         giga_matrioshka = nullptr; }
    if(hardened_matrioshka)     { delete hardened_matrioshka;     hardened_matrioshka = nullptr; }

    if(chonk)                   { delete chonk;                   chonk = nullptr; }
    if(chonkers)                { delete chonkers;                chonkers = nullptr; }
    if(monster_chonk)           { delete monster_chonk;           monster_chonk = nullptr; }
    if(dark_chonky)             { delete dark_chonky;             dark_chonky = nullptr; }
    if(soviet_chonkers)         { delete soviet_chonkers;         soviet_chonkers = nullptr; }
    if(big_momma)               { delete big_momma;               big_momma = nullptr; }
}

const rounds standard_rounds = {
    {
        { { { &nano_matrioshka,        10.0,    20.0 } } },                 // Round 1

        { { { &nano_matrioshka,        10.0,    20.0 },                     // Round 2
            { &nano_matrioshka,         5.0,    10.0 } } },                 // Round 2

        { { { &nano_matrioshka,        10.0,    25.0 },                     // Round 3
            { &micro_matrioshka,        9.0,     5.0 } } },                 // Round 3

        { { { &nano_matrioshka,         5.0,    15.0 },                     // Round 4
            { &micro_matrioshka,        8.0,    20.0 } } },                 // Round 4

        { { { &micro_matrioshka,        5.0,    20.0 },                     // Round 5
            { &milli_matrioshka,        8.0,     8.0 } } },                 // Round 5

        // todo: the other rounds
    }
};