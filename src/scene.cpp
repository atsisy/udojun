#include <cmath>
#include <string>
#include <cstdlib>
#include "gm.hpp"
#include "utility.hpp"
#include "move_func.hpp"
#include "picojson.h"
#include "laser.hpp"
#include <fstream>
#include <random>
#include "value.hpp"
#include "effect.hpp"
#include "rotate_func.hpp"
#include <sstream>
#include <iomanip>
#include <limits>
#include <codecvt>
#include <cstdio>
#include "geometry.hpp"

SceneMaster::SceneMaster()
{
	t = 0;
}

SceneMaster::~SceneMaster()
{}

void SceneMaster::create_view(std::string key, sf::FloatRect area, sf::FloatRect viewport)
{
        auto p = new GraphicBuffer(area, viewport);
        views.emplace(key, p);
}

GraphicBuffer *SceneMaster::get_view(std::string key)
{
        return views[key];
}

void SceneMaster::switch_view(std::string key, sf::RenderWindow &window)
{
        window.setView(get_view(key)->get_buffer());
}

void SceneMaster::set_count_for_debug(u64 count)
{
        t = count;
}

u64 SceneMaster::get_count()
{
	return t;
}

void SceneMaster::update_count()
{
	t++;
}

Bullet *test_bullet;
DrawableObject3D *test_3d_object;
StraightLaser *test_slaser;

