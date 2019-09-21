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
Laser *test_laser;
StraightLaser *test_slaser;

RaceSceneMaster::RaceSceneMaster(GameData *game_data)
	: running_char(CharacterAttribute("stick man"),
		       GameMaster::texture_table[DOT_JUNKO],
		       GameMaster::texture_table[PLAYER_CORE],
		       sf::Vector2f(400, 200)),
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
          game_score_counter(0, game_data->get_font(JP_DEFAULT)),
	  score_counter(0, game_data->get_font(JP_DEFAULT)),
          timelimit_counter(300, game_data->get_font(JP_DEFAULT), 60),
          power_counter(0, game_data->get_font(JP_DEFAULT)),
	  func_table("main.json"),
	  stamina(sf::Vector2f(300, 20), sf::Vector2f(2, 2), 400, 400,
		  sf::Color(10, 10, 20), sf::Color::Green,
		  sf::Color(20, 100, 20)),
	  junko_param(sf::Vector2f(300, 20), sf::Vector2f(2, 2), 0, 400,
		      sf::Color(10, 10, 20), sf::Color(213, 67, 67),
		      sf::Color(110, 50, 50)),
	  udon_hp(sf::Vector2f(850, 5), sf::Vector2f(1, 1), 1000, 1000,
		  sf::Color(10, 10, 20), sf::Color(213, 67, 67),
		  sf::Color(110, 50, 50)),
		  stamina_label(L"体力", game_data->get_font(JP_DEFAULT)),
	  junko_param_label(L"純化度", game_data->get_font(JP_DEFAULT)),
	  graze_label(L"グレイズ", game_data->get_font(JP_DEFAULT)),
          game_score_label(L"スコア", game_data->get_font(JP_DEFAULT)),
          power_label(L"霊力", game_data->get_font(JP_DEFAULT)),
	  graze_counter(0, game_data->get_font(JP_DEFAULT)),
	  window_frame(sf::IntRect(0, 0, 1366, 768),
		       sf::IntRect(32, 32, 960, 704)),
	  danmaku_sched({}),
          abs_danmaku_sched({ "stage1_danmaku.json" }),
          enemy_sched(game_data, "stage1_enemy_schedule.json"),
          udon_marker(GameMaster::texture_table[UDON_MARKER], sf::Vector2f(0, 725), mf::stop, rotate::stop, 0)
{
        set_count_for_debug(0);
        
        this->game_data = game_data;
	test_bullet = new Bullet(GameMaster::texture_table[BULLET1],
				 sf::Vector2f(400, 400),
                                 mf::stop,
                                 0, sf::Vector2f(0.12, 0.12), BulletSize::BULLET1,
                                 true, true, M_PI_4);
        test_3d_object = new DrawableObject3D(GameMaster::texture_table[TAKE1],
                                              sf::Vector3f(-3000, 100, 150),
                                              [](DrawableObject3D *p, u64 n, u64 b){
                                                      return p->get_3d_position() + sf::Vector3f(0, 0, -1);
                                              },
                                              rotate::stop,
                                              0);
        //test_3d_object->set_scale(0.1, 0.1);
        test_laser = new Laser(GameMaster::texture_table[LASER_TAIL3],
                               GameMaster::texture_table[LASER_BODY3],
                               GameMaster::texture_table[LASER_HEAD3],
                               sf::Vector2f(140, 40),
                               mf::curve(sf::Vector2f(100, 30),
                                         sf::Vector2f(-150, 400),
                                         sf::Vector2f(700, 650),
                                         200),
                               0, sf::Vector2f(0.3, 0.3), BulletSize::BULLET1, 128);
        test_slaser = new StraightLaser(GameMaster::texture_table[LASER_BODY3],
                                        sf::Vector2f(500, 100), sf::Vector2f(100, 500),
                                        [](MoveObject *p, u64 n, u64 b){
                                                p->set_scale(1 - (0.01 * (n-b)), 1);
                                                return p->get_place();
                                        },
                                        get_count());
        
	stamina_label.set_place(0, 50);
	stamina.set_place(0, 80);
	junko_param_label.set_place(0, 110);
	junko_param.set_place(0, 140);
        udon_hp.set_place(92, 48);
        timelimit_counter.set_place(870, 48);
        graze_label.set_place(0, 250);
        graze_counter.set_place(150, 250);
        graze_counter.counter_method().add(10000);
        game_score_counter.set_place(150, 200);
        game_score_label.set_place(0, 200);
        power_label.set_place(0, 300);
        power_counter.set_place(150, 300);

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

        bullet_pipeline.enemy_pipeline.add_function(
                new FunctionCallEssential("field1", 30, NO_SHOT_MASTER,
                                          sf::Vector2f(0, 0)));

        backgroundTile.set_scroll_speed(5);
        udon_marker.set_scale(0.4, 0.4);

        target_udon.add_effect({ effect::animation_effect({ UDON1, UDON2, UDON3, UDON4, UDON5 }, 5) });

}

