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

RaceSceneMaster::RaceSceneMaster(GameData *game_data)
	: running_char(CharacterAttribute("stick man"),
		       GameMaster::texture_table[DOT_JUNKO],
		       GameMaster::texture_table[PLAYER_CORE],
		       sf::Vector2f(400, 200)),
	  target_udon(CharacterAttribute("target udon"),
		      GameMaster::texture_table[UDON1], sf::Vector2f(480, -80),
		      sf::Vector2f(0.8, 0.8), 0, mf::stop, rotate::stop,
                      400, 400, danmaku_empty_sched, false),
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
	  udon_hp(sf::Vector2f(850, 5), sf::Vector2f(1, 1), 400, 400,
		  sf::Color(10, 10, 20), sf::Color(213, 67, 67),
		  sf::Color(110, 50, 50)),
		  stamina_label(L"体力", game_data->get_font(JP_DEFAULT)),
	  junko_param_label(L"純化度", game_data->get_font(JP_DEFAULT)),
	  rec_label(L"●REC", game_data->get_font(JP_DEFAULT)),
	  graze_label(L"グレイズ", game_data->get_font(JP_DEFAULT)),
          game_score_label(L"スコア", game_data->get_font(JP_DEFAULT)),
	  graze_counter(0, game_data->get_font(JP_DEFAULT)),
	  window_frame(sf::IntRect(0, 0, 1366, 768),
		       sf::IntRect(32, 32, 960, 704)),
	  danmaku_sched({}),
          abs_danmaku_sched({ "stage1_danmaku.json" })
{
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
        
	stamina_label.set_place(0, 50);
	stamina.set_place(0, 80);
	junko_param_label.set_place(0, 110);
	junko_param.set_place(0, 140);
        udon_hp.set_place(92, 48);
	rec_label.set_place(900, 50);
	rec_label.set_color(sf::Color::Red);
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
                        get_count(), "udon");
                tachie_container.emplace_front(p);
                p->add_effect({ effect::fade_out(150),
                                effect::kill_at(150) });
                {
                        int n = 0;
                        std::vector<Bullet *> &bullets = bullet_pipeline.enemy_pipeline.actual_bullets;
                        for (u32 i = 0; i < bullets.size();
                             i++) {
                                if (running_char.distance(
                                            bullets[i]) < 240) {
                                        bullet_pipeline.special_pipeline.direct_insert_bullet(
                                                new Bullet(
                                                        GameMaster::texture_table[SMALL_CRYSTAL2],
                                                        bullets[i]->get_place(),
                                                        mf::active_homing(sf::Vector2f(300, 300), 10, running_char.get_homing_point()),
                                                        get_count(),
                                                        sf::Vector2f(0.7, 0.7), 7,
                                                        true, false
                                                        ));
                                        delete bullets[i];
                                        bullets[i] =
                                                bullets.back();
                                        bullets.pop_back();
                                        n++;
                                        if (!bullets.size()) {
                                                break;
                                        }
                                }
                        }
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
                func_table.add_function_dynamic("ringo", get_count() + (util::generate_random() % 60));

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
		bullet_pipeline.player_pipeline.add_function(
			new FunctionCallEssential("shot", get_count(),
                                              sf::Vector2f(p.x + 10, p.y - 5)));
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
        
        this->target_udon.set_hp_max();
        // 次の弾幕を追加し、タイマも設定する
        this->add_new_danmaku();
}

void RaceSceneMaster::kill_out_of_filed_bullet(std::vector<Bullet *> &bullets)
{
	for (u32 i = 0; i < bullets.size(); i++) {
		if (bullets[i]->is_finish(
			    sf::IntRect(-1368, -768, 1368 * 2, 768 * 2)) ||
		    !bullets[i]->visible()) {
			delete bullets[i];
			bullets[i] = bullets.back();
			bullets.pop_back();
			if (!bullets.size()) {
				break;
			}
		}else if(running_char.outer_distance(bullets[i]) < 15){
			if (bullets[i]->is_grazable()){
				graze_counter.counter_method().add(5);
                                bullets[i]->disable_graze();
                        }
                }
		bullets[i]->move(get_count());
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
                                                      begin_time, sf::Vector2f(0, 0)), data.time_limit));
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

        if(get_count() == 20){
                bullet_pipeline.enemy_pipeline.add_function(
			new FunctionCallEssential("ellipse", get_count(),
                                                  sf::Vector2f(0, 0)));
        }

	score_counter.counter_method().add(1);
        timelimit_counter.counter_method().add(-1);

        kill_out_of_filed_bullet(bullet_pipeline.player_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.enemy_pipeline.actual_bullets);
        kill_out_of_filed_bullet(bullet_pipeline.special_pipeline.actual_bullets);

        if(get_count() == 300){
                enemy_container.emplace_back(new EnemyCharacter(CharacterAttribute("test"),
                                                                GameMaster::texture_table[UDON1], sf::Vector2f(0, 200),
                                                                sf::Vector2f(0.8, 0.8), get_count(),
                                                                mf::tachie_move_constant(2, 0),
                                                                rotate::stop, 15, 15, danmaku_empty_sched, true));
                enemy_container.emplace_back(new EnemyCharacter(CharacterAttribute("test"),
                                                                GameMaster::texture_table[FLOWER1], sf::Vector2f(100, 10),
                                                                sf::Vector2f(0.75, 0.75), get_count(),
                                                                mf::up(-1),
                                                                rotate::pendulum(M_PI / 3, 60, 0.2),
                                                                15, 15, danmaku_empty_sched, true));
        }else if(get_count() == 2000){
                timer_list.cancel(danmaku_timer_id);
                next_danmaku_forced();
                target_udon.override_move_func(mf::move_point_constant(sf::Vector2f(480, 50),
                                                                       target_udon.get_place(), 2000, 2128));
                backgroundTile.set_scroll_speed(-1);
                timer_list.add_timer(
                        [&](void){
                                sub_event_list.add(new ConversationEvent(this, sf::Vector2f(0, 0), game_data));
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

	rec_label.draw(window);
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

        for(SceneSubEvent *sse : sub_event_list){
                sse->drawing_process(window);
        }
        
}

GameState RaceSceneMaster::post_process(sf::RenderWindow &window)
{
        sub_event_list
                .native_method()
                .remove_if([&](SceneSubEvent *sse)
                                   { return sse->post_process(window) == SUBEVE_FINISH; });

	return RACE;
}

RaceSceneMaster::ConversationEvent::ConversationEvent(RaceSceneMaster *rsm, sf::Vector2f pos, GameData *data)
        : SceneSubEvent(pos), episode("stage1_spell.txt", data->get_font(JP_DEFAULT)),
          background(GameMaster::texture_table[SAMPLE_BACKGROUND1], sf::Vector2f(0, 450), mf::stop, 0)
{
        set_status(SUBEVE_CONTINUE);
        
        auto udon = new Tachie(
                GameMaster::texture_table[UDON_TACHIE],
                sf::Vector2f(100, 70),
                mf::stop,
                get_count(), "udon");
        udon->set_scale(0.55, 0.55);
        udon->add_effect({ effect::fade_in(30) });
        
        auto junko = new Tachie(
                GameMaster::texture_table[JUNKO_TACHIE1],
                sf::Vector2f(600, 70),
                mf::stop,
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

