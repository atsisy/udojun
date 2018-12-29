#include <cmath>
#include <string>
#include <cstdlib>
#include "gm.hpp"
#include "utility.hpp"
#include "move_func.hpp"
#include "picojson.h"
#include "macro.hpp"
#include <fstream>

SceneMaster::SceneMaster()
{
        t = 0;
}

u64 SceneMaster::get_count()
{
        return t;
}

void SceneMaster::update_count()
{
        t++;
}

RaceSceneMaster::RaceSceneMaster()
        : running_char(CharacterAttribute("stick man"), GameMaster::texture_table[UDON1], sf::Vector2f(100, 100)),
          backgroundTile(GameMaster::texture_table[MOON_CITY_TILE], sf::Vector2f(32, 32)),
          score_counter(0),
          stamina(sf::Vector2f(300, 20), sf::Vector2f(2, 2), 400, 400,
                  sf::Color(10, 10, 20), sf::Color::Green, sf::Color(20, 100, 20)),
          junko_param(sf::Vector2f(300, 20), sf::Vector2f(2, 2), 0, 400,
                      sf::Color(10, 10, 20), sf::Color(203, 67, 147), sf::Color(110, 50, 50)),
          stamina_label(L"体力"),
          junko_param_label(L"純化度"),
          window_frame(sf::IntRect(0, 0, 1366, 768), sf::IntRect(32, 32, 960, 704))
{
        std::ifstream ifs("bullets_sched.json", std::ios::in);

        if (ifs.fail()) {
                std::cerr << "failed to read json file" << std::endl;
                exit(1);
        }

        const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        
        picojson::value v;
        const std::string err = picojson::parse(v, json);
        if (err.empty() == false) {
                std::cerr << err << std::endl;
                exit(1);
        }

        picojson::array& array = v.get<picojson::object>()["schedule"].get<picojson::array>();

        for(auto &array_element : array){
                picojson::object &data = array_element.get<picojson::object>();
                if(data["type"].get<std::string>() == "macro"){
                        auto &&gen = macro::expand_macro(data);        stamina.set_place(1000, 40);
                        std::copy(std::begin(gen), std::end(gen), std::back_inserter(bullets_sched));
                }else if(data["type"].get<std::string>() == "dynamic-macro"){
                        bullets_sched.push_back(new BulletData(data, DYNAMIC_MACRO));
                }else if(data["type"].get<std::string>() == "dynamic-unit"){
                        bullets_sched.push_back(new BulletData(data, AIMING_SELF));
                }else{
                        bullets_sched.push_back(new BulletData(data));
                }
        }
        
        std::sort(std::begin(bullets_sched), std::end(bullets_sched),
                  [](const BulletData *b1, const BulletData *b2){
                          return b1->appear_time < b2->appear_time;
                  });

        stamina_label.set_place(1010, 10);
        stamina.set_place(1010, 40);
        junko_param_label.set_place(1010, 70);
        junko_param.set_place(1010, 100);
}

void RaceSceneMaster::player_move()
{
        static float speed;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
                speed = 2;
        }else{
                speed = 4;
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
                running_char.move_diff(sf::Vector2f(-speed, 0));
        }       
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
                running_char.move_diff(sf::Vector2f(speed, 0));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                running_char.move_diff(sf::Vector2f(0, speed));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                running_char.move_diff(sf::Vector2f(0, -speed - 3));
                stamina.add(-2.0);
        }

        running_char.move_diff(sf::Vector2f(0, 3));
        stamina.add(1);
        
        if(get_count() % 20 == 16){
                running_char.change_textures(GameMaster::texture_table[UDON5]);
        }else if(get_count() % 20 == 12){
                running_char.change_textures(GameMaster::texture_table[UDON4]);
        }else if(get_count() % 20 == 8){
                running_char.change_textures(GameMaster::texture_table[UDON3]);                
        }else if(get_count() % 20 == 4){
                running_char.change_textures(GameMaster::texture_table[UDON2]);
        }else if(get_count() % 20 == 0){
                running_char.change_textures(GameMaster::texture_table[UDON1]);
        }
}

void RaceSceneMaster::pre_process(sf::RenderWindow &window)
{
        static util::xor128 rand;
        static  int i;

        for(; i < bullets_sched.size();i++){
                if(bullets_sched.at(i)->appear_time <= get_count()){
                        if(bullets_sched.at(i)->flags & DYNAMIC_MACRO){
                                auto &&gen = macro::expand_dynamic_macro(bullets_sched.at(i)->original_data, running_char);
                                for(auto &elem : gen){
                                        bullets.push_back(
                                                elem->generate(running_char, get_count()));
                                }
                        }else{
                                bullets.push_back(
                                        bullets_sched.at(i)->generate(running_char, get_count()));
                        }
                        
                }else{
                        break;
                }
        }

        for(u32 i = 0;i < bullets.size();i++){
                if(bullets[i]->check_conflict(running_char)){
                        bullets[i]->hide();
                        junko_param.add(80);
                }
        }

        player_move();
        
        score_counter.counter_method().add(1);

        for(u32 i = 0;i < bullets.size();i++){
                if(bullets[i]->is_finish(sf::IntRect(0, 0, 1368, 768)) || !bullets[i]->visible()){
                        delete bullets[i];
                        bullets[i] = bullets.back();
                        bullets.pop_back();
                        if(!bullets.size()){
                                break;
                        }
                }
                bullets[i]->move(get_count());
        }

        backgroundTile.scroll(4);

        update_count();
}

void RaceSceneMaster::drawing_process(sf::RenderWindow &window)
{
        backgroundTile.draw(window);

        running_char.draw(window);
        
        for(u32 i = 0;i < bullets.size();i++){
                bullets[i]->draw(window);
        }

        window_frame.draw(window);
        
        score_counter.draw(window);
        stamina.draw(window);
        junko_param.draw(window);
        stamina_label.draw(window);
        junko_param_label.draw(window);
}

GameState RaceSceneMaster::post_process(sf::RenderWindow &window)
{
        return RACE;
}
