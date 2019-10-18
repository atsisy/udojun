#include <cmath>
#include <string>
#include <cstdlib>
#include "gm.hpp"
#include "utility.hpp"
#include "move_func.hpp"
#include "laser.hpp"
#include <fstream>
#include "value.hpp"
#include "effect.hpp"
#include "rotate_func.hpp"
#include "geometry.hpp"

TitleSceneMaster::TitleSceneMaster(GameData *game_data)
	: background(GameMaster::texture_table[TITLE_BLUE], sf::Vector2f(0, 0), mf::stop, rotate::stop, 0),
          game_state(START)
{
        current_selecting_status = SS_START;
        key_listener.key_update();
        this->bgm_sound_id = GameMaster::sound_player->add(sound::SoundInformation(sound::TITLE_BGM, 50.f, true));
	choice_label_set.emplace(
		"Start",
		new DynamicText(L"Start", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(400, 300),
                                mf::stop, rotate::stop, get_count(), 40));
        choice_label_set.emplace(
		"Ranking",
		new DynamicText(L"Ranking", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(400, 350),
                                mf::stop, rotate::stop, get_count(), 28)
                );
        choice_label_set.emplace(
		"Exit",
		new DynamicText(L"Exit", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(400, 400),
                                mf::stop, rotate::stop, get_count(), 28)
                );

        ss_level_select_text.push_back(
		new DynamicText(L"狐級 Hard", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(400, 320),
                                mf::stop, rotate::stop, get_count(), 32)
                );
        ss_level_select_text.back()->set_alpha(0);
        ss_level_select_text.push_back(
		new DynamicText(L"兎級 Easy", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(400, 400),
                                mf::stop, rotate::stop, get_count(), 32)
                );
        ss_level_select_text.back()->set_alpha(0);

        selecter.add_item("Start");
        selecter.add_item("Ranking");
	selecter.add_item("Exit");

        ss_level_selecter.add_item(0);
        ss_level_selecter.add_item(1);
        
        choice_label_set.at("Start")->override_move_func(
                [](MoveObject *p, u64 current, u64 begin) {
                        auto &&init = p->get_initial_position();
                        float width = 5 * std::sin((float)(current - begin) / 20.0);
                        return sf::Vector2f(init.x, init.y + width);
		});
        choice_label_set.at("Ranking")->override_move_func(
                [](MoveObject *p, u64 current, u64 begin) {
                        auto &&init = p->get_initial_position();
                        float width = 5 * std::sin((float)(current - begin) / 20.0);
                        return sf::Vector2f(init.x, init.y + width);
		});
        choice_label_set.at("Exit")->override_move_func(
                [](MoveObject *p, u64 current, u64 begin) {
                        auto &&init = p->get_initial_position();
                        float width = 5 * std::sin((float)(current - begin) / 20.0);
                        return sf::Vector2f(init.x, init.y + width);
		});

	key_listener.add_key_event(key::ARROW_KEY_DOWN, [this](key::KeyStatus status) {
		if (status & key::KEY_FIRST_PRESSED) {
                        switch(current_selecting_status){
                        case SS_START:
                                this->selecter.down();
                        case SS_LEVEL_SELECT:
                                this->ss_level_selecter.down();
                        }
                        GameMaster::sound_player->add(sound::SELECTING_SOUND);
		}
	});
	key_listener.add_key_event(
		key::ARROW_KEY_UP, [this](key::KeyStatus status) {
			if (status & key::KEY_FIRST_PRESSED) {
                                switch(current_selecting_status){
                                case SS_START:
                                        this->selecter.up();
                                case SS_LEVEL_SELECT:
                                        this->ss_level_selecter.up();
                                }
                                GameMaster::sound_player->add(sound::SELECTING_SOUND);
			}
		});
	key_listener.add_key_event(
		key::VKEY_1, [this](key::KeyStatus status) {
                                     switch(current_selecting_status){
                                     case SS_START:
                                             ss_start_menu_handler(status);
                                             break;
                                     case SS_LEVEL_SELECT:
                                             ss_level_select_handler(status);
                                             break;
                                     default:
                                             break;
                                     }
		});
        key_listener.add_key_event(
		key::VKEY_3, [this](key::KeyStatus status) {
                                     switch(current_selecting_status){
                                     case SS_START:
                                             break;
                                     case SS_LEVEL_SELECT:
                                             current_selecting_status = SS_START;
                                             for(auto p : ss_level_select_text){
                                                     p->clear_effect_queue();
                                                     p->add_effect({ effect::fade_out(20, get_count()) });
                                             }
                                             for(auto &[hash, label] : choice_label_set){
                                                     label->clear_effect_queue();
                                                     label->add_effect({ effect::fade_in(20, get_count()) });
                                             }
                                             break;
                                     default:
                                             break;
                                     }
                             });

        sf::FloatRect scsv_rect(100, 100, 800, 300);
        effect_group.push_back(new ScreenSaver({
                                new ScreenSaverElement(
                                        GameMaster::texture_table[LOTUS_PINK],
                                        geometry::random_screen_vertex(scsv_rect),
                                        rotate::constant(0.01),
                                        get_count(),
                                        sf::Vector2f(0.3, 0.3),
                                        60,
                                        sf::Vector2f(1, 1),
                                        0),
                                new ScreenSaverElement(
                                        GameMaster::texture_table[LOTUS_BLUE],
                                        geometry::random_screen_vertex(scsv_rect),
                                        rotate::constant(0.01),
                                        get_count(),
                                        sf::Vector2f(0.3, 0.3),
                                        60,
                                        sf::Vector2f(1, 1),
                                        0),
                                new ScreenSaverElement(
                                        GameMaster::texture_table[LOTUS_YELLOW],
                                        geometry::random_screen_vertex(scsv_rect),
                                        rotate::constant(0.01),
                                        get_count(),
                                        sf::Vector2f(0.3, 0.3),
                                        60,
                                        sf::Vector2f(1, 1),
                                        0)}));
}

