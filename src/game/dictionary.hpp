#pragma once

static dictionary_entry targeting_mode_names[NUMTARGETINGS] = {
    /* TARGETING_FIRST:*/ {
        { "en_US", "First" }
    },
    /* TARGETING_LAST: */ {
        { "en_US", "Last" }
    },
    /* TARGETING_WEAK: */ {
        { "en_US", "Weak" }
    },
    /* TARGETING_STRONG: */ {
        { "en_US", "Strong" }
    }
};

enum {
    DICT_UNRECOGNIZED_TOWER = 0,
    DICT_UNRECOGNIZED_PACKET,
    NUM_DICTIONARY_ENTRIES
};

static dictionary_entry dictionary[NUM_DICTIONARY_ENTRIES] = {
    /* DICT_UNRECOGNIZED_TOWER: */ {
        { "en_US", "Unrecognized tower type: " }
    },
    /* DICT_UNRECOGNIZED_PACKET: */ {
        { "en_US", "Unrecognized packet type" }
    }
};

extern std::string language;

static const char* get_entry(dictionary_entry& d) {
    for(auto& entry : d)
        if(language == entry.first)
            return entry.second;

    return d["en_US"];
}

static const char* get_entry(int id) {
    if(id < 0 || id > NUM_DICTIONARY_ENTRIES) return nullptr;

    for(auto& entry : dictionary[id])
        if(language == entry.first)
            return entry.second;

    return dictionary[id]["en_US"];
}
