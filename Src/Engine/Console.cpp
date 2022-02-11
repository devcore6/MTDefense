#include <iostream>
#include <chrono>
#include "Engine.hpp"

ivarp(consoletimeout, 0, 30, 1000);
ivarp(consoleheight, 1, 5, 20);
ivarp(chatheight, 1, 5, 20);
ivarp(chattimeout, 0, 15, 1000);

hvarp(console_white,   0x00000000, 0xffffffff, 0xffffffff);
hvarp(console_red,     0x00000000, 0xc23620ff, 0xffffffff);
hvarp(console_green,   0x00000000, 0x25bc24ff, 0xffffffff);
hvarp(console_yellow,  0x00000000, 0xacac26ff, 0xffffffff);
hvarp(console_blue,    0x00000000, 0x442ee1ff, 0xffffffff);
hvarp(console_magenta, 0x00000000, 0xd337d3ff, 0xffffffff);
hvarp(console_cyan,    0x00000000, 0x33bac7ff, 0xffffffff);
hvarp(console_gray,    0x00000000, 0xcbccccff, 0xffffffff);

svarp(name, "unnamed"_str);

using sc = std::chrono::system_clock;

std::vector<std::pair<sc::time_point, std::string>> console_messages;
std::vector<std::pair<sc::time_point, std::string>>    chat_messages;

void conout(std::string msg) {
    std::cout << msg << '\n';
    console_messages.push_back(std::make_pair(sc::now(), msg));
}

void conerr(std::string msg) {
    std::cerr << msg << '\n';
    console_messages.push_back(std::make_pair(sc::now(), "\\1"_str + msg));
}

command(echo, {
    if(args.size() == 1) conout("Usage: echo \"message\";");
    else conout(args[1]);
})

// Todo: internet features when that gets added
void say(std::string msg) {
    chat_messages.push_back(std::make_pair(sc::now(), name + ": " + msg));
}

command(say, {
    if(args.size() == 1) conout("Usage: say \"message\";");
    else say(args[1]);
})

void add_to_chat(std::string msg) {
    chat_messages.push_back(std::make_pair(sc::now(), msg));
}

void render_console() {
    auto now = sc::now();
    intmax_t rendered = 0;

    for(auto& message : console_messages) {
        if(rendered >= consoleheight) return;
        if(std::chrono::duration_cast<std::chrono::seconds>(now - message.first).count() > consoletimeout) return;
        auto s = message.second;
        uint32_t color = (uint32_t)console_white;
        std::vector<std::pair<std::string, uint32_t>> messages;
        size_t pos = 0, next_pos = 0;

        while((next_pos = s.find("\\", pos)) != std::string::npos) {
            uint32_t next_color = color;
            switch(s[next_pos + 1]) {
                case '0': { next_color = (uint32_t)console_white;   break; }
                case '1': { next_color = (uint32_t)console_red;     break; }
                case '2': { next_color = (uint32_t)console_green;   break; }
                case '3': { next_color = (uint32_t)console_yellow;  break; }
                case '4': { next_color = (uint32_t)console_blue;    break; }
                case '5': { next_color = (uint32_t)console_magenta; break; }
                case '6': { next_color = (uint32_t)console_cyan;    break; }
                case '7': { next_color = (uint32_t)console_gray;    break; }
                default: break;
            }
            if(color != next_color) {
                messages.push_back(std::make_pair(s.substr(0, next_pos), color));
                color = next_color;
                s = s.substr(next_pos + 2);
                pos = 0;
            } else pos = next_pos;
        }

        messages.push_back(std::make_pair(s, color));

        int x = 25;

        for(auto& m : messages)
            x += render_text(m.first, x, (int)(40 + font_size * 1.75 * rendered),
                (uint8_t)((m.second >> 24) & 0xFF),
                (uint8_t)((m.second >> 16) & 0xFF),
                (uint8_t)((m.second >>  8) & 0xFF),
                (uint8_t)( m.second        & 0xFF),
                true, false
            );
        
        rendered++;
    }
}

void render_chat() {
    auto now = sc::now();
    intmax_t rendered = 0;

    for(auto& message : chat_messages) {
        if(rendered >= chatheight) return;
        if(std::chrono::duration_cast<std::chrono::seconds>(now - message.first).count() > chattimeout) return;
        auto s = message.second;
        uint32_t color = (uint32_t)console_white;
        std::vector<std::pair<std::string, uint32_t>> messages;
        size_t pos = 0, next_pos = 0;

        while((next_pos = s.find("\\", pos)) != std::string::npos) {
            uint32_t next_color = color;
            switch(s[next_pos + 1]) {
                case '0': { next_color = (uint32_t)console_white;   break; }
                case '1': { next_color = (uint32_t)console_red;     break; }
                case '2': { next_color = (uint32_t)console_green;   break; }
                case '3': { next_color = (uint32_t)console_yellow;  break; }
                case '4': { next_color = (uint32_t)console_blue;    break; }
                case '5': { next_color = (uint32_t)console_magenta; break; }
                case '6': { next_color = (uint32_t)console_cyan;    break; }
                case '7': { next_color = (uint32_t)console_gray;    break; }
                default: break;
            }
            if(color != next_color) {
                messages.push_back(std::make_pair(s.substr(0, next_pos), color));
                color = next_color;
                s = s.substr(next_pos + 2);
                pos = 0;
            } else pos = next_pos;
        }

        messages.push_back(std::make_pair(s, color));

        int x = 25;

        for(auto& m : messages)
            x += render_text(m.first, x, (int)(height - 45 - font_size * 1.75 * rendered),
                (uint8_t)((m.second >> 24) & 0xFF),
                (uint8_t)((m.second >> 16) & 0xFF),
                (uint8_t)((m.second >>  8) & 0xFF),
                (uint8_t)( m.second        & 0xFF),
                true, false
            );
        
        rendered++;
    }
}
