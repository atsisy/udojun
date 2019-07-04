#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <queue>
#include <vector>
#include <string>
#include "character.hpp"
#include "advanced_component.hpp"
#include "textures.hpp"
#include "move_func.hpp"
#include "programable.hpp"
#include "sched.hpp"
#include "fonts.hpp"
#include <unordered_map>
#include <forward_list>

class GameData;

enum GameState {
        START = 0,
        RACE = 1,
        END = 2,
};

class SceneMaster {
private:
        u64 t;

protected:
        util::str_hash<sf::View *> views;

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

class TitleSceneMaster : public SceneMaster {
private:
        util::str_hash<Label *> choice_label_set;
	BackgroundTile background;
	util::SelecterImplements<std::string> selecter;
        GameState game_state;

        bool keyboard_function(void);

public:
        TitleSceneMaster(GameData *game_data);
        
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};

class RaceSceneMaster : public SceneMaster {
private:
        std::forward_list<Tachie *> tachie_container;
        
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
        Label graze_label;
        DrawableScoreCounter graze_counter;
        WindowFrame window_frame;

        void add_new_functional_bullets_to_schedule(void);
        void proceed_bullets_schedule(void);

public:
        RaceSceneMaster(GameData *game_data);

        void player_move();
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};


class GameData {
private:
        FontContainer *font_container;

public:
        GameData();
        sf::Font *get_font(FontID id);
        
};

class GameMaster {
private:
        sf::RenderWindow window;
        sf::WindowHandle window_handle;

        SceneMaster *current_scene;
        RaceSceneMaster *race_scene_master;
        TitleSceneMaster *title_scene_master;
        GameState current_state;
        GameData *game_data;
        
public:
        static TextureTable texture_table;
        
        GameMaster();
        void init();
        void main_loop();
        void switch_scene();
        void load_textures(const char *json_path);
};