RaceSceneMaster::RaceSceneMaster(GameData *game_data)
	: running_char(CharacterAttribute("stick man"),
		       GameMaster::texture_table[DOT_JUNKO],
		       GameMaster::texture_table[PLAYER_CORE],
		       sf::Vector2f(400, 200),
                       PlayableCharacterStatus(3, 3, 0)),
	  target_udon(CharacterAttribute("target udon"),
		      GameMaster::texture_table[UDON1], sf::Vector2f(480, -80),
		      sf::Vector2f(0.8, 0.8), 0, mf::stop, rotate::stop,
                      1000, 1000, false),
	  backgroundTile(GameMaster::texture_table[MOON_BACKGROUND],
			 sf::Vector2f(32, 32), sf::IntRect(0, 0, 960, 736),
			 sf::Vector2f(1, 1)),
	  game_background(GameMaster::texture_table[ICHIMATSU1],
			  sf::Vector2f(0, 0), sf::IntRect(0, 0, 1366, 768),
			  sf::Vector2f(0.2, 0.2)),
          game_score_counter(0, game_data->get_font(JP_DEFAULT), GLYPH_DESIGN1, ""),
	  score_counter(0, game_data->get_font(JP_DEFAULT), GLYPH_DESIGN1, ""),
          timelimit_counter(300, game_data->get_font(JP_DEFAULT), GLYPH_DESIGN1, "", 60),
          power_counter(0, game_data->get_font(JP_DEFAULT), GLYPH_DESIGN1, ""),
	  func_table("main.json"),
	  udon_hp(sf::Vector2f(850, 5), sf::Vector2f(1, 1), 1000, 1000,
		  sf::Color(10, 10, 20), sf::Color(213, 67, 67),
		  sf::Color(110, 50, 50)),
	  graze_label(L"グレイズ", game_data->get_font(JP_DEFAULT)),
          game_score_label(L"スコア", game_data->get_font(JP_DEFAULT)),
          power_label(L"霊力", game_data->get_font(JP_DEFAULT)),
          fps_label(""),
	  graze_counter(0, game_data->get_font(JP_DEFAULT), GLYPH_DESIGN1, ""),
	  window_frame(sf::IntRect(0, 0, 1366, 768),
		       sf::IntRect(32, 32, 960, 704)),
	  danmaku_sched({}),
          abs_danmaku_sched({ "stage1_danmaku.json" }),
          enemy_sched(game_data, { "stage1_enemy_schedule.json", "stage1_enemy_schedule2.json" }),
          udon_marker(GameMaster::texture_table[UDON_MARKER], sf::Vector2f(0, 725), mf::stop, rotate::stop, 0),
          life_counter(sf::Vector2f(90, 118), sf::Vector2f(0.12, 0.12), GameMaster::texture_table[JUNKO_HART_BULLET], 2, 0),
          life_counter_label(L"諦めない心", game_data->get_font(JP_DEFAULT)),
          avail_bomb_counter(sf::Vector2f(90, 200), sf::Vector2f(0.1, 0.1), GameMaster::texture_table[STAR_ITEM1], 2, 0),
          bomb_counter_label(L"霊撃", game_data->get_font(JP_DEFAULT))
{
        set_count_for_debug(GameMaster::game_config->race_scene_time_offset);
        
        this->game_data = game_data;
	test_bullet = new Bullet(GameMaster::texture_table[SHINREI1_TX1],
				 sf::Vector2f(400, 400),
                                 mf::random_turning(running_char.get_homing_point(), 0.12, 120, 8),
                                 0, sf::Vector2f(0.12, 0.12), BulletSize::BULLET1,
                                 true, true, 0);
        test_3d_object = new DrawableObject3D(GameMaster::texture_table[TAKE1],
                                              sf::Vector3f(-3000, 100, 150),
                                              [](DrawableObject3D *p, u64 n, u64 b){
                                                      return p->get_3d_position() + sf::Vector3f(0, 0, -1);
                                              },
                                              rotate::stop,
                                              0);
        //test_3d_object->set_scale(0.1, 0.1);
        test_slaser = new StraightLaser(GameMaster::texture_table[LASER_BODY3],
                                        sf::Vector2f(500, 100), sf::Vector2f(100, 500),
                                        [](MoveObject *p, u64 n, u64 b){
                                                p->set_scale(1 - (0.01 * (n-b)), 1);
                                                return p->get_place();
                                        },
                                        get_count());
        
        udon_hp.set_place(92, 48);
        timelimit_counter.set_place(870, 48);
        graze_label.set_place(0, 300);
        graze_counter.set_place(150, 300);
        game_score_counter.set_place(150, 250);
        game_score_label.set_place(0, 250);
        power_label.set_place(0, 350);
        fps_label.set_place(940, 711);
        fps_label.change_status(GLYPH_DESIGN1);
        fps_label.set_font_size(20);
        power_counter.set_place(150, 350);
        life_counter_label.set_place(90, 73);
        life_counter_label.set_font_size(32);
        bomb_counter_label.set_place(90, 172);
        bomb_counter_label.set_font_size(32);
        running_char.set_drawing_depth(126);
        target_udon.set_drawing_depth(126);

        create_view("background", sf::FloatRect(0.0f, 0.0f, 1366.f, 768.f), sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

	create_view("field",
		    sf::FloatRect(32.0f, 32.0f, 960.f, 736.f - (32.f)),
                    sf::FloatRect(32.f / 1366.f, 32.f / 768.f,
                                  960.f / 1366.f,
                                  702.f / 768.f));

        create_view("character",
		    sf::FloatRect(32.0f, 32.0f, 960.f, 736.f - (32.f)),
                    sf::FloatRect(32.f / 1366.f, 32.f / 768.f,
                                  960.f / 1366.f,
                                  702.f / 768.f));

        create_view("effect",
		    sf::FloatRect(32.0f, 32.0f, 960.f, 736.f - (32.f)),
                    sf::FloatRect(32.f / 1366.f, 32.f / 768.f,
                                  960.f / 1366.f,
                                  702.f / 768.f));
        
	create_view("bullets", sf::FloatRect(32.0f, 32.0f, 960.f, 768.f - (32.f * 2)),
                    sf::FloatRect(32.f / 1366.f, 32.f / 768.f, 960.f / 1366.f, 702.f / 768.f));

        create_view("conversation", sf::FloatRect(32.0f, 32.0f, 960.f, 768.f - (32.f * 2)),
                    sf::FloatRect(32.f / 1366.f, 32.f / 768.f, 960.f / 1366.f, 702.f / 768.f));

        create_view("game_info", sf::FloatRect(32.0f, 32.0f, 960.f, 768.f - (32.f * 2)),
                    sf::FloatRect(32.f / 1366.f, 32.f / 768.f, 960.f / 1366.f, 702.f / 768.f));
        
        create_view("params", sf::FloatRect(0.0f, 0.0f, 420.f, 500.f),
                    sf::FloatRect(1010.f / 1366.f, 32.f / 768.f, 420.f / 1366.f, 500.f / 768.f));
        
	create_view("tachie", sf::FloatRect(0.0f, 0.0f, 1366.f, 768.f),
                    sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

        create_view("loading",
		    sf::FloatRect(0.f, 0.f, 1366.f, 768.f),
                    sf::FloatRect(0.f, 0.f, 1.f, 1.f));

        bullet_pipeline.enemy_pipeline.add_function(
                new FunctionCallEssential("field1", 30, NO_SHOT_MASTER,
                                          sf::Vector2f(0, 0)));

        backgroundTile.set_scroll_speed(5);
        udon_marker.set_scale(0.4, 0.4);

        target_udon.add_effect({ effect::animation_effect({ UDON1, UDON2, UDON3, UDON4, UDON5 }, 5) });

        running_char.set_move_speed(4.5);

        game_state = RACE;

        for(auto fn : builtin_enemy_funcs){
                enemy_sched.push_back(fn(game_data));
        }
        
        enemy_sched.sort();

        key_listener.add_key_event(key::VKEY_3,
                                   [=](key::KeyStatus status){
                                           if(status & key::KEY_FIRST_PRESSED){
                                                   sub_event_list.push_back(new PauseEvent(this, sf::Vector2f(0, 0), game_data));
                                           }
                                   });

        key_listener.add_key_event(key::ARROW_KEY_RIGHT,
                                   [this](key::KeyStatus status){
                                           if(status & key::KEY_FIRST_PRESSED){
                                                   running_char.add_effect({
                                                                   effect::rotate_animation(10, this->get_count(),
                                                                                            running_char.get_angle(),
                                                                                            0.05) });
                                           }else if(status & key::KEY_RELEASED){
                                                   running_char.add_effect({
                                                                   effect::rotate_animation(10, this->get_count(),
                                                                                            running_char.get_angle(),
                                                                                            0) });
                                           }
                                   });
        key_listener.add_key_event(key::ARROW_KEY_LEFT,
                                   [this](key::KeyStatus status){
                                           if(status & key::KEY_FIRST_PRESSED){
                                                   running_char.add_effect({
                                                                   effect::rotate_animation(10, this->get_count(),
                                                                                            running_char.get_angle(),
                                                                                            -0.05) });
                                           }else if(status & key::KEY_RELEASED){
                                                   running_char.add_effect({
                                                                   effect::rotate_animation(10, this->get_count(),
                                                                                            running_char.get_angle(),
                                                                                            0) });
                                           }
                                   });
        
}

RaceSceneMaster::~RaceSceneMaster(void)
{
        for(auto p : tachie_container){
                delete p;
        }

        for(auto p : move_object_container){
                delete p;
        }

        for(auto p : enemy_container){
                delete p;
        }

        for(auto p : sub_event_list){
                delete p;
        }

        for(auto p : object3d_list){
                delete p;
        }

	for (auto p : game_info_container) {
		delete p;
	}
}

void RaceSceneMaster::player_spellcard_effect(void)
{
        for(int n = 0;n < 8;n++){
                for (int i = 0;i < 32;i++) {
                        timer_list.add_timer([this](){
                                                     auto p = new MoveObject(
                                                             GameMaster::texture_table[JUNKO_HART_BULLET],
                                                             sf::Vector2f(util::generate_random() % 900, util::generate_random() % 700),
                                                             mf::stop,
                                                             rotate::stop,
                                                             get_count());
                                                     p->add_effect({
                                                                     effect::scale_effect(sf::Vector2f(0.01, 0.01), sf::Vector2f(0.5, 0.5), 60),
                                                                     effect::fade_out(60),
                                                                     effect::kill_at(60)});
                                                     p->set_scale(0, 0);
                                                     move_object_container.emplace_front(p);
                                             }, 1 + (n * 5), get_count());
                }
        }
        
        
        for(EnemyCharacter *e : enemy_container){
                e->damage(400);
                try_enemy_kill_check(e);
        }
        target_udon.damage(100);
        udon_hp.set_value(target_udon.get_hp());
        if(target_udon.hp_zero()){
                udon_dead_event();
        }
}

void RaceSceneMaster::player_spellcard(void)
{
        if (std::find_if(
                    container_entire_range(tachie_container),
                    [](Tachie *p) {
                            return p->are_you("udon");
                    }) == std::end(tachie_container)){
                auto p = new Tachie(
                        GameMaster::texture_table[JUNKO_TACHIE1],
                        sf::Vector2f(500, 100),
                        mf::tachie_move_constant(4, 0),
                        rotate::stop,
                        get_count(), "udon");
                tachie_container.emplace_front(p);
                p->add_effect({ effect::fade_out(150),
                                effect::kill_at(150) });
                {
                        std::list<Bullet *> &bullets = bullet_pipeline.enemy_pipeline.actual_bullets;
                        bullets.remove_if([&, this](Bullet *b){
                                                  if (running_char.distance(b) < 800) {
                                                          bullet_pipeline.special_pipeline.direct_insert_bullet(
                                                                  new SpecialBullet(
                                                                          GameMaster::texture_table[SMALL_CRYSTAL2],
                                                                          b->get_place(),
                                                                          mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                                                          get_count(),
                                                                          sf::Vector2f(0.7, 0.7), 15,
                                                                          true, false, 0, SpecialBulletAttribute(0, 10, 0, 0)
                                                                          ));
                                                          delete b;
                                                          return true;
                                                  }else{
                                                          return false;
                                                  }
                                          });
                        avail_bomb_counter.add(get_count(), -1);
                        player_spellcard_effect();
                        
                }
        }
}

void RaceSceneMaster::add_new_danmaku(void)
{
        /*
         * まだ残りがある
         */
	if(danmaku_sched.size()){
                // 残りをスケジュールする
		bullet_pipeline.enemy_pipeline.add_function(new FunctionCallEssential(
			danmaku_sched.top().func_essential.func_name,
                        get_count() + 60, MAIN_ENEMY_SHOT));
		if (danmaku_sched.top().type == SPELL_CARD_DANMAKU) {
			sub_event_list.push_back(new SpellCardEvent(
                                                   this, sf::Vector2f(0, 0), game_data,
                                                   danmaku_sched.top()));
		}

                EnemyCharacterSchedule spell_enemy_sched = danmaku_sched.top().enemy_sched;
                while(spell_enemy_sched.size()){
                        EnemyCharacterMaterial &&elem = spell_enemy_sched.get_front();
                        elem.time += get_count();
                        enemy_sched.push_back(elem);
                        spell_enemy_sched.pop_front();
                }
                
                // スケジュールした弾幕が切れた時に新しい弾幕をスケジュール出来るように処理
                danmaku_timer_id = last_danmaku_timer_id = timer_list.add_timer(
                        [this](void) { this->next_danmaku_forced(); },
			get_count(), danmaku_sched.top().time_limit);
                
		// タイマの実行時間は、弾幕発生 + 弾幕タイムリミット
                timelimit_counter.reset_counter(danmaku_sched.top().time_limit);
                
                // 先頭の弾幕を捨てる
                danmaku_sched.drop_top();

                // タイムリミット表示
                effect_conroller.timelimit_on = true;
	}else{
                /*
                 * 全てのスペルカードを撃ち終わった
                 */
                prepare_for_next_scene();
        }
}

void RaceSceneMaster::prepare_for_next_scene(void)
{
        timer_list.add_timer(
                [this](void){
                        ScoreInformation current_score_info = export_score_information();
                        sub_event_list.push_back(new ResultEvent(
                                                         this, sf::Vector2f(0, 0), game_data,
                                                         current_score_info));
                },
                180, get_count());

        timer_list.add_timer(
                [this](void){
                        auto p = new MoveObject(GameMaster::texture_table[BLACK_ANTEN],
                                                sf::Vector2f(0, 0),
                                                mf::stop,
                                                rotate::stop,
                                                get_count());
                        add_animation_object(p);
                        p->add_effect({ effect::fade_in(100) });
                },
                500, get_count());
        timer_list.add_timer(
                [this](void){
                        this->game_state = SAVE;
                },
                620, get_count());
        effect_conroller.timelimit_on = false;
}

void RaceSceneMaster::random_mist(void)
{
        if(!(util::generate_random() % 200)){
                auto p = new MoveObject(
                        GameMaster::texture_table[MIST1],
                        sf::Vector2f(util::generate_random() % 800, -500),
                        mf::up(-4),
                        rotate::stop,
                        get_count());
                p->set_drawing_depth(180);
                move_object_container.emplace_front(p);
        }
}

void RaceSceneMaster::player_move()
{
	if (key::KeyboardTypeCoordinator::simple_pressed_check(key::MOD_VKEY_1)) {
		running_char.core_on();
		running_char.set_move_speed(1.9);
	} else {
		running_char.core_off();
                running_char.set_move_speed(4.5);
	}


        float speed = running_char.get_move_speed();
	if (key::KeyboardTypeCoordinator::simple_pressed_check(key::ARROW_KEY_LEFT)) {
		running_char.move_diff(sf::Vector2f(-speed, 0));
	}
	if (key::KeyboardTypeCoordinator::simple_pressed_check(key::ARROW_KEY_RIGHT)) {
		running_char.move_diff(sf::Vector2f(speed, 0));
	}
	if (key::KeyboardTypeCoordinator::simple_pressed_check(key::ARROW_KEY_DOWN)) {
		running_char.move_diff(sf::Vector2f(0, speed));
	}
	if (key::KeyboardTypeCoordinator::simple_pressed_check(key::ARROW_KEY_UP)) {
		running_char.move_diff(sf::Vector2f(0, -speed));
        }
	
        if(key::KeyboardTypeCoordinator::simple_pressed_check(key::VKEY_2)){
                if(avail_bomb_counter.get_count() > 0){
                        player_spellcard();
                }
        }

        running_char.effect(get_count());

        running_char.update_slaves(get_count());

        running_char.move_diff(sf::Vector2f(0, 0));

        target_udon.move_diff(sf::Vector2f(((float)(util::generate_random() % 10) / 10.0) * std::sin((float)((float)get_count() / (float)60)), 0));
        target_udon.move(get_count());
        target_udon.effect(get_count());
        sf::Vector2f udon_place = target_udon.get_place();
        udon_marker.set_place(sf::Vector2f(udon_place.x, 725));
        
        if(running_char.get_origin().y < 250){
                current_item_collect();
        }

        std::vector<MoveObject *> shinrei = running_char.shinrei_flush(get_count());
        if(shinrei.size()){
		for (auto p : shinrei) {
			move_object_container.push_front(p);
		}
                sf::Texture *t;
                u32 i = util::generate_random(0, 2);
                if(i == 0)
                        t = GameMaster::texture_table[LOTUS_PINK];
                else if(i == 1)
                        t = GameMaster::texture_table[LOTUS_BLUE];
                else
                        t = GameMaster::texture_table[LOTUS_YELLOW];
                
                bullet_pipeline.special_pipeline.direct_insert_bullet(
                        new SpecialBullet(
                                t,
                                sf::Vector2f(running_char.get_place().x, running_char.get_origin().y - 300),
                                mf::accelerating(sf::Vector2f(0, -5), sf::Vector2f(0, 0.15), sf::Vector2f(0, 0), sf::Vector2f(-5, 2)),
                                get_count(),
                                sf::Vector2f(0.06, 0.06),
                                20,
                                true,
                                false,
                                0,
                                SpecialBulletAttribute(0.0, 1500, 1, 0)
                                ));
	}
}

FunctionCallEssential *RaceSceneMaster::player_slow_shot(void)
{
        double current_power = power_counter.counter_method().get_score();
        sf::Vector2f &&p = running_char.get_place();
        
        if(current_power < 0.25){
                return new FunctionCallEssential(
                        "junko_shot_slow_lv1", get_count(), RUNNING_CHARACTER_SHOT,
                        sf::Vector2f(p.x + 10, p.y - 7));
        }else if(current_power < 0.5){
                return new FunctionCallEssential(
                        "junko_shot_slow_lv2", get_count(), RUNNING_CHARACTER_SHOT,
                        sf::Vector2f(p.x + 10, p.y - 7));
        }else{
                return new FunctionCallEssential(
                        "junko_shot_slow_lv2", get_count(), RUNNING_CHARACTER_SHOT,
                        sf::Vector2f(p.x + 10, p.y - 7));
        }

        return nullptr;
}

FunctionCallEssential *RaceSceneMaster::player_fast_shot(void)
{
        double current_power = power_counter.counter_method().get_score();
        sf::Vector2f &&p = running_char.get_place();
        
        if(current_power < 0.25){
                return new FunctionCallEssential(
                        "junko_shot_fast_lv1", get_count(), RUNNING_CHARACTER_SHOT,
                        sf::Vector2f(p.x + 10, p.y - 7));
        }else if(current_power < 0.5){
                return new FunctionCallEssential(
                        "junko_shot_fast_lv1", get_count(), RUNNING_CHARACTER_SHOT,
                        sf::Vector2f(p.x + 10, p.y - 7));
        }else{
                return new FunctionCallEssential(
                        "junko_shot_fast_lv1", get_count(), RUNNING_CHARACTER_SHOT,
                        sf::Vector2f(p.x + 10, p.y - 7));
        }

        return nullptr;
}

void RaceSceneMaster::player_shot(void)
{
        FunctionCallEssential *f;
        
        if (key::KeyboardTypeCoordinator::simple_pressed_check(key::MOD_VKEY_1)) {
                f = player_slow_shot();
        }else{
                f = player_fast_shot();
        }

        if(!f){
                DEBUG_PRINT_HERE();
                std::cerr << "FunctionCallEssential is null." << std::endl;
                return;
        }
        
        bullet_pipeline.player_pipeline.add_function(f);
}

void RaceSceneMaster::add_new_functional_bullets_to_schedule(void)
{
        if(running_char.shot_is_enable()){
                if (key::KeyboardTypeCoordinator::simple_pressed_check(key::VKEY_1)) {
                        player_shot();
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
                        bullet_pipeline.enemy_pipeline.clear_all_bullets();
                }
        }
        
        
	/*
          if(danmaku_sched.function_is_coming(get_count())){
          DanmakuCallEssential &&e = danmaku_sched.drop_top();
          func_table.add_function_dynamic(e.func_essential);
          timer_list.add_timer([this](void){
          //this->clear_all_bullets();  
          }, e.time_limit, get_count());
          }
        */

        bullet_pipeline.all_flush_called_function(get_count(), func_table);
}

void RaceSceneMaster::convert_bullet_to_small_crystal(BulletPipeline &pipeline)
{
        for (auto &&bullet : pipeline.actual_bullets) {
                if(bullet->visible() && !bullet->is_finish(sf::IntRect(0, 0, 1366, 768))){
                        bullet_pipeline.special_pipeline.direct_insert_bullet(
                                new SpecialBullet(
                                        GameMaster::texture_table[SMALL_CRYSTAL2],
                                        bullet->get_place(),
                                        mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                        get_count(),
                                        sf::Vector2f(0.7, 0.7), 7,
                                        true, false, 0, SpecialBulletAttribute(0, 10, 0, 0)
                                        ));
                }
	}
        
        pipeline.clear_all_bullets();
}

void RaceSceneMaster::spellcard_result(u64 elapsed_time, u64 remaining_time)
{
        std::ostringstream oss;
        i64 bonus_score = ((double)elapsed_time / (double)60.0) * 100000;

        oss << std::fixed << std::setprecision(4) << (double)elapsed_time / (double)60.0;
        
        // 追加得点. 消費時間
        auto text = new DynamicText(
                L"Get Spell Card Bonus!!", game_data->get_font(JP_DEFAULT),
                GLYPH_DESIGN1,
                sf::Vector2f(400, 200), mf::stop,
                rotate::stop, get_count(), 36);
        text->add_effect({
                        effect::fade_in(30),
                        effect::kill_at(150),
                        effect::fade_out_later(30, 120) });
        game_info_container.push_front(text);

        std::wstring *bonus_str = util::utf8_str_to_widechar_str(
                std::string("8000 + ") + std::to_string(bonus_score));
        auto additonal = new DynamicText(
                bonus_str->data(), game_data->get_font(JP_DEFAULT),
                GLYPH_DESIGN1,
                sf::Vector2f(500, 260), mf::stop,
                rotate::stop, get_count(), 36);
        additonal->add_effect({
                        effect::fade_in(30),
                        effect::kill_at(150),
                        effect::fade_out_later(30, 120) });
        game_info_container.push_front(additonal);

        game_score_counter.counter_method().add(bonus_score);

        auto elapsed = new DynamicText(
                util::utf8_str_to_widechar_str(std::string("撃破時間: ") + oss.str())->data(),
                game_data->get_font(JP_DEFAULT),
                GLYPH_DESIGN1,
                sf::Vector2f(500, 320), mf::stop,
                rotate::stop, get_count(), 36);
        elapsed->add_effect({
                        effect::fade_in(30),
                        effect::kill_at(150),
                        effect::fade_out_later(30, 120) });
        game_info_container.push_front(elapsed);       
}

void RaceSceneMaster::next_danmaku_forced(void)
{
        convert_bullet_to_small_crystal(bullet_pipeline.enemy_pipeline);
        
        sub_event_list.remove_if([&, this](SceneSubEvent *sse){
                                         if(sse->get_name() == "spell"){
                                                 delete sse;
                                                 auto &&bomb = enemy_manager.kill_all_enemy_with_normal_effect(
                                                         enemy_container,
                                                         get_count());
                                                 for(auto p : enemy_container){
                                                         killed_shot_master_id.push_back(p->get_shot_master_id());
                                                 }
                                                 for(auto effect : bomb){
                                                         move_object_container.push_front(effect);
                                                 }
                                                 spellcard_result(
                                                         timelimit_counter.get_elapsed(),
                                                         timelimit_counter.get_last_set());

                                                 return true;
                                         }
                                         return false;
                                 });
        
        this->target_udon.set_hp_max();
        udon_hp.set_value(target_udon.get_hp());
        // 次の弾幕を追加し、タイマも設定する
        this->add_new_danmaku();
}

void RaceSceneMaster::kill_out_of_filed_laser(std::list<Laser *> &lasers)
{
        lasers.remove_if([](Laser *p){
                                 if (!p->visible() ||
                                     (p->finish_shot() &&
                                      geometry::out_of_screen(sf::IntRect(-400, -400, 2100, 1400), p->get_tail_position()))
                                         ) {
                                          delete p;
                                          return true;
                                  }
                                  return false;
                          });
}

void RaceSceneMaster::kill_out_of_filed_bullet(std::list<Bullet *> &bullets)
{
        bullets.remove_if([this](Bullet *b){
                                  if (b->is_finish(
                                              sf::IntRect(-200, -400, 1368 + 400, 768 + 500)) ||
                                      !b->visible() |
                                      std::find(std::begin(killed_shot_master_id),
                                                std::end(killed_shot_master_id),
                                                b->get_shot_master_id()) != std::end(killed_shot_master_id)) {
                                          delete b;
                                          b = nullptr;
                                          return true;
                                  }
                                  return false;
                          });
}

void RaceSceneMaster::try_release_bomb_item(u64 graze, u64 count)
{
        if(graze % 800 == 0){
                bullet_pipeline.special_pipeline.direct_insert_bullet(
                        new SpecialBullet(
                                GameMaster::texture_table[STAR_ITEM1],
                                sf::Vector2f(running_char.get_place().x, running_char.get_origin().y - 300),
                                mf::accelerating(sf::Vector2f(0, -5), sf::Vector2f(0, 0.15), sf::Vector2f(0, 0), sf::Vector2f(-5, 2)),
                                count,
                                sf::Vector2f(0.13, 0.13),
                                20,
                                true,
                                false,
                                0,
                                SpecialBulletAttribute(0.0, 0, 0, 1)
                                ));
                std::cout << "bomb item has been released!!" << std::endl;
        }
}

void RaceSceneMaster::check_graze(std::list<Bullet *> &bullets)
{
        for (Bullet *b : bullets) {
                if (b->is_grazable()){
                        if(running_char.distance(b) > 15){
				graze_counter.counter_method().add(1);
                                try_release_bomb_item(graze_counter.counter_method().get_score(), get_count());
                                b->disable_graze();
                        }
                }
	}
}

void RaceSceneMaster::insert_enemy_spellcard(int index)
{
        u64 begin_time = get_count();
        std::vector<AbstractDanmakuData> *buf = abs_danmaku_sched.at(index++);
        for(AbstractDanmakuData &data : *buf){
                danmaku_sched.push_back(
                        DanmakuCallEssential(
                                FunctionCallEssential(data.func_name, MAIN_ENEMY_SHOT,
                                                      begin_time, sf::Vector2f(0, 0)),
                                data.time_limit,
                                data.type,
                                data.danmaku_name,
                                data.enemy_object_schedule_path,
                                this->game_data)
                        );
                begin_time += data.time_limit;
        }
}

void RaceSceneMaster::remove_killed_shot(BulletPipeline &pipeline)
{
        for(SHOT_MASTER_ID id : killed_shot_master_id){
                pipeline.clear_killed_shot(id);
        }
}

void RaceSceneMaster::cleanup_enemy_container(void)
{
        
}

void RaceSceneMaster::try_enemy_kill_check(EnemyCharacter *p)
{
        if(!p->dead() && p->hp_zero()){
                auto bomb = enemy_manager.kill_enemy_with_normal_effect(p, get_count());
                move_object_container.emplace_front(bomb);

                auto point_text = new DynamicText(L"+250", game_data->get_font(JP_DEFAULT), GLYPH_DESIGN1, p->get_place(),
                                                  mf::vector_linear(sf::Vector2f(0, 1)), rotate::stop, get_count(), 18);
                point_text->add_effect({ effect::fade_out(60), effect::kill_at(60) });
                move_object_container.emplace_front(point_text);
                game_score_counter.counter_method().add(250);

                generate_items_random(ItemOrder(2, 2), p->get_origin(), 50);
                                
                killed_shot_master_id.push_back(p->get_shot_master_id());
        }
}

void RaceSceneMaster::game_over_continue(void)
{
        life_counter.add(get_count(), 2);
        running_char.add_life(3);
        game_score_counter.counter_method().set_score(0);
}

void RaceSceneMaster::game_over(void)
{
        timer_list.add_timer([this](){
                                     sub_event_list.push_back(new GameOverEvent(this, sf::Vector2f(0, 0), game_data));
                             }, 15, get_count());
        std::cout << "game_over" << std::endl;
}

void RaceSceneMaster::running_char_hit(void)
{
        race_status.hit();
        life_counter.add(get_count(), -1);
        avail_bomb_counter.set_value(2);
        running_char.conflict();

        for(int i = 0;i < 128;i++){
                auto p = new MoveObject(GameMaster::texture_table[SHINREI1_TX1], running_char.get_origin(),
                                        mf::vector_linear(sf::Vector2f(
                                                                  util::generate_random.floating(-10, 10),
                                                                  util::generate_random.floating(-10, 10)
                                                                  )),
                                        rotate::stop,
                                        get_count());
                p->add_effect({ effect::fade_out(60) });
                p->set_scale(0.1, 0.1);
                move_object_container.push_front(p);
        }
        
        if(running_char.game_over()){
                game_over();
        }

        running_char.add_effect({ effect::flashing(5) });
        running_char.conflict_off();
        timer_list.add_timer([this](void){
                                     running_char.clear_effect_queue();
                                     running_char.set_alpha(255);
                                     running_char.conflict_on();
                             }, 180, get_count());

        GameMaster::sound_player->add(sound::SoundInformation(sound::JUNKO_HIT, 50.f, false));
}

void RaceSceneMaster::udon_dead_event(void)
{
        timer_list.cancel(last_danmaku_timer_id);
        generate_items_random(ItemOrder(24, 24), target_udon.get_origin(), 100);
        next_danmaku_forced();
}

void RaceSceneMaster::conflict_judge(void)
{
	for (auto &&bullet : bullet_pipeline.player_pipeline.actual_bullets) {
                if (bullet->visible() && !bullet->is_finish(
			    sf::IntRect(0, 0, 1368, 768))) {
                        if (bullet->check_conflict(target_udon)){
                                bullet->hide();
                                target_udon.damage(1);
                                game_score_counter.counter_method().add(3);
                                udon_hp.set_value(target_udon.get_hp());
                                // HPが0になると次の弾幕に移行
                                if(target_udon.hp_zero()){
                                        udon_dead_event();
                                }
                                continue;
                        }
                }


                for(EnemyCharacter *p : enemy_container){
                        if(p->check_conflict(*bullet)){
                                p->damage(1);
                                bullet->hide();
                                bullet_pipeline.special_pipeline.direct_insert_bullet(
                                        new SpecialBullet(
                                                GameMaster::texture_table[SMALL_CRYSTAL1],
                                                bullet->get_place(),
                                                mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                                get_count(),
                                                sf::Vector2f(0.7, 0.7), 7,
                                                true, false, 0, SpecialBulletAttribute(0, 10, 0, 0)
                                                ));
                        }
                        try_enemy_kill_check(p);
                }

                enemy_container.remove_if(
                        [](EnemyCharacter *p){
                                return !p->visible();
                        });
	}
        
        for (auto &&bullet : bullet_pipeline.enemy_pipeline.actual_bullets) {
                if (bullet->check_conflict(running_char)) {
                        bullet->hide();
                        running_char_hit();
                }
	}

        for (auto &&laser : bullet_pipeline.enemy_pipeline.actual_lasers) {
                for(auto &&bullet : laser->get_bullet_stream()){
			if (bullet->check_conflict(running_char)) {
				bullet->hide();
				race_status.hit();
			}
		}
	}
        
        for (auto &&bullet : bullet_pipeline.special_pipeline.actual_bullets) {
                SpecialBullet *sp_bullet = dynamic_cast<SpecialBullet *>(bullet);
		if (sp_bullet && sp_bullet->check_conflict_ignore_flags(running_char)) {
                        SpecialBulletAttribute &&a = sp_bullet->get_attribute();
			sp_bullet->hide();
                        game_score_counter.counter_method().add(a.score);
                        power_counter.counter_method().add(a.power);
                        running_char.add_shinrei(get_count(), a.power);
                        life_counter.add(get_count(), a.life);
                        avail_bomb_counter.add(get_count(), a.bomb);
                        running_char.add_life(a.life);
		}
	}
        
	if (test_bullet->check_conflict(running_char)) {
		puts("CONFLICT");
		test_bullet->hide();
	}
}

void RaceSceneMaster::current_item_collect(void)
{
        for(Bullet *b : bullet_pipeline.special_pipeline.actual_bullets){
                b->override_move_func(mf::active_homing(sf::Vector2f(0, 0), 10, running_char.get_homing_point()));
        }
}

void RaceSceneMaster::generate_items_random(ItemOrder item, sf::Vector2f origin, i64 width)
{
        i64 half = width >> 1;
        
        for(int i = 0;i < item.power;i++){
                auto p = new SpecialBullet(
                        GameMaster::texture_table[POWER_PANEL],
                        origin + sf::Vector2f((util::generate_random() % width) - half, (util::generate_random() % width) - half),
                        mf::accelerating(sf::Vector2f(0, -5), sf::Vector2f(0, 0.15), sf::Vector2f(0, 0), sf::Vector2f(-5, 2)),
                        get_count() + (util::generate_random() % 5),
                        sf::Vector2f(0.04, 0.04), BulletSize::SPECIAL_PANEL,
                        true, false, 0, SpecialBulletAttribute(0.03, 0, 0, 0)
                        );
                p->set_shot_master_id(SPECIAL_ITEM);
                bullet_pipeline.special_pipeline.direct_insert_bullet(p);
        }

        for(int i = 0;i < item.score;i++){
                auto p = new SpecialBullet(
                        GameMaster::texture_table[SCORE_PANEL],
                        origin + sf::Vector2f((util::generate_random() % width) - half, (util::generate_random() % width) - half),
                        mf::accelerating(sf::Vector2f(0, -5), sf::Vector2f(0, 0.15), sf::Vector2f(0, 0), sf::Vector2f(-5, 2)),
                        get_count() + (util::generate_random() % 5),
                        sf::Vector2f(0.04, 0.04), BulletSize::SPECIAL_PANEL,
                        true, false, 0, SpecialBulletAttribute(0, 10, 0, 0)


                        
                        );
                p->set_shot_master_id(SPECIAL_ITEM);
                bullet_pipeline.special_pipeline.direct_insert_bullet(p);
        }
}

void RaceSceneMaster::move_objects_sub(void)
{
        for(DrawableObject3D *p : object3d_list){
                p->move(get_count());
                p->effect(get_count());
        }
        
        // 立ち絵を移動
        tachie_container.remove_if(
                [this](Tachie *p) {
                        if(p->visible()){
                                p->move(get_count());
                                p->effect(get_count());
                                return false;
                        }else{
                                return true;
                        }
                });

        
        for(auto &&p : enemy_container){
                p->move(get_count());
                p->effect(get_count());
                p->rotate_with_func(get_count());
	}

        move_object_container.remove_if(
                [this](MoveObject *p){
                        if(p->out_of_screen(sf::IntRect(-100, -100, 1466, 868))){
                                delete p;
                                return true;
                        }
                        if(p->visible()){
                                p->move(get_count());
                                p->effect(get_count());
                                return false;
                        }else{
                                delete p;
                                return true;
                        }
                });
        game_info_container.remove_if(
                [this](MoveObject *p){
                        if(p->visible()){
                                p->move(get_count());
                                p->effect(get_count());
                                return false;
                        }else{
                                return true;
                        }
                });

        bullet_pipeline.player_pipeline.move_all_bullets(get_count());
        bullet_pipeline.special_pipeline.move_all_bullets(get_count());
        bullet_pipeline.enemy_pipeline.move_all_bullets(get_count());

        player_move();

        test_bullet->move(get_count());
        test_3d_object->move(get_count());
}

void RaceSceneMaster::move_objects(void)
{
        move_objects_sub();
}

void RaceSceneMaster::update_fps_label(void)
{
        if(get_count() % 40 == 0){
                static char label_buf[16];
                std::sprintf(label_buf, "%.1f", GameMaster::fps_calc.get_current_fps() + 0.5);
                fps_label.set_text(label_buf);
        }
}

void RaceSceneMaster::pre_process_non_paused(sf::RenderWindow &window)
{        
        key_listener.key_update();
        
        add_new_functional_bullets_to_schedule();
        bullet_pipeline.all_schedule_bullet(get_count(), running_char, target_udon);

        random_mist();
        
        conflict_judge();
        
        test_slaser->update_scale(get_count());

        for(auto p : enemy_container){
                std::optional<FunctionCallEssential> e = p->shot(get_count());
                if(e){
                        bullet_pipeline.enemy_pipeline.add_function(
                                new FunctionCallEssential(
                                        e.value().func_name,
                                        get_count(),
                                        e.value().shot_master_id,
                                        p->get_place()));
                }
        }

        if(get_count() == 20){
                bullet_pipeline.enemy_pipeline.add_function(
			new FunctionCallEssential("ellipse", get_count(), NO_SHOT_MASTER,
                                                  sf::Vector2f(0, 0)));
        }

        if(get_count() % 20 == 0){
                DrawableObject3D *p = new DrawableObject3D(GameMaster::texture_table[TAKE1],
                                                           sf::Vector3f(-3000, 5000, 150),
                                                           [](DrawableObject3D *p, u64 n, u64 b){
                                                                   return p->get_3d_position() + sf::Vector3f(0, 0, -1);
                                                           },
                                                           rotate::stop,
                                                           get_count());
                p->add_effect({ effect::fade_in(40) });
                object3d_list.push_back(p);

                p = new DrawableObject3D(GameMaster::texture_table[TAKE1],
                                         sf::Vector3f(9000, 5000, 150),
                                         [](DrawableObject3D *p, u64 n, u64 b){
                                                 return p->get_3d_position() + sf::Vector3f(0, 0, -1);
                                         },
                                         rotate::stop,
                                         get_count());
                p->add_effect({ effect::fade_in(40) });
                object3d_list.push_back(p);
        }

	score_counter.counter_method().add(1);
        timelimit_counter.counter_method().add(-1);

        check_graze(bullet_pipeline.enemy_pipeline.actual_bullets);
        
        kill_out_of_filed_bullet(bullet_pipeline.player_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.enemy_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.special_pipeline.actual_bullets);

        kill_out_of_filed_laser(bullet_pipeline.player_pipeline.actual_lasers);
        kill_out_of_filed_laser(bullet_pipeline.enemy_pipeline.actual_lasers);
        kill_out_of_filed_laser(bullet_pipeline.special_pipeline.actual_lasers);
        
        remove_killed_shot(bullet_pipeline.enemy_pipeline);
        killed_shot_master_id.clear();

        update_fps_label();

        for (Bullet *b : bullet_pipeline.special_pipeline.actual_bullets) {
                if(b->get_shot_master_id() == SPECIAL_ITEM && running_char.outer_distance(b) < b->get_radius() + 100){
                        b->override_move_func(mf::active_homing(running_char.get_origin(), 5.5, running_char.get_homing_point()));
                }       
	}

        while(enemy_sched.size()){
		if (enemy_sched.get_front().time == get_count()) {
			enemy_container.push_front(new EnemyCharacter(
                                                           enemy_sched.get_front(), get_count()));
			enemy_sched.pop_front();
		}else{
                        break;
                }
	}

        if(get_count() == 8500){
                timer_list.cancel(danmaku_timer_id);
                convert_bullet_to_small_crystal(bullet_pipeline.enemy_pipeline);
                this->target_udon.set_hp_max();
                target_udon.override_move_func(mf::move_point_constant(sf::Vector2f(480, 50),
                                                                       target_udon.get_place(), 8500, 8628));
                backgroundTile.set_scroll_speed(-1);
                timer_list.add_timer(
                        [&](void){
                                sub_event_list.push_back(new ConversationEvent(this, sf::Vector2f(0, 0), game_data));
                        }, 240, get_count());
                effect_conroller.udon_marker_hide = false;
        }

        move_objects();

	timer_list.check_and_call(get_count());
        
        for(SceneSubEvent *sse : sub_event_list){
                sse->pre_process(window);
        }

        flush_effect_buffer(get_count());
        
	update_count();
}

void RaceSceneMaster::pre_process_paused(sf::RenderWindow &window)
{
        for(SceneSubEvent *sse : sub_event_list){
                if(sse->check_flags(SSE_FLAG_CONTROLLABLE_MAIN)){
                        sse->pre_process(window);
                }
        }
}

void RaceSceneMaster::pre_process(sf::RenderWindow &window)
{
        if(unlikely(effect_conroller.lock_object_move)){
                /*
                 * ポーズ状態の時の処理
                 */
                pre_process_paused(window);
        }else{
                /*
                 * 通常時の処理
                 */
                pre_process_non_paused(window);
        }
}

void RaceSceneMaster::drawing_process(sf::RenderWindow &window)
{
        /*
         * それぞれのViewに個別に書き込むことが出来る
         * それを利用して、背景とゲーム画面を別々に処理させる
         */
        /*
	sf::View background_view(sf::FloatRect(0.0f, 0.0f, 1366.f, 768.f));
        sf::View game_view(sf::FloatRect(32.0f, 32.0f, 1366.f, 768.f - (32.f * 2)));

        background_view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	game_view.setViewport(sf::FloatRect(32.f / 1366.f, 32.f / 768.f, 0.9f, 0.9f));
        */
        post_draw_request_vargs("background", &game_background);
        
	if (!effect_conroller.udon_marker_hide) {
                post_draw_request_vargs("background", &udon_marker);
	}
        
        post_draw_request_vargs("field", &backgroundTile);
        post_draw_request("field", object3d_list);
        
        for(SceneSubEvent *sse : sub_event_list){
                sse->drawing_process(window);
        }
        
        post_draw_request_vargs("character", &running_char, &target_udon);

        post_draw_request("bullets", move_object_container);
        post_draw_request("game_info", game_info_container);
        post_draw_request("character", enemy_container);

        post_draw_request_vargs("bullets",
                                &bullet_pipeline,
                                test_bullet,
                                test_3d_object);

	//window_frame.draw

        post_draw_request_vargs("params",
                                &game_score_counter,
                                &game_score_label,
                                &graze_counter,
                                &graze_label,
                                &power_label,
                                &power_counter,
                                &score_counter,
                                &life_counter,
                                &life_counter_label,
                                &avail_bomb_counter,
                                &bomb_counter_label
                );

        post_draw_request_vargs("game_info",
                                &udon_hp,
                                &score_counter,
                                &fps_label);
        if(effect_conroller.timelimit_on){
                post_draw_request_vargs("game_info",
                                        &timelimit_counter);
        }

        // 立ち絵の移動
        post_draw_request("tachie", tachie_container);

        /*
         * それぞれのviewに貯められたリクエストを全て処理する
         */
        switch_view("background", window);
        get_view("background")->flush_draw_requests(window);

        switch_view("field", window);
        get_view("field")->flush_draw_requests(window);

        switch_view("effect", window);
        get_view("effect")->flush_draw_requests(window);

        switch_view("character", window);
        get_view("character")->flush_draw_requests(window);
        
        switch_view("bullets", window);
        get_view("bullets")->flush_draw_requests(window);

        switch_view("tachie", window);
        get_view("tachie")->flush_draw_requests(window);
        
        switch_view("conversation", window);
        get_view("conversation")->flush_draw_requests(window);

        switch_view("game_info", window);
        get_view("game_info")->flush_draw_requests(window);

        switch_view("params", window);
        get_view("params")->flush_draw_requests(window);

        switch_view("loading", window);
        draw_animation(window);
}

GameState RaceSceneMaster::post_process(sf::RenderWindow &window)
{
        sub_event_list
                .remove_if([&](SceneSubEvent *sse)
                                   { return sse->post_process(window) == SUBEVE_FINISH; });

	return game_state;
}

ScoreInformation RaceSceneMaster::export_score_information(void)
{
        return  ScoreInformation(power_counter.counter_method().get_score(),
                                 game_score_counter.counter_method().get_score(),
                                 graze_counter.counter_method().get_score(),
                                 race_status.get_hit_count());
}

RaceSceneMaster::ConversationEvent::ConversationEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data)
        : SceneSubEvent(pos, "conv"), episode("stage1_spell.txt", data->get_font(JP_DEFAULT)),
          background(GameMaster::texture_table[SAMPLE_BACKGROUND1], sf::Vector2f(0, 450), mf::stop, rotate::stop, 0)
{
        set_status(SUBEVE_CONTINUE);

        this->rsm = rsm;
        
        auto udon = new Tachie(
                GameMaster::texture_table[UDON_TACHIE],
                sf::Vector2f(600, 0),
                mf::stop,
                rotate::stop,
                get_count(), "udon");
        udon->set_scale(0.55, 0.55);
        udon->add_effect({ effect::fade_in(30) });

        auto dtext = new DynamicText(L"地上の月兎", data->get_font(JP_DEFAULT), GLYPH_DESIGN1,
                                     sf::Vector2f(600, 550),
                                     mf::ratio_two_step(sf::Vector2f(600, 550), sf::Vector2f(630, 550), sf::Vector2f(660, 550), 100, 0.1),
                                     rotate::stop,
                                     get_count(),
                                     30);
        dtext->add_effect({ effect::fade_in(15), effect::fade_out_later(15, 110) });
        move_objects.push_front(dtext);
        auto dtext2 = new DynamicText(L"鈴仙・優曇華院・イナバ", data->get_font(JP_DEFAULT), GLYPH_DESIGN1,
                                     sf::Vector2f(600, 590),
                                      mf::ratio_two_step(sf::Vector2f(600, 590), sf::Vector2f(630, 590), sf::Vector2f(660, 590), 100, 0.1),
                                     rotate::stop,
                                     get_count(),
                                     30);
        dtext2->add_effect({ effect::fade_in(15), effect::fade_out_later(15, 110) });
        move_objects.push_front(dtext2);
        
        auto junko = new Tachie(
                GameMaster::texture_table[JUNKO_TACHIE1],
                sf::Vector2f(20, 70),
                mf::stop,
                rotate::stop,
                get_count(), "junko");
        junko->set_scale(0.5, 0.5);
        junko->add_effect({ effect::fade_in(30) });
        
        tachie_container.emplace_front(udon);
        tachie_container.emplace_front(junko);

        key_listener
                .add_key_event(key::VKEY_1,
                               [=](key::KeyStatus status){
                                       if(status & key::KEY_FIRST_PRESSED){
                                               if(episode.last_page()){
                                                       rsm->insert_enemy_spellcard(0);
                                                       rsm->target_udon.damage_on();
                                                       set_status(SUBEVE_FINISH);
                                                       rsm->add_new_danmaku();
                                                       rsm->running_char.shot_on();
                                               }else{
                                                       episode.next();
                                               }
                                       }
                               });
        background.set_alpha(200);

        rsm->running_char.shot_off();
}

void RaceSceneMaster::ConversationEvent::pre_process(sf::RenderWindow &window)
{
        key_listener.key_update();
        
        for(auto &&p : tachie_container){
                p->move(get_count());
		p->effect(get_count());
	}

        for(auto &&p : move_objects){
                p->move(get_count());
		p->effect(get_count());
	}

        update_count();
}

void RaceSceneMaster::ConversationEvent::drawing_process(sf::RenderWindow &window)
{
        rsm->post_draw_request_vargs("conversation", &background);
        rsm->post_draw_request("tachie", tachie_container);
        rsm->post_draw_request_vargs("conversation", episode.get_current_page());
        rsm->post_draw_request("game_info", move_objects);
}

GameState RaceSceneMaster::ConversationEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}

