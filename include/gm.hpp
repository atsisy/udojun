#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <queue>
#include <vector>
#include <string>
#include "character.hpp"
#include "game_component.hpp"
#include "textures.hpp"
#include "move_func.hpp"
#include "programable.hpp"
#include "sched.hpp"
#include <unordered_map>

enum GameState {
        START = 0,
        RACE = 1,
        END = 2,
};

class SceneMaster {
private:
        u64 t;

protected:
        std::unordered_map<std::string, sf::View *> views;

	sf::View *create_view(std::string key, sf::FloatRect area);
	sf::View *get_view(std::string key);
        void switch_view(std::string key, sf::RenderWindow &window);
        
public:
        SceneMaster();
        virtual void pre_process(sf::RenderWindow &window) = 0;
        virtual void drawing_process(sf::RenderWindow &window) = 0;
        virtual GameState post_process(sf::RenderWindow &window) = 0;
        u64 get_count();
        void update_count();
};

class RaceSceneMaster : public SceneMaster {
private:
        PlayerCharacter running_char;
        BackgroundTile backgroundTile;
        BackgroundTile game_background;
        DrawableScoreCounter score_counter;
        std::vector<Bullet *> bullets;
        BulletFuncTable func_table;
        BulletScheduler bullets_sched;
        Meter stamina;
        Meter junko_param;
        Label stamina_label;
        Label junko_param_label;
        Label rec_label;
        WindowFrame window_frame;

public:
        RaceSceneMaster();

        void player_move();
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};

class GameMaster {
private:
        sf::RenderWindow window;
        sf::WindowHandle window_handle;

        SceneMaster *current_scene;
        RaceSceneMaster *race_scene_master;

        GameState current_state;
        
public:
        static TextureTable texture_table;
        
        GameMaster();
        void init();
        void main_loop();
        void switch_scene();
        void load_textures(const char *json_path);
};
