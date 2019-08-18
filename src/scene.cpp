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

sf::View *SceneMaster::create_view(std::string key, sf::FloatRect area)
{
        sf::View *p = new sf::View(area);
        views.emplace(key, p);
        return p;
}

sf::View *SceneMaster::get_view(std::string key)
{
        return views[key];
}

void SceneMaster::switch_view(std::string key, sf::RenderWindow &window)
{
        window.setView(*get_view(key));
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
Bullet *test_bullet2;
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
	  backgroundTile(GameMaster::texture_table[MOON_CITY_TILE],
			 sf::Vector2f(32, 32), sf::IntRect(0, 0, 960, 736),
			 sf::Vector2f(1, 1)),
	  game_background(GameMaster::texture_table[ICHIMATSU1],
			  sf::Vector2f(0, 0), sf::IntRect(0, 0, 1366, 768),
			  sf::Vector2f(0.2, 0.2)),
          game_score_counter(0, game_data->get_font(JP_DEFAULT)),
	  score_counter(0, game_data->get_font(JP_DEFAULT)),
          timelimit_counter(30, game_data->get_font(JP_DEFAULT)),
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
	  graze_counter(0, game_data->get_font(JP_DEFAULT)),
	  window_frame(sf::IntRect(0, 0, 1366, 768),
		       sf::IntRect(32, 32, 960, 704)),
	  danmaku_sched({}),
          abs_danmaku_sched({ "stage1_danmaku.json" }),
          enemy_sched(game_data, "stage1_enemy_schedule.json")
{
        set_count_for_debug(3200);
        
        this->game_data = game_data;
	test_bullet = new Bullet(GameMaster::texture_table[BULLET1],
				 sf::Vector2f(400, 400),
                                 mf::stop,
                                 0, sf::Vector2f(0.12, 0.12), BulletSize::BULLET1,
                                 true, true, M_PI_4);
        
        test_bullet2 = new Bullet(GameMaster::texture_table[BULLET1],
                                  sf::Vector2f(400, 400),
                                  mf::stop,
                                  0, sf::Vector2f(0.12, 0.12), BulletSize::BULLET1,
                                  true, true, 0);
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
        game_score_counter.set_place(150, 200);
        game_score_label.set_place(0, 200);

        create_view("background", sf::FloatRect(0.0f, 0.0f, 1366.f, 768.f))->
                setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

	create_view("field",
		    sf::FloatRect(32.0f, 32.0f, 960.f, 736.f - (32.f)))
		->setViewport(sf::FloatRect(32.f / 1366.f, 32.f / 768.f,
					    960.f / 1366.f,
					    702.f / 768.f));

	create_view("bullets", sf::FloatRect(32.0f, 32.0f, 960.f, 768.f - (32.f * 2)))->
                setViewport(sf::FloatRect(32.f / 1366.f, 32.f / 768.f, 960.f / 1366.f, 702.f / 768.f));
        
        create_view("params", sf::FloatRect(0.0f, 0.0f, 420.f, 300.f))->
                setViewport(sf::FloatRect(1010.f / 1366.f, 32.f / 768.f, 420.f / 1366.f, 300.f / 768.f));
        
	create_view("tachie", sf::FloatRect(0.0f, 0.0f, 1366.f, 768.f))
		->setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

        bullet_pipeline.enemy_pipeline.add_function(
                new FunctionCallEssential("field1", 30,
                                          sf::Vector2f(0, 0)));

        backgroundTile.set_scroll_speed(5);

}

void RaceSceneMaster::player_spellcard(void)
{
        if (std::find_if(
                    container_entire_range(tachie_container),
                    [](Tachie *p) {
                            return p->are_you("udon");
                    }) == std::end(tachie_container)){
                auto p = new Tachie(
                        GameMaster::texture_table[UDON_TACHIE],
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
                        for (Bullet *b : bullets) {
                                if (running_char.distance(b) < 240) {
                                        bullet_pipeline.special_pipeline.direct_insert_bullet(
                                                new Bullet(
                                                        GameMaster::texture_table[SMALL_CRYSTAL2],
                                                        b->get_place(),
                                                        mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                                        get_count(),
                                                        sf::Vector2f(0.7, 0.7), 7,
                                                        true, false
                                                        ));
                                }
                        }

                        for(Bullet *&b : bullets){
				if (running_char.distance(b) < 240) {
					delete b;
					b = nullptr;
				}
			}
                        bullets.remove_if([=](Bullet *b){ return !b; });
                        
                        graze_counter.counter_method().add(
                                -200);
                        junko_param.add(-n);
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
                        get_count()));
		if (danmaku_sched.top().type == SPELL_CARD_DANMAKU) {
			sub_event_list.push_back(new SpellCardEvent(
                                                   this, sf::Vector2f(0, 0), game_data,
                                                   danmaku_sched.top()));
		}
                
                // スケジュールした弾幕が切れた時に新しい弾幕をスケジュール出来るように処理
                danmaku_timer_id = last_danmaku_timer_id = timer_list.add_timer(
			[this](void) { this->next_danmaku_forced(); },
			get_count(), danmaku_sched.top().time_limit);
		// タイマの実行時間は、弾幕発生 + 弾幕タイムリミット
                
                timelimit_counter.counter_method().set_score(danmaku_sched.top().time_limit);
                
                // 先頭の弾幕を捨てる
                danmaku_sched.drop_top();
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
        
/*
  if (get_count() % 20 == 16) {
  running_char.change_textures(GameMaster::texture_table[UDON5]);
  } else if (get_count() % 20 == 12) {
  running_char.change_textures(GameMaster::texture_table[UDON4]);
  } else if (get_count() % 20 == 8) {
  running_char.change_textures(GameMaster::texture_table[UDON3]);
  } else if (get_count() % 20 == 4) {
  running_char.change_textures(GameMaster::texture_table[UDON2]);
  } else if (get_count() % 20 == 0) {
  running_char.change_textures(GameMaster::texture_table[UDON1]);
  }
*/
}

void RaceSceneMaster::add_new_functional_bullets_to_schedule(void)
{
/*
        if(!(util::generate_random() % 60)){
		func_table.add_function_dynamic(FunctionCallEssential(
			"junko1",
			get_count() + (util::generate_random() % 60),
			sf::Vector2f(util::generate_random(0, 800),
				     util::generate_random(0, 400))));
	}else if(!(util::generate_random() % 300)){
                f2unc_table.add_function_dynamic("ringo", get_count() + (util::generate_random() % 60));

	} else if (!(util::generate_random() % 200)) {
		func_table.add_function_dynamic(
                        FunctionCallEssential(
			"hart", get_count() + (util::generate_random() % 60),
			sf::Vector2f(util::generate_random(0, 400),
				     util::generate_random(0, 400))));
	}
*/

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                sf::Vector2f &&p = running_char.get_place();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
			bullet_pipeline.player_pipeline.add_function(
				new FunctionCallEssential(
					"junko_shot_slow_lv1", get_count(),
					sf::Vector2f(p.x + 10, p.y - 7)));
		}else{
                        bullet_pipeline.player_pipeline.add_function(
				new FunctionCallEssential(
					"junko_shot_fast_lv1", get_count(),
					sf::Vector2f(p.x + 10, p.y - 7)));
                }                
	}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
                bullet_pipeline.enemy_pipeline.clear_all_bullets();
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