RaceSceneMaster::SpellCardEvent::SpellCardEvent(RaceSceneMaster *rsm, sf::Vector2f pos,
                                                GameData *data, DanmakuCallEssential danmaku_data)
        : SceneSubEvent(pos, "spell")
{
        set_status(SUBEVE_CONTINUE);

        this->rsm = rsm;
        
        auto udon = new Tachie(
                GameMaster::texture_table[UDON_TACHIE],
                sf::Vector2f(500, 70),
                mf::vector_linear(sf::Vector2f(1, -2)),
                rotate::stop,
                get_count(), "udon");
        udon->set_scale(0.55, 0.55);
        udon->add_effect({ effect::fade_out(120) });
        udon->set_drawing_depth(125);

        tachie_container.push_front(udon);

        for(int i = 0;i < 5;i++){
                auto p = new MoveObject(GameMaster::texture_table[SPELL_CARD_ATTACK],
                                        sf::Vector2f(700, 196 + (90 * i)),
                                        mf::vector_linear(sf::Vector2f(-2, -1)),
                                        rotate::stop,
                                        get_count());
                p->rotate(std::atan(-0.5));
                p->set_scale(sf::Vector2f(0.5, 0.5));
                p->set_alpha(128);
                p->add_effect({effect::fade_out(150)});
                p->set_drawing_depth(127);
                objects.push_front(p);
        }

        for(int i = 0;i < 4;i++){
                auto p = new MoveObject(GameMaster::texture_table[SPELL_CARD_ATTACK],
                                        sf::Vector2f(-256, 720 + (90 * i)),
                                        mf::vector_linear(sf::Vector2f(2, 1)),
                                        rotate::stop,
                                        get_count());
                p->rotate(std::atan(-0.5));
                p->set_scale(sf::Vector2f(0.5, 0.5));
                p->set_alpha(128);
                p->add_effect({effect::fade_out(150)});
                p->set_drawing_depth(127);
                objects.push_front(p);
        }
        timer_list.add_timer([this](void){ this->set_status(SUBEVE_FINISH); }, danmaku_data.time_limit);

        auto hexagram = new MoveObject(GameMaster::texture_table[HEXAGRAM_BLUE],
                                       sf::Vector2f(200, 50),
                                       mf::same_position(&rsm->target_udon),
                                       rotate::constant(0.05),
                                       get_count());
        hexagram->set_scale(1, 1);
        hexagram->set_default_origin();
        hexagram->add_effect({ effect::fade_in(30) });
        hexagram->set_drawing_depth(127);
        hexagram->add_effect(
                { [](MoveObject *p, u64 now, u64 begin)
                                     {
                                             u64 past = now - begin;
                                             p->set_scale(
                                                     (std::sin((float)past / 40.f) * 0.15) + 1.1,
                                                     (std::sin((float)past / 40.f) * 0.15) + 1.1);
                                     } });
        objects.push_front(hexagram);

        if(danmaku_data.type == SPELL_CARD_DANMAKU){
                float x = 980 - (24 * util::wstrlen(danmaku_data.danmaku_name->data()));
                auto text = new DynamicText(
                        danmaku_data.danmaku_name->data(), data->get_font(JP_DEFAULT),
                        GLYPH_DESIGN1,
                        sf::Vector2f(x, 600), mf::ratio_step(sf::Vector2f(x, 80), 0.1),
                        rotate::stop, 0, 24);
                text->set_drawing_depth(123);
                info_objects.push_front(text);
        }
        
        background = new MoveObject(GameMaster::texture_table[BACKGROUND1],
                                         sf::Vector2f(0, 0),
                                         mf::stop,
                                         rotate::stop,
                                         get_count());

        background->set_repeat_flag(true);
        background->add_effect({ effect::fade_in(20) });
        background->set_alpha(250);
        background->override_move_func([](MoveObject *p, u64 now, u64 begin){
                                               p->move_sprite(sf::Vector2f(now - begin, 0));
                                               return p->get_place();
                                       });
        background->set_drawing_depth(128);
        
}