void TitleSceneMaster::start_handler(void)
{
	auto p = new MoveObject(GameMaster::texture_table[BLACK_ANTEN],
				sf::Vector2f(0, 0),
                                mf::stop,
                                rotate::stop,
                                get_count());
        add_animation_object(p);
        p->add_effect({ effect::fade_in(100) });
}

void TitleSceneMaster::ss_start_menu_handler(key::KeyStatus status)
{
        if (status & key::KEY_FIRST_PRESSED) {
                std::string command = selecter.get();
                if (command == "Exit") {
                        exit(0);
                } else if (command == "Start") {
                        current_selecting_status = SS_LEVEL_SELECT;
                        for(auto p : ss_level_select_text){
                                p->clear_effect_queue();
                                p->add_effect({ effect::fade_in(20, get_count()) });
                        }
                        for(auto &[hash, label] : choice_label_set){
                                label->clear_effect_queue();
                                label->add_effect({ effect::fade_out(20, get_count()) });
                        }
                } else if (command == "Ranking") {
                        start_handler();
                        timer_list.add_timer(
                                [this](void) {
                                        this->game_state =
                                                RANKING;
                                },
                                180, get_count());
                }
        }
}

void TitleSceneMaster::ss_level_select_handler(key::KeyStatus status)
{
        if (status & key::KEY_FIRST_PRESSED) {
                u64 command = ss_level_selecter.get();
                if (command == 0) {
                        /*
                         * Hard
                         */
                        if(GameMaster::sound_player->stop(bgm_sound_id) == -1){
                                std::cout << "unstopped: " << bgm_sound_id << std::endl;
                        }
                        start_handler();
                        timer_list.add_timer(
                                [this](void) {
                                        this->game_state =
                                                OPENING_EPISODE;
                                },
                                180, get_count());
                } else if (command == 1) {
                        /*
                         * Easy
                         */
                        if(GameMaster::sound_player->stop(bgm_sound_id) == -1){
                                std::cout << "unstopped: " << bgm_sound_id << std::endl;
                        }
                        start_handler();
                        timer_list.add_timer(
                                [this](void) {
                                        this->game_state =
                                                OPENING_EPISODE;
                                },
                                180, get_count());
                }
        }
}

void TitleSceneMaster::pre_process(sf::RenderWindow &window)
{
        for (auto &[hash, label] : choice_label_set) {
                if(selecter.get() != label->get_text())
                        label->set_font_size(28);
                else
                        label->set_font_size(40);

                label->move(get_count());
                label->effect(get_count());
                
        }
        for (size_t i = 0;i < ss_level_select_text.size();i++) {
                if(i != ss_level_selecter.get())
                        ss_level_select_text[i]->set_font_size(28);
                else
                        ss_level_select_text[i]->set_font_size(40);
                ss_level_select_text[i]->move(get_count());
                ss_level_select_text[i]->effect(get_count());
        }
        
        for(auto p : effect_group){
                p->effect(get_count());
        }

        key_listener.key_update();
        flush_effect_buffer(get_count());
}

void TitleSceneMaster::drawing_process(sf::RenderWindow &window)
{
        background.draw(window);

        for(auto p : effect_group){
                p->draw(window);
        }

        for(auto &[hash, label] : choice_label_set){
                label->draw(window);
        }
        for(auto text : ss_level_select_text){
                text->draw(window);
        }
        
        draw_animation(window);
}

GameState TitleSceneMaster::post_process(sf::RenderWindow &window)
{
        timer_list.check_and_call(get_count());
	update_count();
	return game_state;
}
