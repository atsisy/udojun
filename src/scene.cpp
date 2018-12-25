#include <cmath>
#include "gm.hpp"
#include "utility.hpp"

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
        : running_char(CharacterAttribute("stick man"), GameMaster::texture_table[STICK_MAN], sf::Vector2f(100, 100)),
          backgroundTile(GameMaster::texture_table[TILE_HART], sf::Vector2f(800, 0)),
          score_counter(0)
{}

void RaceSceneMaster::pre_process(sf::RenderWindow &window)
{
        static util::xor128 rand;
        
        if(!bullets.size() || rand() % 60 == 0){
                bullets.push_back(new Bullet(GameMaster::texture_table[BULLET_HART], sf::Vector2f(300 + (rand() % 200), 734),
                                             [this](sf::Vector2f &p, u64 count){
                                                     return sf::Vector2f(
                                                             p.x + (2 * std::sin(util::count_to_second(get_count(), count, 60))),
                                                             p.y - 2);
                                             }, get_count()));
        }
        
        for(u32 i = 0;i < bullets.size();i++){
                if(bullets[i]->check_conflict(running_char)){
                        bullets[i]->hide();
                }
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
                running_char.move_diff(sf::Vector2f(-2, 0));
        }
                
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                running_char.move_diff(sf::Vector2f(2, 0));
        }
        
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

        update_count();
}

void RaceSceneMaster::drawing_process(sf::RenderWindow &window)
{
        running_char.draw(window);
        score_counter.draw(window);
        backgroundTile.draw(window);
        
        for(u32 i = 0;i < bullets.size();i++){
                bullets[i]->draw(window);
        }
}

GameState RaceSceneMaster::post_process(sf::RenderWindow &window)
{
        return RACE;
}
