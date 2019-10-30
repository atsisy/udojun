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
#include "3d.hpp"
#include "sound.hpp"

class GameData;

enum GameState {
        START = 0,
        OPENING_EPISODE,
        RACE,
        END,
        SUBEVE_CONTINUE,
        SUBEVE_FINISH,
        SAVE,
        RANKING,
        CREDIT,
        RESET_CURRENT,
};

class GraphicBuffer {
private:
        sf::View buffer;
        std::priority_queue<
                DrawableComponent *,
                std::vector<DrawableComponent *>,
                std::function<bool(DrawableComponent *, DrawableComponent *)>> draw_requests;

        static bool compare_depth(DrawableComponent *a, DrawableComponent *b)
        {
                return a->get_drawing_depth() < b->get_drawing_depth();
        }

public:
        GraphicBuffer(sf::FloatRect window, sf::FloatRect viewport);
        sf::View &get_buffer(void);
        void add_draw_request(DrawableComponent *p);
        void flush_draw_requests(sf::RenderWindow &window);
};

class SceneMaster {
private:
        u64 t;
        
protected:
        util::str_hash<GraphicBuffer *> views;
        util::SimpleTimerList timer_list;
	void create_view(std::string key, sf::FloatRect area, sf::FloatRect viewport);
	GraphicBuffer *get_view(std::string key);
        void switch_view(std::string key, sf::RenderWindow &window);

        template <class List>
        void post_draw_request(std::string key, List &&buf)
        {
                GraphicBuffer *gb = get_view(key);
                std::for_each(std::begin(buf), std::end(buf),
                              [&](DrawableComponent *p){
                                      gb->add_draw_request(p);
                              });
        }

        template<class... A>
        void post_draw_request_vargs(std::string key, A... args)
        {
                GraphicBuffer *gb = get_view(key);
                
                for(DrawableComponent *p : std::initializer_list<DrawableComponent *>{args...}){
                        gb->add_draw_request(p);
                }
        }
        
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


constexpr u64 SSE_FLAG_CONTROLLABLE_MAIN = 0x01;

class SceneSubEvent : public SceneMaster, public SceneAnimation {
private:
        sf::Vector2f position;
        GameState current_status;
        std::string name;
        u64 flags;

protected:
        void up_flags(u64 flags);
        void down_flags(u64 flags);

public:
        SceneSubEvent(sf::Vector2f pos, std::string n);
        void set_status(GameState status);
        GameState get_status(void);

        std::string get_name(void);

        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;

        bool check_flags(u64 _flags);
};


class TitleSceneMaster : public SceneMaster, public SceneAnimation {

        enum SelectingStatus {
                SS_START,
                SS_LEVEL_SELECT,
        };
        
private:
        SelectingStatus current_selecting_status;
        util::str_hash<DynamicText *> choice_label_set;
	MoveObject background;
	util::SelecterImplements<std::string> selecter;
        GameState game_state;
        key::KeyboardListener key_listener;
        std::vector<EffectableGroup *> effect_group;
        i16 bgm_sound_id;

        std::vector<DynamicText *> ss_level_select_text;
        util::SelecterImplements<u64> ss_level_selecter;

        bool keyboard_function(void);
        void start_handler(void);

        void ss_start_menu_handler(key::KeyStatus status);
        void ss_level_select_handler(key::KeyStatus status);
        
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
        u64 get_index(void);
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

class SaveSceneMaster : public SceneMaster, public SceneAnimation {
private:
        std::forward_list<MoveObject *> objects;
        std::forward_list<EffectableGroup *> effect_objects;
        DynamicText *display;
        DrawableKeyboard keyboard;
        SaveData save_data;
        GameState game_state;

        void save_as_json(std::string out_file, SaveData data);
        void prepare_for_next_scene(void);

public:
        SaveSceneMaster(GameData *game_data, ScoreInformation info);
        
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};


class RankingSceneMaster : public SceneMaster, public SceneAnimation {
private:
        std::forward_list<DynamicText *> text_objects;
        std::forward_list<EffectableGroup *> effect_objects;
        
        MoveObject *background;
        GameState game_state;
        key::KeyboardListener key_listener;

        void prepare_for_next_scene(void);

public:
        RankingSceneMaster(GameData *game_data);
        
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};

class CreditSceneMaster : public SceneMaster, public SceneAnimation {
private:
        std::forward_list<DynamicText *> text_objects;
        std::forward_list<EffectableGroup *> effect_objects;
        