void RaceSceneMaster::SpellCardEvent::pre_process(sf::RenderWindow &window)
{
        for(auto &&p : tachie_container){
                p->move(get_count());
		p->effect(get_count());
        }

        for(auto &&p : objects){
                p->effect(get_count());
                p->rotate_with_func(get_count());
                if(p->visible())
                        p->move(get_count());
	}

        for(auto &&p : info_objects){
                p->effect(get_count());
                p->rotate_with_func(get_count());
                if(p->visible())
                        p->move(get_count());
	}

        background->move_sprite(sf::Vector2f(1, 0));
        
        timer_list.check_and_call(get_count());
        update_count();
}

void RaceSceneMaster::SpellCardEvent::drawing_process(sf::RenderWindow &window)
{
        rsm->post_draw_request_vargs("effect", background);

        for(auto &&p : objects){
                if(p->visible())
                        rsm->post_draw_request_vargs("effect", p);
	}

        for(auto &&p : info_objects){
                if(p->visible())
                        rsm->post_draw_request_vargs("game_info", p);
	}
        
        for(auto &&p : tachie_container){
                if(p->visible())
                        rsm->post_draw_request_vargs("tachie", p);
	}        
}

GameState RaceSceneMaster::SpellCardEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}

RaceSceneMaster::RaceSceneEffectController::RaceSceneEffectController(void)
{
        bullet_stop = false;
        bullet_force_hide = false;
        enemy_stop = false;
        enemy_force_hide = false;
        time_limit_hide = true;
        udon_marker_hide = true;
        timelimit_on = false;
        lock_object_move = false;
}

