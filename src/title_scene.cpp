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

TitleSceneMaster::TitleSceneMaster(GameData *game_data)
	: background(GameMaster::texture_table[ICHIMATSU3], sf::Vector2f(-128, -128),
		     sf::IntRect(0, 0, 1366 + 128, 768 + 128), sf::Vector2f(0.2, 0.2)),
          game_state(START)
{
        choice_label_set.emplace("Start", new  Label(L"Start", game_data->get_font(JP_DEFAULT)));
        choice_label_set.emplace("Exit", new Label(L"Exit", game_data->get_font(JP_DEFAULT)));

        selecter.add_item("Start");
	selecter.add_item("Exit");

	choice_label_set.at("Start")->set_place(WindowInformation::HALF_WIDTH, 100);
	choice_label_set.at("Exit")->set_place(WindowInformation::HALF_WIDTH, 500);
 
        choice_label_set.at("Start")->set_color(sf::Color::Black);
        choice_label_set.at("Exit")->set_color(sf::Color::Black);

	choice_label_set.at("Start")->set_font_size(28);
	choice_label_set.at("Exit")->set_font_size(28);
}

bool TitleSceneMaster::keyboard_function(void)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		selecter.down();
		choice_label_set.at(selecter.get())->set_font_size(40);
	} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		selecter.up();
		choice_label_set.at(selecter.get())->set_font_size(40);
	} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		std::string command = selecter.get();
		if (command == "Exit") {
			exit(0);
		} else if (command == "Start") {
			game_state = RACE;
		}
	}else{
                return false;
        }

        return true;
}

void TitleSceneMaster::pre_process(sf::RenderWindow &window)
{
        static bool key_disable;
        
	for (auto &[hash, label] : choice_label_set) {
                if(selecter.get() != label->get_text())
                        label->set_font_size(28);
	}

        if(!key_disable){
                if(keyboard_function())
                        key_disable = true;
        }

	if (!(get_count() % 30)) {
		key_disable = false;
	}

	background.scroll([&](sf::Vector2f init, sf::Vector2f current) {
                                  sf::Vector2f next = current;
				  if(next.x - init.x > background.displaying_size().x){
                                          return init;
                                  }
                                  next.x += 0.5;
                                  next.y += 0.5;
				  return next;
	});

	update_count();
}

void TitleSceneMaster::drawing_process(sf::RenderWindow &window)
{
        background.draw(window);
        
	for(auto &[hash, label] : choice_label_set){
		label->draw(window);
        }
}

GameState TitleSceneMaster::post_process(sf::RenderWindow &window)
{
	return game_state;
}