        MoveObject *background;
        GameState game_state;
        key::KeyboardListener key_listener;

        void prepare_for_next_scene(void);

public:
        CreditSceneMaster(GameData *game_data);
        
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;
};

class EnemyManager {
public:
        EnemyManager(void);
        MoveObject *kill_enemy_with_normal_effect(EnemyCharacter *p, u64 now);
        std::vector<MoveObject *> kill_all_enemy_with_normal_effect(
                std::forward_list<EnemyCharacter *> &p_vec,
                u64 now);
};

class DrawingManager {
private:
        std::priority_queue<
        DrawableComponent *,
        std::vector<DrawableComponent *>,
        std::function<bool(DrawableComponent *, DrawableComponent *)>> queue;

        static bool compare_depth(DrawableComponent *a, DrawableComponent *b)
        {
                return a->get_drawing_depth() < b->get_drawing_depth();
        }
        
public:
        DrawingManager(void);

        void add(DrawableComponent *p);
        void draw_and_clear(sf::RenderWindow &window);
};

class RaceSceneMaster : public SceneMaster, public SceneAnimation {

        class RaceSceneEffectController {
        public:
                bool bullet_stop: 1;
                bool bullet_force_hide: 1;
                bool enemy_stop: 1;
                bool enemy_force_hide: 1;
                bool time_limit_hide: 1;
                bool udon_marker_hide: 1;
                bool timelimit_on: 1;
                bool lock_object_move: 1;
                bool disable_bullet_conflict: 1;
                bool lock_player_spellcard: 1;

                RaceSceneEffectController(void);
        };

        class PauseEvent : public SceneSubEvent {
        private:
                std::forward_list<DynamicText *> tachie_container;
                key::KeyboardListener key_listener;
                RaceSceneMaster *rsm;
                MoveObject *background;
                std::vector<DynamicText *> choice_label_set;
                util::SelecterImplements<u64> selecter;
                
        public:
                PauseEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data);

                void pre_process(sf::RenderWindow &window) override;
                void drawing_process(sf::RenderWindow &window) override;
                GameState post_process(sf::RenderWindow &window) override;
        };

        class GameOverEvent : public SceneSubEvent {
        private:
                key::KeyboardListener key_listener;
                RaceSceneMaster *rsm;
                MoveObject *background;
                std::vector<DynamicText *> choice_label_set;
                util::SelecterImplements<u64> selecter;
                
        public:
                GameOverEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data);

                void pre_process(sf::RenderWindow &window) override;
                void drawing_process(sf::RenderWindow &window) override;
                GameState post_process(sf::RenderWindow &window) override;
        };

        class ConversationEvent : public SceneSubEvent {
        private:
                std::forward_list<Tachie *> tachie_container;
                Tachie *udon;
                EpisodeController episode;
                std::forward_list<MoveObject *> move_objects;
                key::KeyboardListener key_listener;
                MoveObject background;
                RaceSceneMaster *rsm;
                
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
                std::forward_list<MoveObject *> info_objects;
                
                MoveObject *background;
                RaceSceneMaster *rsm;
                
        public:
                SpellCardEvent(RaceSceneMaster *rsm, sf::Vector2f pos,
                               GameData *data, DanmakuCallEssential danmaku_data);

                void pre_process(sf::RenderWindow &window) override;
                void drawing_process(sf::RenderWindow &window) override;
                GameState post_process(sf::RenderWindow &window) override;
        };

        class ResultEvent : public SceneSubEvent {
        private:
                std::forward_list<MoveObject *> objects;
                RaceSceneMaster *rsm;
                
        public:
                ResultEvent(RaceSceneMaster *rsm, sf::Vector2f pos,
                            GameData *game_data, ScoreInformation score_info);

                void pre_process(sf::RenderWindow &window) override;
                void drawing_process(sf::RenderWindow &window) override;
                GameState post_process(sf::RenderWindow &window) override;
        };
        
private:
        std::forward_list<Tachie *> tachie_container;
        std::list<MoveObject *> move_object_container;
        std::forward_list<EnemyCharacter *> enemy_container;
        std::list<SceneSubEvent *> sub_event_list;
        std::list<DrawableObject3D *> object3d_list;
        std::forward_list<MoveObject *> game_info_container;
        std::vector<SHOT_MASTER_ID> killed_shot_master_id;
        