RaceSceneMaster::ResultEvent::ResultEvent(RaceSceneMaster *rsm, sf::Vector2f pos,
                                          GameData *game_data, ScoreInformation score_info)
        : SceneSubEvent(pos, "result")
{
        set_status(SUBEVE_CONTINUE);
        
        this->rsm = rsm;
        
        objects.push_front(
                new DynamicText(
                        util::utf8_str_to_widechar_str(
                                std::string("得点: ")
                                +
                                std::to_string(score_info.score.get_current()))->data(),
                        game_data->get_font(JP_DEFAULT),
                        GLYPH_DESIGN2,
                        sf::Vector2f(400, 500),
                        mf::ratio_step(sf::Vector2f(300, 500), 0.1),
                        rotate::stop, get_count(), 28));
        
        objects.push_front(new DynamicText(
                                   util::utf8_str_to_widechar_str(
                                           std::string("グレイズ: ")
                                           +
                                           std::to_string(score_info.graze.get_current()))->data(),
                                   game_data->get_font(JP_DEFAULT),
                                   GLYPH_DESIGN2,
                                   sf::Vector2f(400, 550),
                                   mf::ratio_step(sf::Vector2f(300, 550), 0.1),
                                   rotate::stop, get_count(), 28));
        
        objects.push_front(new DynamicText(
                                   util::utf8_str_to_widechar_str(
                                           std::string("被弾数: ")
                                           +
                                           std::to_string(score_info.hit.get_current()))->data(),
                                   game_data->get_font(JP_DEFAULT),
                                   GLYPH_DESIGN2,
                                   sf::Vector2f(400, 600),
                                   mf::ratio_step(sf::Vector2f(300, 600), 0.1),
                                   rotate::stop, get_count(), 28));
}

