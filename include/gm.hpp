#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include "character.hpp"
#include "game_component.hpp"
#include "textures.hpp"

enum GameState {
        START = 0,
        RACE = 1,
        END = 2,
};

class SceneMaster {
private:
        u64 t;
        
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
        DrawableCharacter running_char;
        BackgroundTile backgroundTile;
        DrawableScoreCounter score_counter;
        std::vector<Bullet *> bullets;

public:
        RaceSceneMaster(std::string running_char_image_path);
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
        void load_textures();
};