void RaceSceneMaster::player_spellcard_effect(void)
{
        for (int i = 0;i < 256;i++) {
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
                move_object_container.emplace_front(p);
        }
        for(EnemyCharacter *e : enemy_container){
                e->damage(400);
        }
        target_udon.damage(400);
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
                        int n = 0;
                        std::list<Bullet *> &bullets = bullet_pipeline.enemy_pipeline.actual_bullets;
                        bullets.remove_if([&, this](Bullet *b){
                                                  if (running_char.distance(b) < 800) {
                                                          bullet_pipeline.special_pipeline.direct_insert_bullet(
                                                                  new SpecialBullet(
                                                                          GameMaster::texture_table[SMALL_CRYSTAL2],
                                                                          b->get_place(),
                                                                          mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                                                          get_count(),
                                                                          sf::Vector2f(0.7, 0.7), 7,
                                                                          true, false, 0, SpecialBulletAttribute(0, 10)
                                                                          ));
                                                          delete b;
                                                          return true;
                                                  }else{
                                                          return false;
                                                  }
                                          });
                        
                        graze_counter.counter_method().add(
                                -200);
                        junko_param.add(-n);
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
                timelimit_counter.counter_method().set_score(danmaku_sched.top().time_limit);
                
                
                // 先頭の弾幕を捨てる
                danmaku_sched.drop_top();
	}else{
                /*
                 * 全てのスペルカードを撃ち終わった
                 */
                sub_event_list.push_back(new ResultEvent(
                                                 this, sf::Vector2f(0, 0), game_data,
                                                 ScoreInformation(power_counter.counter_method().get_score(),
                                                                  game_score_counter.counter_method().get_score(),
                                                                  graze_counter.counter_method().get_score(),
                                                                  race_status.get_hit_count())));
        }
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
                move_object_container.emplace_front(p);
        }
}

void RaceSceneMaster::player_move()
{
	static float speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
		running_char.core_on();
		speed = 1.9;
	} else {
		running_char.core_off();
		speed = 4.5;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		running_char.move_diff(sf::Vector2f(-speed, 0));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		running_char.move_diff(sf::Vector2f(speed, 0));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		running_char.move_diff(sf::Vector2f(0, speed));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		running_char.move_diff(sf::Vector2f(0, -speed));
		stamina.add(-2.0);
        }
	
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
                if(graze_counter.counter_method().get_score() >= 200){
                        player_spellcard();
                }
        }

        running_char.move_diff(sf::Vector2f(0, 0));
	stamina.add(1);

        target_udon.move_diff(sf::Vector2f(((float)(util::generate_random() % 10) / 10.0) * std::sin((float)((float)get_count() / (float)60)), 0));
        target_udon.move(get_count());
        target_udon.effect(get_count());
        sf::Vector2f udon_place = target_udon.get_place();
        udon_marker.set_place(sf::Vector2f(udon_place.x, 725));
        
        if(running_char.get_origin().y < 250){
                current_item_collect();
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
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
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
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
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
                                        true, false, 0, SpecialBulletAttribute(0, 10)
                                        ));
                }
	}
        
        pipeline.clear_all_bullets();
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
                                                 return true;
                                         }
                                         return false;
                                 });
        
        this->target_udon.set_hp_max();
        // 次の弾幕を追加し、タイマも設定する
        this->add_new_danmaku();
}

void RaceSceneMaster::kill_out_of_filed_bullet(std::list<Bullet *> &bullets)
{
        bullets.remove_if([this](Bullet *b){
                                  if (b->is_finish(
                                              sf::IntRect(-1368, -768, 1368 * 2, 768 * 2)) ||
                                      !b->visible() ||
                                      std::find(std::begin(killed_shot_master_id),
                                                std::end(killed_shot_master_id),
                                                b->get_shot_master_id()) != std::end(killed_shot_master_id)) {
                                          delete b;
                                          b = nullptr;
                                          return true;
                                  }
                                  return false;
                          });
	for (Bullet *b : bullets) {
                b->move(get_count());
                b->effect(get_count());
	}
}