void RaceSceneMaster::ResultEvent::pre_process(sf::RenderWindow &window)
{
        for(auto &&p : objects){
                p->effect(get_count());
                p->rotate_with_func(get_count());
                if(p->visible())
                        p->move(get_count());
	}
        
        timer_list.check_and_call(get_count());
        update_count();
}

void RaceSceneMaster::ResultEvent::drawing_process(sf::RenderWindow &window)
{
        for(auto &&p : objects){
                if(p->visible())
                        rsm->post_draw_request_vargs("game_info", p);
	}
}

GameState RaceSceneMaster::ResultEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}

RaceSceneMaster::PauseEvent::PauseEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *game_data)
        : SceneSubEvent(pos, "pause")
{
        set_status(SUBEVE_CONTINUE);
        up_flags(SSE_FLAG_CONTROLLABLE_MAIN);
        
        this->rsm = rsm;

        rsm->effect_conroller.lock_object_move = true;
        std::cout << "RaceSceneMaster is paused. Press VKEY_3 to restart." << std::endl;

        background = new MoveObject(GameMaster::texture_table[BLACK_ANTEN], sf::Vector2f(0, 0),
                                    mf::stop,
                                    rotate::stop,
                                    0);
        background->set_alpha(0);
        background->add_effect({ effect::alpha_animation(10, 0, 0, 40) });

        key_listener.key_update();
        key_listener.add_key_event(key::VKEY_3,
                                   [&, this](key::KeyStatus status) {
                                           if (status & key::KEY_FIRST_PRESSED) {
                                                   std::cout << "RaceSceneMaster is unpaused." << std::endl;
                                                   this->rsm->effect_conroller.lock_object_move = false;
                                                   set_status(SUBEVE_FINISH);
                                           }
                                   });
        key_listener.add_key_event(key::ARROW_KEY_DOWN,
                                   [this](key::KeyStatus status) {
                                           if (status & key::KEY_FIRST_PRESSED) {
                                                   this->selecter.down();
                                                   GameMaster::sound_player->add(sound::SELECTING_SOUND);
                                           }
                                   });
	key_listener.add_key_event(
		key::ARROW_KEY_UP,
                [this](key::KeyStatus status) {
                        if (status & key::KEY_FIRST_PRESSED) {
                                this->selecter.up();
                                GameMaster::sound_player->add(sound::SELECTING_SOUND);
                        }
                });
        
        choice_label_set.push_back(
		new DynamicText(L"再開", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(350, 300),
                                mf::ratio_step(sf::Vector2f(300, 300), 0.1),
                                rotate::stop, get_count(), 40));
        choice_label_set.push_back(
		new DynamicText(L"最初からやり直す", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(350, 350),
                                mf::ratio_step(sf::Vector2f(300, 350), 0.1),
                                rotate::stop, get_count(), 28)
                );
        choice_label_set.push_back(
		new DynamicText(L"タイトルに戻る", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(350, 400),
                                mf::ratio_step(sf::Vector2f(300, 400), 0.1),
                                rotate::stop, get_count(), 28)
                );
        
        selecter.add_item(0);
        selecter.add_item(1);
	selecter.add_item(2);

        key_listener.add_key_event(
		key::VKEY_1, [this](key::KeyStatus status) {
                                     if (status & key::KEY_FIRST_PRESSED) {
                                             u64 index = selecter.get();
                                             
                                             switch(index){
                                             case 0:
                                                     this->set_status(SUBEVE_FINISH);
                                                     this->rsm->effect_conroller.lock_object_move = false;
                                                     break;
                                             case 1:
                                                     this->rsm->game_state = RESET_CURRENT;
                                                     break;
                                             case 2:
                                                     this->rsm->game_state = START;
                                                     break;
                                             }
                                     }
                             });
}