void RaceSceneMaster::next_danmaku_forced(void)
{
        for (auto &&bullet : bullet_pipeline.enemy_pipeline.actual_bullets) {
                if(bullet->visible() && !bullet->is_finish(sf::IntRect(-512, -512, 1366 + 1024, 768 + 1024))){
                        bullet_pipeline.special_pipeline.direct_insert_bullet(
                        new Bullet(
                                GameMaster::texture_table[SMALL_CRYSTAL2],
                                bullet->get_place(),
                                mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                get_count(),
                                sf::Vector2f(0.7, 0.7), 7,
                                true, false
                                ));
                }
	}
        
        bullet_pipeline.enemy_pipeline.clear_all_bullets();
        sub_event_list.remove_if([](SceneSubEvent *sse){
                                         if(sse->get_name() == "spell"){
                                                 delete sse;
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
        bullets.remove_if([](Bullet *b){
                                  if (b->is_finish(
                                              sf::IntRect(-1368, -768, 1368 * 2, 768 * 2)) ||
                                      !b->visible()) {
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
                                FunctionCallEssential(data.func_name,
                                                      begin_time, sf::Vector2f(0, 0)), data.time_limit, data.type, data.danmaku_name)
                        );
                begin_time += data.time_limit;
        }
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
                                if(target_udon.dead()){
                                        timer_list.cancel(last_danmaku_timer_id);
                                        next_danmaku_forced();
                                }
                                continue;
                        }
                }

                for(size_t i = 0;i < enemy_container.size();i++){
                        auto p = enemy_container[i];
                        if(p->check_conflict(*bullet)){
                                p->damage(1);
                                bullet->hide();
                                bullet_pipeline.special_pipeline.direct_insert_bullet(
                                        new Bullet(
                                                GameMaster::texture_table[SMALL_CRYSTAL1],
                                                bullet->get_place(),
                                                mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                                get_count(),
                                                sf::Vector2f(0.7, 0.7), 7,
                                                true, false
                                                ));
                                if(p->dead()){
                                        puts("dead");
                                        std::swap(enemy_container[i], enemy_container.back());
                                        enemy_container.pop_back();
                                }
                                break;
                        }
                }
	}
        
        for (auto &&bullet : bullet_pipeline.enemy_pipeline.actual_bullets) {
                if (bullet->check_conflict(running_char)) {
                        bullet->hide();
                        junko_param.add(10);
                }
	}
        
        for (auto &&bullet : bullet_pipeline.special_pipeline.actual_bullets) {
		if (bullet->check_conflict(running_char)) {
			bullet->hide();
                        game_score_counter.counter_method().add(5);
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

void RaceSceneMaster::pre_process(sf::RenderWindow &window)
{
        add_new_functional_bullets_to_schedule();
        bullet_pipeline.all_schedule_bullet(get_count(), running_char);

        random_mist();
        
        conflict_judge();
        
	player_move();
        test_bullet->move(get_count());
        test_laser->move(get_count());
        test_slaser->update_scale(get_count());

        for(auto p : enemy_container){
                std::optional<FunctionCallEssential> e = p->shot(get_count());
                if(e){
                        bullet_pipeline.enemy_pipeline.add_function(
                                new FunctionCallEssential(
                                        e.value().func_name,
                                        get_count(),
                                        p->get_place()));
                }
        }

        if(get_count() == 20){
                bullet_pipeline.enemy_pipeline.add_function(
			new FunctionCallEssential("ellipse", get_count(),
                                                  sf::Vector2f(0, 0)));
        }

	score_counter.counter_method().add(1);
        timelimit_counter.counter_method().add(-1);

        check_graze(bullet_pipeline.enemy_pipeline.actual_bullets);
        
        kill_out_of_filed_bullet(bullet_pipeline.player_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.enemy_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.special_pipeline.actual_bullets);

        while(enemy_sched.size()){
		if (enemy_sched.get_front().time == get_count()) {
			enemy_container.emplace_back(new EnemyCharacter(
				enemy_sched.get_front(), get_count()));
			enemy_sched.pop_front();
		}else{
                        break;
                }
	}

	if(get_count() == 300){
                enemy_container.emplace_back(new EnemyCharacter(CharacterAttribute("test"),
                                                                GameMaster::texture_table[UDON1], sf::Vector2f(0, 200),
                                                                sf::Vector2f(0.8, 0.8), get_count(),
                                                                mf::tachie_move_constant(2, 0),
                                                                rotate::stop, 15, 15, true));
                enemy_container.emplace_back(new EnemyCharacter(CharacterAttribute("test"),
                                                                GameMaster::texture_table[FLOWER1], sf::Vector2f(0, 0),
                                                                sf::Vector2f(0.75, 0.75), get_count(),
                                                                mf::curve(sf::Vector2f(0, 0),
                                                                          sf::Vector2f(50, 500),
                                                                          sf::Vector2f(700, 50),
                                                                          1000),

                                                                rotate::pendulum(M_PI / 3, 60, 0.2),
                                                                15, 15, true));
        }else if(get_count() == 3500){
                timer_list.cancel(danmaku_timer_id);
                next_danmaku_forced();
                target_udon.override_move_func(mf::move_point_constant(sf::Vector2f(480, 50),
                                                                       target_udon.get_place(), 3500, 3628));
                backgroundTile.set_scroll_speed(-1);
                timer_list.add_timer(
                        [&](void){
                                sub_event_list.push_back(new ConversationEvent(this, sf::Vector2f(0, 0), game_data));
                        }, 240, get_count());
                        
        }
        
	tachie_container.remove_if([](Tachie *p) { return !p->visible(); });
        // 立ち絵の移動
        for(auto &&p : tachie_container){
                p->move(get_count());
		p->effect(get_count());
	}

        for(auto &&p : enemy_container){
                p->move(get_count());
                p->rotate_with_func(get_count());
	}

        for(auto p : move_object_container){
                p->move(get_count());
        }
        
	backgroundTile.scroll();

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

        switch_view("background", window);
        
	game_background.draw(window);

        switch_view("field", window);
        
	backgroundTile.draw(window);

        
        for(SceneSubEvent *sse : sub_event_list){
                sse->drawing_process(window);
        }

	running_char.draw(window);
        target_udon.draw(window);

        switch_view("bullets", window);

        for(auto p : move_object_container){
                p->draw(window);
        }
        for(auto p : enemy_container){
                p->draw(window);
        }

        bullet_pipeline.draw(window);
	test_bullet->draw(window);
        test_bullet2->draw(window);
        test_laser->draw(window);
        //test_slaser->draw(window);

        udon_hp.draw(window);
        timelimit_counter.draw(window);

	//window_frame.draw(window);

        switch_view("params", window);
	score_counter.draw(window);

        game_score_counter.draw(window);
        game_score_label.draw(window);
        
	stamina.draw(window);
	junko_param.draw(window);
	stamina_label.draw(window);
	junko_param_label.draw(window);

        graze_counter.draw(window);
        graze_label.draw(window);

	switch_view("tachie", window);
        // 立ち絵の移動
        for(auto &&p : tachie_container){
                if(p->visible())
                        p->draw(window);
	}
        
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
                                               }else{
                                                       episode.next();
                                               }
                                       }
                               });
        background.set_alpha(200);
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
        background.draw(window);
        
        for(auto &&p : tachie_container){
                if(p->visible())
                        p->draw(window);
	}

        episode.get_current_page()->draw(window);
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
        
        auto udon = new Tachie(
                GameMaster::texture_table[UDON_TACHIE],
                sf::Vector2f(500, 70),
                mf::vector_linear(sf::Vector2f(1, -2)),
                rotate::stop,
                get_count(), "udon");
        udon->set_scale(0.55, 0.55);
        udon->add_effect({ effect::fade_out(120) });

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
                objects.push_front(p);
        }
        timer_list.add_timer([this](void){ this->set_status(SUBEVE_FINISH); }, danmaku_data.time_limit);

        auto hexagram = new MoveObject(GameMaster::texture_table[HEXAGRAM],
                                       sf::Vector2f(200, 50),
                                       mf::same_position(&rsm->target_udon),
                                       rotate::constant(0.05),
                                       get_count());
        hexagram->set_scale(0.25, 0.25);
        hexagram->set_default_origin();
        hexagram->add_effect({ effect::fade_in(30) });
        objects.push_front(hexagram);

        if(danmaku_data.type == SPELL_CARD_DANMAKU){
                float x = 980 - (24 * util::wstrlen(danmaku_data.danmaku_name->data()));
                objects.push_front(new DynamicText(
                                           danmaku_data.danmaku_name->data(), data->get_font(JP_DEFAULT),
                                           sf::Vector2f(x, 600), mf::ratio_step(sf::Vector2f(x, 80), 0.1),
                                           rotate::stop, 0, 24));
        }
        
        background = new MoveObject(GameMaster::texture_table[BACKGROUND1],
                                         sf::Vector2f(0, 0),
                                         mf::stop,
                                         rotate::stop,
                                         get_count());

        background->set_repeat_flag(true);
        background->add_effect({ effect::fade_in(20) });
        background->set_alpha(220);
        background->override_move_func([](MoveObject *p, u64 now, u64 begin){
                                               p->move_sprite(sf::Vector2f(now - begin, 0));
                                               return p->get_place();
                                       });

        create_view("general", sf::FloatRect(0.0f, 0.0f, 920.f, 768.f))->setViewport(sf::FloatRect(0.0f, 0.0f, 920.f / 1366, 1.0f));
        create_view("background", sf::FloatRect(0.0f, 0.0f, 1366.f, 768.f))->setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
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

        timer_list.check_and_call(get_count());
        update_count();
}

void RaceSceneMaster::SpellCardEvent::drawing_process(sf::RenderWindow &window)
{
        background->draw(window);
        background->move_sprite(sf::Vector2f(1, 0));


        for(auto &&p : objects){
                if(p->visible())
                        p->draw(window);
	}
        
        for(auto &&p : tachie_container){
                if(p->visible())
                        p->draw(window);
	}
        
}

GameState RaceSceneMaster::SpellCardEvent::post_process(sf::RenderWindow &window)
{
        return SceneSubEvent::post_process(window);
}