        GameData *game_data;
        PlayerCharacter running_char;
        EnemyCharacter target_udon;
        BackgroundTile backgroundTile;
        BackgroundTile game_background;
        DrawableScoreCounter<i64> game_score_counter;
        DrawableScoreCounter<i64> score_counter;
	ElapsedCounter timelimit_counter;
        DrawableScoreCounter<double> power_counter;
        BulletFuncTable func_table;
        Meter udon_hp;
        Label graze_label;
        Label game_score_label;
        Label power_label;
        Label fps_label;
        DrawableScoreCounter<i64> graze_counter;
        WindowFrame window_frame;
	DanmakuScheduler danmaku_sched;
        u64 last_danmaku_timer_id;
        BulletPipelineContainer bullet_pipeline;
        u64 danmaku_timer_id;
        AbstractDanmakuSchedule abs_danmaku_sched;
        EnemyCharacterSchedule enemy_sched;
        RaceSceneEffectController effect_conroller;
        EnemyManager enemy_manager;
        MoveObject udon_marker;
        RaceStatus race_status;
        GameState game_state;
        key::KeyboardListener key_listener;
        DrawableStackCounter life_counter;
        Label life_counter_label;
        DrawableStackCounter avail_bomb_counter;
        Label bomb_counter_label;
        i16 bgm_handler;
        
	void add_new_functional_bullets_to_schedule(void);
        void add_new_danmaku(void);
        void next_danmaku_forced(void);
        void conflict_judge(void);
        void kill_out_of_filed_bullet(std::list<Bullet *> &bullets);
        void kill_out_of_filed_laser(std::list<Laser *> &lasers);
        void check_graze(std::list<Bullet *> &bullets);
        void player_spellcard(void);
        void player_spellcard_effect(void);
        void random_mist(void);
        void insert_enemy_spellcard(int index);
        void remove_killed_shot(BulletPipeline &pipeline);
        void cleanup_enemy_container(void);
        void current_item_collect(void);
        FunctionCallEssential *player_slow_shot(void);
        FunctionCallEssential *player_fast_shot(void);
        void player_shot(void);
        void generate_items_random(ItemOrder item, sf::Vector2f origin, i64 width);
        void convert_bullet_to_small_crystal(BulletPipeline &pipeline);
        void spellcard_result(u64 elapsed_time, u64 remaining_time);
        void prepare_for_next_scene(void);
        void try_enemy_kill_check(EnemyCharacter *p);
        void udon_dead_event(void);
        void move_objects(void);
        void move_objects_sub(void);
        void pre_process_non_paused(sf::RenderWindow &window);
        void pre_process_paused(sf::RenderWindow &window);
        void running_char_hit(void);
        void game_over(void);
        void update_fps_label(void);
        void try_release_bomb_item(u64 graze, u64 count);

        void game_over_continue(void);

        void spellcard_init(void);
        void spellcard_effect_chain(std::vector<sf::Vector2f> *origin_point,
                                    std::vector<Bullet *> *catched_bullet,
                                    i16 called,
                                    u64 root_call_count, float effect_range);

    public:
        RaceSceneMaster(GameData *game_data);
        ~RaceSceneMaster(void);

        void player_move();
        void pre_process(sf::RenderWindow &window) override;
        void drawing_process(sf::RenderWindow &window) override;
        GameState post_process(sf::RenderWindow &window) override;

        ScoreInformation export_score_information(void);
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

class GameConfig {
public:
        u64 race_scene_time_offset;
        
        GameConfig(std::string json_path);
};

class FpsCalculator {
private:
        sf::Clock clock;
        float last;
        float fps;

public:
        FpsCalculator(void);
        void update();
        float get_current_fps(void);
};

class GameMaster {
private:
        sf::RenderWindow window;
        sf::WindowHandle window_handle;
        
        SceneMaster *current_scene;
        GameState current_state;
        GameData *game_data;

        u64 master_clock;

        SceneMaster *create_new_scene(GameState state);
        void init_display(void);
        
public:
        static TextureTable texture_table;
        static sound::SoundPlayer *sound_player;
        static GameConfig *game_config;
        static FpsCalculator fps_calc;
        
        GameMaster();
        void init();
        void main_loop();
        void switch_scene(GameState res);
        void load_textures(const char *json_path);
};

namespace builtin_enemy {
        std::vector<EnemyCharacterMaterial> ghost_group1(GameData *);
}

extern std::vector<std::function<std::vector<EnemyCharacterMaterial>(GameData *)>> builtin_enemy_funcs;