void RaceSceneMaster::check_graze(std::list<Bullet *> &bullets)
{
        for (Bullet *b : bullets) {
                if (b->is_grazable()){
                        if(running_char.outer_distance(b) < b->get_radius() + 5){
				graze_counter.counter_method().add(5);
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
                                        timer_list.cancel(last_danmaku_timer_id);
                                        generate_items_random(ItemOrder(24, 24), target_udon.get_origin(), 100);
                                        next_danmaku_forced();
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
                                                true, false, 0, SpecialBulletAttribute(0, 10)
                                                ));
                        }
                        if(!p->dead() && p->hp_zero()){
                                auto bomb = enemy_manager.kill_enemy_with_normal_effect(p, get_count());
                                move_object_container.emplace_front(bomb);

                                generate_items_random(ItemOrder(2, 2), p->get_origin(), 50);
                                
                                killed_shot_master_id.push_back(p->get_shot_master_id());
                        }
                }

                enemy_container.remove_if(
                        [](EnemyCharacter *p){
                                return !p->visible();
                        });
	}
        
        for (auto &&bullet : bullet_pipeline.enemy_pipeline.actual_bullets) {
                if (bullet->check_conflict(running_char)) {
                        bullet->hide();
                        race_status.hit();
                        junko_param.add(10);
                }
	}
        
        for (auto &&bullet : bullet_pipeline.special_pipeline.actual_bullets) {
                SpecialBullet *sp_bullet = dynamic_cast<SpecialBullet *>(bullet);
		if (sp_bullet && sp_bullet->check_conflict(running_char)) {
                        SpecialBulletAttribute &&a = sp_bullet->get_attribute();
			sp_bullet->hide();
                        game_score_counter.counter_method().add(a.score);
                        power_counter.counter_method().add(a.power);
		}
	}

        for (auto &&bullet : test_laser->get_bullet_stream()) {
		if (bullet->check_conflict(running_char)) {
			bullet->hide();
                        game_score_counter.counter_method().add(5);
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
                        origin + sf::Vector2f((util::generate_random() % width) - half, 0),
                        mf::accelerating(sf::Vector2f(0, -5), sf::Vector2f(0, 0.15), sf::Vector2f(0, 0), sf::Vector2f(-5, 2)),
                        get_count() + (util::generate_random() % 5),
                        sf::Vector2f(0.04, 0.04), BulletSize::SPECIAL_PANEL,
                        true, false, 0, SpecialBulletAttribute(0.03, 0)
                        );
                p->set_shot_master_id(SPECIAL_ITEM);
                bullet_pipeline.special_pipeline.direct_insert_bullet(p);
        }

        for(int i = 0;i < item.score;i++){
                auto p = new SpecialBullet(
                        GameMaster::texture_table[SCORE_PANEL],
                        origin + sf::Vector2f((util::generate_random() % width) - half, 0),
                        mf::accelerating(sf::Vector2f(0, -5), sf::Vector2f(0, 0.15), sf::Vector2f(0, 0), sf::Vector2f(-5, 2)),
                        get_count() + (util::generate_random() % 5),
                        sf::Vector2f(0.04, 0.04), BulletSize::SPECIAL_PANEL,
                        true, false, 0, SpecialBulletAttribute(0, 10)
                        );
                p->set_shot_master_id(SPECIAL_ITEM);
                bullet_pipeline.special_pipeline.direct_insert_bullet(p);
        }
}

