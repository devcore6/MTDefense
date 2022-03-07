#include "../Engine/Engine.hpp"
#include "../Engine/Server.hpp"
#include "../Engine/Map.hpp"
#include "Game.hpp"
#include <enet/enet.h>

extern ENetHost* client;
extern ENetPeer* peer;

clientinfo playerinfo;
extern void render_map();

void init_game() {

}

void do_connect() {

}

void mouse_press_handler(int x, int y) { }
void mouse_release_handler(int x, int y) { }

void render_frame() {
    if(current_map) {
        render_map();
    } else {
    //    render_menu();
    }
    render_console();
    render_chat();
}

void main_loop() {
    if(peer) {
         
    }
}
