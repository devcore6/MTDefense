#pragma once

class game_ui: public cxxgui::embeddable {
private:
    bool clicking = false;
    bool send_click = false;
    int lastx = 0;
    int lasty = 0;
    double update_at = DBL_MAX;

    struct data_t {
        uint8_t val;
        double cost;
        bool owned;
    };

    std::vector<data_t*> to_free;

    void do_update() {
        last_selected = selected;
        update_at = DBL_MAX;

        while(to_free.size()) {
            delete to_free[to_free.size() - 1];
            to_free.pop_back();
        }

        if(!selected) return;

        std::string upgrade_path = std::to_string(selected->upgrade_paths[0]) + '-'
            + std::to_string(selected->upgrade_paths[1]) + '-'
            + std::to_string(selected->upgrade_paths[2]);

        std::string upgrade_paths[3] = { std::to_string(selected->upgrade_paths[0] + 1) + '-'
                                       + std::to_string(selected->upgrade_paths[1]) + '-'
                                       + std::to_string(selected->upgrade_paths[2]),
                                         std::to_string(selected->upgrade_paths[0]) + '-'
                                       + std::to_string(selected->upgrade_paths[1] + 1) + '-'
                                       + std::to_string(selected->upgrade_paths[2]),
                                         std::to_string(selected->upgrade_paths[0]) + '-'
                                       + std::to_string(selected->upgrade_paths[1]) + '-'
                                       + std::to_string(selected->upgrade_paths[2] + 1) };

        uint8_t upgrade_count = selected->upgrade_paths[0] + selected->upgrade_paths[1] + selected->upgrade_paths[2];

        float x = (selected->pos_x >= 710.0) ? 0.0f : 1088.0f;

        if(body) delete body;
        body = new vstack {
            new image(
                       tower_types[selected->base_type].animations[upgrade_path].frames[0].textid,
                (float)tower_types[selected->base_type].animations[upgrade_path].frames[0].width,
                (float)tower_types[selected->base_type].animations[upgrade_path].frames[0].height
            )
        };

        body->frame(500.0f, 500.0f, 500.0f,
            1080.0f, 1080.0f, 1080.0f,
            alignment_t::center);
        body->offset(x, 0.0f);
        body->padding(16.0f);
        body->background_color(color::background);

        vstack* s = new vstack {
            (new text(selected->custom_name != "" ? selected->custom_name.c_str() : get_entry(tower_types[selected->base_type].name)))
                ->font(title)
        };

        ((vstack*)body)->add(s);

        s->add(new hstack {
            symbols::backward_circle()
                ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center)
                ->offset(-16.0f, 0.0f)
                ->on_click([&](view*, float, float, void*) {
                    uint8_t targeting_mode = selected->targeting_mode - 1;
                    if(targeting_mode >= NUMTARGETINGS) targeting_mode = NUMTARGETINGS - 1;
                    packetstream p { };
                    p << N_UPDATETARGETING
                      << 5_u32
                      << selected->id
                      << targeting_mode;
                    send_packet(peer, 0, true, p);
                }),
            (new text(get_entry(targeting_mode_names[selected->targeting_mode])))
                ->font(headline),
             symbols::forward_circle()
                ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center)
                ->offset(-16.0f, 0.0f)
                ->on_click([&](view*, float, float, void*) {
                    uint8_t targeting_mode = selected->targeting_mode + 1;
                    if(targeting_mode >= NUMTARGETINGS) targeting_mode = 0;
                    packetstream p { };
                    p << N_UPDATETARGETING
                      << 5_u32
                      << selected->id
                      << targeting_mode;
                    send_packet(peer, 0, true, p);
                })
            }
        );

        hstack* upgrades_available = new hstack { };

        for(uint8_t i = 0_u8; i < upgrade_count; i++)
            upgrades_available->add(
                symbols::stop_fill()
                ->frame(32.0f, 32.0f, 32.0f, 32.0f, 32.0f, 32.0f, alignment_t::center)
            );

        for(uint8_t i = upgrade_count; i < 8_u8; i++)
            upgrades_available->add(
                symbols::stop()
                ->frame(32.0f, 32.0f, 32.0f, 32.0f, 32.0f, 32.0f, alignment_t::center)
            );

        s->add(upgrades_available->offset(-20.0f, -0.0f));

        for(uint8_t i = 0_u8; i < 3_u8; i++) {
            vstack* upgrades_stack = new vstack { };

            for(uint8_t j = 0_u8; j < selected->upgrade_paths[i]; j++)
                upgrades_stack->add(
                    symbols::stop_fill()
                    ->frame(24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f, alignment_t::center)
                );

            for(uint8_t j = selected->upgrade_paths[i]; j < 6_u8; j++)
                upgrades_stack->add(
                    symbols::stop()
                    ->frame(24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f, alignment_t::center)
                );

            if(selected->upgrade_paths[i] == 6) {
                s->add((new hstack {
                    upgrades_stack,
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f),
                    symbols::forward()
                        ->offset(-20.0f, 0.0f)
                        ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center),
                    (new vstack { })
                        ->frame(200.0f, 200.0f, 200.0f, 0.0f, 0.0f, 0.0f, alignment_t::center)
                    })
                    ->padding(16.0f, 0.0f)
                    ->offset(-24.0f, 0.0f)
                    ->align(alignment_t::center));
            } else if(upgrade_count == 8) {
                s->add((new hstack {
                    upgrades_stack,
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f),
                    symbols::forward()
                        ->offset(-20.0f, 0.0f)
                        ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center),
                    (new vstack {
                        (new text("Upgrade unavailable"))
                            ->font(headline)
                            ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading)
                    })
                        ->frame(200.0f, 200.0f, 200.0f, 0.0f, 0.0f, 0.0f, alignment_t::center)
                    })
                    ->padding(16.0f, 0.0f)
                    ->offset(-24.0f, 0.0f)
                    ->align(alignment_t::center));
            } else if(selected->upgrade_paths[i] == 0) {
                double cost = tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].base_price * cs.diff.tower_cost_modifier;
                bool owned = false;

                for(auto& id : playerinfo.towers)
                    if(id == selected->id) {
                        owned = true;
                        break;
                    }

                uint32_t c = owned ? (cost > playerinfo.cash ? color::red : color::green) : color::gray;

                if(cost > playerinfo.cash && cost < update_at) update_at = cost;

                to_free.push_back(new data_t({ i, cost, owned }));

                s->add((new hstack {
                    upgrades_stack,
                    (new vstack { })
                        ->frame(200.0f, 200.0f, 200.0f, 0.0f, 0.0f, 0.0f, alignment_t::center),
                    symbols::forward()
                        ->offset(-20.0f, 0.0f)
                        ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center),
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text("$"_str + to_string_digits(cost, 2)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::center)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f)
                        ->background_color(c)
                        ->hover_background_color(color_multiply(c, 0.8))
                        ->on_click([&](view*, float, float, void* data) {
                            data_t val = *(data_t*)data;
                            if(!val.owned) return;
                            if(playerinfo.cash >= val.cost) {
                                try_upgrade(val.val);
                                last_selected = nullptr;
                            } else error.play();
                        }, to_free[to_free.size() - 1])
                    })
                    ->padding(16.0f, 0.0f)
                            ->offset(-24.0f, 0.0f)
                            ->align(alignment_t::center));
            } else {
                double cost = tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].base_price * cs.diff.tower_cost_modifier;

                bool owned = false;

                for(auto& id : playerinfo.towers)
                    if(id == selected->id) {
                        owned = true;
                        break;
                    }

                uint32_t c = owned ? (cost > playerinfo.cash ? color::red : color::green) : color::gray;

                if(cost > playerinfo.cash && cost < update_at) update_at = cost;

                to_free.push_back(new data_t({ i, cost, owned }));

                s->add((new hstack {
                    upgrades_stack,
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i] - 1].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f),
                    symbols::forward()
                        ->offset(-20.0f, 0.0f)
                        ->frame(64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, alignment_t::center),
                    (new vstack {
                        (new vstack {
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].name)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text(get_entry(tower_types[selected->base_type].upgrade_paths[i][selected->upgrade_paths[i]].desc)))
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::top_leading),
                            (new text("$"_str + to_string_digits(cost, 2)))
                                ->font(headline)
                                ->frame(168.0f, 168.0f, 168.0f, 0.0f, 0.0f, 0.0f, alignment_t::center)
                        })
                            ->offset(-16.0f, -16.0f)
                    })
                        ->padding(16.0f)
                        ->background_color(c)
                        ->hover_background_color(color_multiply(c, 0.8))
                        ->on_click([&](view*, float, float, void* data) {
                            data_t val = *(data_t*)data;
                            if(!val.owned) return;
                            if(playerinfo.cash >= val.cost) {
                                try_upgrade(val.val);
                                last_selected = nullptr;
                            } else error.play();
                        }, to_free[to_free.size() - 1])
                    })
                    ->padding(16.0f, 0.0f)
                            ->offset(-24.0f, 0.0f)
                            ->align(alignment_t::center));
            }
        }
    }

public:
    tower* last_selected = nullptr;
    game_ui() { }
    ~game_ui() { }

    void release_handler() {
        SDL_GetMouseState(&lastx, &lasty);
        lastx = (int)((float)lastx / (float)width * 1920.0f);
        lasty = (int)((float)lasty / (float)height * 1080.0f);
        send_click = true;
        clicking = false;
    }

    void press_handler() {
        clicking = true;
    }

    void update() {
        if(selected != last_selected || playerinfo.cash >= update_at) do_update();
    }

    void ui() {
        float width_scale = 1.0f / (float)width * 1920.0f;
        float height_scale = 1.0f / (float)height * 1080.0f;
        render_gui(clicking, send_click, lastx, lasty, width_scale, height_scale);
        send_click = false;
    }
} ui;