void RaceSceneMaster::pre_process(sf::RenderWindow &window)
{
        add_new_functional_bullets_to_schedule();
        bullet_pipeline.all_schedule_bullet(get_count(), running_char, target_udon);

        random_mist();
        
        conflict_judge();
        
	player_move();
        test_bullet->move(get_count());
        test_3d_object->move(get_count());
        test_laser->move(get_count());
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
                                                           sf::Vector3f(-3000, 3000, 150),
                                                           [](DrawableObject3D *p, u64 n, u64 b){
                                                                   return p->get_3d_position() + sf::Vector3f(0, 0, -1);
                                                           },
                                                           rotate::stop,
                                                           get_count());
                p->add_effect({ effect::fade_in(40) });
                object3d_list.push_back(p);

                p = new DrawableObject3D(GameMaster::texture_table[TAKE1],
                                         sf::Vector3f(9000, 3000, 150),
                                         [](DrawableObject3D *p, u64 n, u64 b){
                                                 return p->get_3d_position() + sf::Vector3f(0, 0, -1);
                                         },
                                         rotate::stop,
                                         get_count());
                p->add_effect({ effect::fade_in(40) });
                object3d_list.push_back(p);
        }

        for(DrawableObject3D *p : object3d_list){
                p->move(get_count());
                p->effect(get_count());
        }

	score_counter.counter_method().add(1);
        timelimit_counter.counter_method().add(-1);

        check_graze(bullet_pipeline.enemy_pipeline.actual_bullets);
        
        kill_out_of_filed_bullet(bullet_pipeline.player_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.enemy_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.special_pipeline.actual_bullets);
        remove_killed_shot(bullet_pipeline.enemy_pipeline);
        killed_shot_master_id.clear();

        for (Bullet *b : bullet_pipeline.special_pipeline.actual_bullets) {
                if(b->get_shot_master_id() == SPECIAL_ITEM && running_char.outer_distance(b) < b->get_radius() + 100){
                        b->override_move_func(mf::active_homing(running_char.get_origin(), 5, running_char.get_homing_point()));
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

        if(get_count() == 4500){
                timer_list.cancel(danmaku_timer_id);
                convert_bullet_to_small_crystal(bullet_pipeline.enemy_pipeline);
                this->target_udon.set_hp_max();
                target_udon.override_move_func(mf::move_point_constant(sf::Vector2f(480, 50),
                                                                       target_udon.get_place(), 4500, 4628));
                backgroundTile.set_scroll_speed(-1);
                timer_list.add_timer(
                        [&](void){
                                sub_event_list.push_back(new ConversationEvent(this, sf::Vector2f(0, 0), game_data));
                        }, 240, get_count());
                effect_conroller.udon_marker_hide = false;
                        
        }
        
	tachie_container.remove_if([](Tachie *p) { return !p->visible(); });
        // 立ち絵の移動
        for(auto &&p : tachie_container){
                p->move(get_count());
		p->effect(get_count());
	}

        for(auto &&p : enemy_container){
                p->move(get_count());
                p->effect(get_count());
                p->rotate_with_func(get_count());
	}

        move_object_container.remove_if(
                [this](MoveObject *p){
                        if(p->visible()){
                                p->move(get_count());
                                p->effect(get_count());
                                return false;
                        }else{
                                return true;
                        }
                });
        
	//backgroundTile.scroll();

	timer_list.check_and_call(get_count());
        
        for(SceneSubEvent *sse : sub_event_list){
                sse->pre_process(window);
        }
        
	update_count();
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
        post_draw_request("character", enemy_container);

        post_draw_request_vargs("bullets",
                                &bullet_pipeline,
                                test_bullet,
                                test_3d_object,
                                test_laser);

	//window_frame.draw

        post_draw_request_vargs("params",
                                &game_score_counter,
                                &game_score_label,
                                &stamina,
                                &junko_param,
                                &stamina_label,
                                &junko_param,
                                &graze_counter,
                                &graze_label,
                                &power_label,
                                &power_counter
                );

        post_draw_request_vargs("game_info",
                                &udon_hp,
                                &score_counter,
                                &timelimit_counter);

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

        switch_view("conversation", window);
        get_view("conversation")->flush_draw_requests(window);

        switch_view("game_info", window);
        get_view("game_info")->flush_draw_requests(window);

        switch_view("params", window);
        get_view("params")->flush_draw_requests(window);

        switch_view("tachie", window);
        get_view("tachie")->flush_draw_requests(window);
}

GameState RaceSceneMaster::post_process(sf::RenderWindow &window)
{
        sub_event_list
                .remove_if([&](SceneSubEvent *sse)
                                   { return sse->post_process(window) == SUBEVE_FINISH; });

	return RACE;
}

RaceSceneMaster::ConversationEvent::ConversationEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data)
        : SceneSubEvent(pos, "conv"), episode("stage1_spell.txt", data->get_font(JP_DEFAULT)),
          background(GameMaster::texture_table[SAMPLE_BACKGROUND1], sf::Vector2f(0, 450), mf::stop, rotate::stop, 0)
{
        set_status(SUBEVE_CONTINUE);

        this->rsm = rsm;
        
        auto udon = new Tachie(
                GameMaster::texture_table[UDON_TACHIE],
                sf::Vector2f(100, 70),
                mf::stop,
                rotate::stop,
                get_count(), "udon");
        udon->set_scale(0.55, 0.55);
        udon->add_effect({ effect::fade_in(30) });
        
        auto junko = new Tachie(
                GameMaster::texture_table[JUNKO_TACHIE1],
                sf::Vector2f(600, 70),
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

        update_count();
}

void RaceSceneMaster::ConversationEvent::drawing_process(sf::RenderWindow &window)
{
        rsm->post_draw_request_vargs("conversation", &background);
        rsm->post_draw_request("tachie", tachie_container);
        rsm->post_draw_request_vargs("conversation", episode.get_current_page());
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
                                std::string("スコア: ")
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
                        rsm->post_draw_request_vargs("field", p);
	}
}

GameState RaceSceneMaster::ResultEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}