void RaceSceneMaster::PauseEvent::pre_process(sf::RenderWindow &window)
{
        for (size_t i = 0;i < choice_label_set.size();i++) {
                if(i != selecter.get())
                        choice_label_set[i]->set_font_size(28);
                else
			choice_label_set[i]->set_font_size(40);
                choice_label_set[i]->move(get_count());
                choice_label_set[i]->effect(get_count());
	}

        background->effect(get_count());
        
        key_listener.key_update();
        timer_list.check_and_call(get_count());
        update_count();
}

void RaceSceneMaster::PauseEvent::drawing_process(sf::RenderWindow &window)
{
        rsm->post_draw_request("game_info", choice_label_set);
        rsm->post_draw_request_vargs("conversation", background);
}

GameState RaceSceneMaster::PauseEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}

RaceSceneMaster::GameOverEvent::GameOverEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *game_data)
        : SceneSubEvent(pos, "game_over")
{
        set_status(SUBEVE_CONTINUE);
        up_flags(SSE_FLAG_CONTROLLABLE_MAIN);
        
        this->rsm = rsm;

        rsm->effect_conroller.lock_object_move = true;
        std::cout << "RaceSceneMaster is paused. Press VKEY_3 to restart." << std::endl;

        key_listener.key_update();
        
        background = new MoveObject(GameMaster::texture_table[BLACK_ANTEN], sf::Vector2f(0, 0),
                                    mf::stop,
                                    rotate::stop,
                                    0);
        background->set_alpha(0);
        background->add_effect({ effect::alpha_animation(10, 0, 0, 40) });
        
        key_listener.key_update();
        key_listener.add_key_event(key::VKEY_3,
                                   [&, this](key::KeyStatus status) {
                                           if (status & key::KEY_FIRST_PRESSED) {
                                                   std::cout << "RaceSceneMaster is unpaused." << std::endl;
                                                   this->rsm->effect_conroller.lock_object_move = false;
                                                   set_status(SUBEVE_FINISH);
                                           }
                                   });
        key_listener.add_key_event(key::ARROW_KEY_DOWN,
                                   [this](key::KeyStatus status) {
                                           if (status & key::KEY_FIRST_PRESSED) {
                                                   this->selecter.down();
                                                   GameMaster::sound_player->add(sound::SELECTING_SOUND);
                                           }
                                   });
	key_listener.add_key_event(
		key::ARROW_KEY_UP,
                [this](key::KeyStatus status) {
                        if (status & key::KEY_FIRST_PRESSED) {
                                this->selecter.up();
                                GameMaster::sound_player->add(sound::SELECTING_SOUND);
                        }
                });
        
        choice_label_set.push_back(
		new DynamicText(L"諦めない", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(350, 300),
                                mf::ratio_step(sf::Vector2f(300, 300), 0.1),
                                rotate::stop, get_count(), 40));
        choice_label_set.push_back(
		new DynamicText(L"最初からやり直す", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(350, 350),
                                mf::ratio_step(sf::Vector2f(300, 350), 0.1),
                                rotate::stop, get_count(), 28)
                );
        choice_label_set.push_back(
		new DynamicText(L"タイトルに戻る", game_data->get_font(JP_DEFAULT),
                                GLYPH_DESIGN1,
				sf::Vector2f(350, 400),
                                mf::ratio_step(sf::Vector2f(300, 400), 0.1),
                                rotate::stop, get_count(), 28)
                );
        
        selecter.add_item(0);
        selecter.add_item(1);
	selecter.add_item(2);

        key_listener.add_key_event(
		key::VKEY_1, [this](key::KeyStatus status) {
                                     if (status & key::KEY_FIRST_PRESSED) {
                                             u64 index = selecter.get();
                                             
                                             switch(index){
                                             case 0:
                                                     /*
                                                      * 諦めない
                                                      */
                                                     this->set_status(SUBEVE_FINISH);
                                                     this->rsm->effect_conroller.lock_object_move = false;
                                                     this->rsm->game_over_continue();
                                                     break;
                                             case 1:
                                                     /*
                                                      * 最初からやり直す
                                                      */
                                                     this->rsm->game_state = RESET_CURRENT;
                                                     break;
                                             case 2:
                                                     /*
                                                      * タイトルに戻る
                                                      */
                                                     this->rsm->game_state = START;
                                                     break;
                                             }
                                     }
                             });
}


void RaceSceneMaster::GameOverEvent::pre_process(sf::RenderWindow &window)
{
        for (size_t i = 0;i < choice_label_set.size();i++) {
                if(i != selecter.get())
                        choice_label_set[i]->set_font_size(28);
                else
			choice_label_set[i]->set_font_size(40);
                choice_label_set[i]->move(get_count());
                choice_label_set[i]->effect(get_count());
	}
        
        background->effect(get_count());
        
        key_listener.key_update();
        timer_list.check_and_call(get_count());
        update_count();
}

void RaceSceneMaster::GameOverEvent::drawing_process(sf::RenderWindow &window)
{
        rsm->post_draw_request("game_info", choice_label_set);
        rsm->post_draw_request_vargs("conversation", background);
}

GameState RaceSceneMaster::GameOverEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}
