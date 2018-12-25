#include <iostream>
#include "gm.hpp"
#include "character.hpp"

TextureTable GameMaster::texture_table;

GameMaster::GameMaster()
        : window(sf::VideoMode(1366, 768), "udjn")
{
        window_handle = window.getSystemHandle();
        current_scene = nullptr;
}

void GameMaster::load_textures()
{
        sf::Texture *t;

        t = new sf::Texture;
        if(!t->loadFromFile("bullet_hart.png")){
                exit(0);
        }
        t->setSmooth(true);
        texture_table.emplace(BULLET_HART, t);
}

void GameMaster::init()
{
        window.setVerticalSyncEnabled(true);
        current_state = RACE;

        load_textures();

        race_scene_master = new RaceSceneMaster("stick_man.png");
}

void GameMaster::switch_scene()
{
        switch(current_state){
        case START:
                break;
        case RACE:
                current_scene = race_scene_master;
                break;
        case END:
                break;
        }
}

void GameMaster::main_loop()
{       
        while(window.isOpen()){
                sf::Event event;
                while(window.pollEvent(event)){
                        if(event.type == sf::Event::Closed)
                                window.close();
                }

                switch_scene();
                
                current_scene->pre_process(window);
                
                window.clear();

                current_scene->drawing_process(window);
                
                window.display();

                current_state = current_scene->post_process(window);
        }

}
