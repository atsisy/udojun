#include "gm.hpp"

namespace builtin_enemy {
        std::vector<EnemyCharacterMaterial> ghost_group1(GameData *game_data)
        {
                std::vector<EnemyCharacterMaterial> ret;
                EnemyCharacterMaterial *p = game_data->get_enemy_material("GHOST_ENEMY");
                
                for(int i = 0;i < 70;i++){
                        p->time = 800 + (i * 8);
                        p->point.y = 100 + util::generate_random(0, 15);
                        ret.push_back(*p);
                }

                return ret;
        }

        std::vector<EnemyCharacterMaterial> nokogiri_ghost_group1(GameData *game_data)
        {
                std::vector<EnemyCharacterMaterial> ret;
                EnemyCharacterMaterial *p = game_data->get_enemy_material("NOKOGIRI_GHOST_ENEMY");
                
                for(int i = 0;i < 16;i++){
                        p->time = 4300 + (i * 30);
                        p->point = sf::Vector2f(100, 0);
                        ret.push_back(*p);
                }

                for(int i = 0;i < 16;i++){
                        p->time = 4300 + (i * 30);
                        p->point = sf::Vector2f(300, 0);
                        ret.push_back(*p);
                }

                for(int i = 0;i < 16;i++){
                        p->time = 4300 + (i * 30);
                        p->point = sf::Vector2f(500, 0);
                        ret.push_back(*p);
                }


                return ret;
        }

        std::vector<EnemyCharacterMaterial> ghost_group2(GameData *game_data)
        {
                std::vector<EnemyCharacterMaterial> ret;
                EnemyCharacterMaterial *a = game_data->get_enemy_material("GHOST_ENEMY4a");
                EnemyCharacterMaterial *b = game_data->get_enemy_material("GHOST_ENEMY4b");
                EnemyCharacterMaterial *c = game_data->get_enemy_material("GHOST_ENEMY5");
                
                for(int i = 0;i < 16;i++){
                        a->time = 4900 + (i * 5);
                        a->point = sf::Vector2f(0, 100 + util::generate_random(-20, 20));
                        ret.push_back(*a);
                }
                c->time = 4900;
                c->point = sf::Vector2f(700, 0);
                ret.push_back(*c);

                for(int i = 0;i < 16;i++){
                        b->time = 5200 + (i * 5);
                        b->point = sf::Vector2f(1000, 100 + util::generate_random(-20, 20));
                        ret.push_back(*b);
                }
                c->time = 5200;
                c->point = sf::Vector2f(150, 0);
                ret.push_back(*c);

                for(int i = 0;i < 16;i++){
                        a->time = 5500 + (i * 5);
                        a->point = sf::Vector2f(0, 100 + util::generate_random(-20, 20));
                        ret.push_back(*a);
                }
                c->time = 5500;
                c->point = sf::Vector2f(700, 0);
                ret.push_back(*c);

                for(int i = 0;i < 16;i++){
                        b->time = 5800 + (i * 5);
                        b->point = sf::Vector2f(1000, 100 + util::generate_random(-20, 20));
                        ret.push_back(*b);
                }
                c->time = 5800;
                c->point = sf::Vector2f(150, 0);
                ret.push_back(*c);
                
                return ret;
        }
}

std::vector<std::function<std::vector<EnemyCharacterMaterial>(GameData *)>> builtin_enemy_funcs = {
        builtin_enemy::ghost_group1,
        builtin_enemy::nokogiri_ghost_group1,
        builtin_enemy::ghost_group2
};
