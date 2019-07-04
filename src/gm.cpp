#include <iostream>
#include <fstream>
#include "gm.hpp"
#include "character.hpp"
#include "picojson.h"

TextureTable GameMaster::texture_table;
util::xor128 util::generate_random;

GameData::GameData()
{
        font_container = new FontContainer("fonts.json");
}

sf::Font *GameData::get_font(FontID id)
{
        return font_container->get(id);
}

GameMaster::GameMaster()
        : window(sf::VideoMode(1366, 768), "udjn")
{
        window_handle = window.getSystemHandle();
        current_scene = nullptr;
        game_data = new GameData();
}

void GameMaster::load_textures(const char *json_path)
{
        std::ifstream ifs(json_path, std::ios::in);
        sf::Texture *t;

        std::cout << "Loading textures listed in " << json_path << "..." << std::endl;

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

        picojson::object& obj = v.get<picojson::object>();

        for(const auto &[key, val] : obj){
                t = new sf::Texture;
                std::cout << "\t" << val.to_str() << "...";
                if(!t->loadFromFile(val.to_str())){
                        exit(0);
                }
                std::cout << "done!!" << std::endl;
                t->setSmooth(true);
                texture_table.emplace(str_to_txid(key.c_str()), t);
        }

        std::cout << "All textures have been loaded." << std::endl;
}

void GameMaster::init()
{
        window.setVerticalSyncEnabled(true);
        current_state = START;

        load_textures("textures.json");

	title_scene_master = new TitleSceneMaster(this->game_data);
	race_scene_master = new RaceSceneMaster(this->game_data);
}

void GameMaster::switch_scene()
{
        switch(current_state){
        case START:
                current_scene = title_scene_master;
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
