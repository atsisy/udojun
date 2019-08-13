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
#include <list>
#include "animation.hpp"
#include "key_input.hpp"
#include "bullet_management.hpp"
#include "enemy_character.hpp"

class GameData;

enum GameState {
        START = 0,
        OPENING_EPISODE,
        RACE,
        END,
        SUBEVE_CONTINUE,
        SUBEVE_FINISH,
};

class SceneMaster {
private:
        u64 t;

protected:
        util::str_hash<sf::View *> views;
        util::SimpleTimerList timer_list;
	sf::View *create_view(std::string key, sf::FloatRect area);
	sf::View *get_view(std::string key);
        void switch_view(std::string key, sf::RenderWindow &window);

        void set_count_for_debug(u64 count);
        
public:
        SceneMaster();
        virtual ~SceneMaster();
        virtual void pre_process(sf::RenderWindow &window) = 0;
        virtual void drawing_process(sf::RenderWindow &window) = 0;
        virtual GameState post_process(sf::RenderWindow &window) = 0;
        u64 get_count();
        void update_count();
};

class SceneSubEvent : public SceneMaster, public SceneAnimation {
private:
        sf::Vector2f position;
        GameState current_status;

public:
        SceneSubEvent(sf::Vector2f pos);
        void set_status(GameState status);
        GameState get_status(void);

        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};


class TitleSceneMaster : public SceneMaster, public SceneAnimation {
private:
        util::str_hash<DynamicText *> choice_label_set;
	BackgroundTile background;
	util::SelecterImplements<std::string> selecter;
        GameState game_state;
        key::KeyboardListener key_listener;

        bool keyboard_function(void);
        void start_handler(void);
        

public:
        TitleSceneMaster(GameData *game_data);
        
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};

class EpisodeController {
private:
	std::vector<NovelText *> episode;
        u64 page_index;

public:
        EpisodeController(const char *path, sf::Font *font);
        void next(void);
        void back(void);
        void end(void);
        void top(void);
        bool last_page(void);
        NovelText *get_current_page(void);
};

class OpeningEpisodeSceneMaster : public SceneMaster, public SceneAnimation {
private:
	GameState game_state;
	BackgroundTile background;
	EpisodeController episode;
        key::KeyboardListener key_listener;

        void prepare_for_next_scene(void);

public:
        OpeningEpisodeSceneMaster(GameData *game_data);
        
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};

class RaceSceneMaster : public SceneMaster {

        class ConversationEvent : public SceneSubEvent {
        private:
                std::forward_list<Tachie *> tachie_container;
                EpisodeController episode;
                key::KeyboardListener key_listener;
                MoveObject background;
                
        public:
                ConversationEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data);

                void pre_process(sf::RenderWindow &window) override;
                void drawing_process(sf::RenderWindow &window) override;
                GameState post_process(sf::RenderWindow &window) override;
        };

        class SpellCardEvent : public SceneSubEvent {
        private:
                std::forward_list<Tachie *> tachie_container;
                std::forward_list<MoveObject *> objects;
                
        public:
                SpellCardEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data);

                void pre_process(sf::RenderWindow &window) override;
                void drawing_process(sf::RenderWindow &window) override;
                GameState post_process(sf::RenderWindow &window) override;
        };
        
private:
        std::forward_list<Tachie *> tachie_container;
        std::forward_list<MoveObject *> move_object_container;
        std::vector<EnemyCharacter *> enemy_container;
        util::GPContainer<SceneSubEvent *, std::list> sub_event_list;
        
        GameData *game_data;
        PlayerCharacter running_char;
        EnemyCharacter target_udon;
        BackgroundTile backgroundTile;
        BackgroundTile game_background;
        DrawableScoreCounter game_score_counter;
        DrawableScoreCounter score_counter;
	DrawableScoreCounter timelimit_counter;
        BulletFuncTable func_table;
        Meter stamina;
        Meter junko_param;
        Meter udon_hp;
        Label stamina_label;
        Label junko_param_label;
        Label rec_label;
        Label graze_label;
        Label game_score_label;
        DrawableScoreCounter graze_counter;
        WindowFrame window_frame;
	DanmakuScheduler danmaku_sched;
        u64 last_danmaku_timer_id;
        BulletPipelineContainer bullet_pipeline;
        u64 danmaku_timer_id;
        ConversationEvent *kaiwa_event;
        AbstractDanmakuSchedule abs_danmaku_sched;
        EnemyCharacterSchedule enemy_sched;

	void add_new_functional_bullets_to_schedule(void);
        void add_new_danmaku(void);
        void next_danmaku_forced(void);
        void conflict_judge(void);
        void kill_out_of_filed_bullet(std::vector<Bullet *> &bullets);
        void player_spellcard(void);
        void random_mist(void);
        void insert_enemy_spellcard(int index);

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
        EnemyCharacterTable enemy_table;

public:
        GameData();
        sf::Font *get_font(FontID id);
        EnemyCharacterMaterial *get_enemy_material(std::string name);
        
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

        SceneMaster *create_new_scene(GameState state);
        
public:
        static TextureTable texture_table;
        
        GameMaster();
        void init();
        void main_loop();
        void switch_scene(GameState res);
        void load_textures(const char *json_path);
};
