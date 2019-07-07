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

RaceSceneMaster::RaceSceneMaster(GameData *game_data)
	: running_char(CharacterAttribute("stick man"),
		       GameMaster::texture_table[DOT_JUNKO],
		       GameMaster::texture_table[PLAYER_CORE],
		       sf::Vector2f(400, 200)),
	  backgroundTile(GameMaster::texture_table[MOON_CITY_TILE],
			 sf::Vector2f(32, 32), sf::IntRect(0, 0, 960, 736),
			 sf::Vector2f(1, 1)),
	  game_background(GameMaster::texture_table[ICHIMATSU1],
			  sf::Vector2f(0, 0), sf::IntRect(0, 0, 1366, 768),
			  sf::Vector2f(0.2, 0.2)),
	  score_counter(0, game_data->get_font(JP_DEFAULT)),
          func_table("main.json"), bullets_sched(),
	  stamina(sf::Vector2f(300, 20), sf::Vector2f(2, 2), 400, 400,
		  sf::Color(10, 10, 20), sf::Color::Green,
		  sf::Color(20, 100, 20)),
	  junko_param(sf::Vector2f(300, 20), sf::Vector2f(2, 2), 0, 400,
		      sf::Color(10, 10, 20), sf::Color(213, 67, 67),
		      sf::Color(110, 50, 50)),
	  stamina_label(L"体力", game_data->get_font(JP_DEFAULT)),
          junko_param_label(L"純化度", game_data->get_font(JP_DEFAULT)),
	  rec_label(L"●REC", game_data->get_font(JP_DEFAULT)),
          graze_label(L"グレイズ", game_data->get_font(JP_DEFAULT)),
          graze_counter(0, game_data->get_font(JP_DEFAULT)),
	  window_frame(sf::IntRect(0, 0, 1366, 768),
		       sf::IntRect(32, 32, 960, 704))
{
	test_bullet = new Bullet(GameMaster::texture_table[BULLET_BIG_RED],
				 sf::Vector2f(400, 400), mf::stop, 0, sf::Vector2f(0.05, 0.05), BulletSize::BIG_CIRCLE_RED);
	stamina_label.set_place(0, 50);
	stamina.set_place(0, 80);
	junko_param_label.set_place(0, 110);
	junko_param.set_place(0, 140);
	rec_label.set_place(900, 50);
	rec_label.set_color(sf::Color::Red);
        graze_label.set_place(0, 200);
        graze_counter.set_place(150, 200);

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
			if (std::find_if(
				    container_entire_range(tachie_container),
				    [](Tachie *p) {
					    return p->are_you("udon");
				    }) == std::end(tachie_container)) {
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
					for (u32 i = 0; i < bullets.size();
					     i++) {
						if (running_char.distance(
							    bullets[i]) < 240) {
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
	}

        running_char.move_diff(sf::Vector2f(0, 0));
	stamina.add(1);
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
	static u32 i;

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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
		func_table.add_function_dynamic(
			FunctionCallEssential("shot", get_count()));
	}

	// スケジュールされる予定の弾幕がまだある場合継続
	while (func_table.get_func_sched().size() > i) {
		/*
                 * スケジュールされる予定のカウントに達した
                 */
		if (func_table.get_func_sched().at(i).time <= get_count()) {
                        // 関数生成のためのデータを取り出す
			FunctionCallEssential f_essential =
				func_table.get_func_sched().at(i++);
			// 生成
			std::vector<BulletData *> &&v =
				func_table.call_function(f_essential);
			// 生成されたそれぞれの弾丸データに出現時刻をセットする
                        // 基本的には即時出現
			std::for_each(std::begin(v), std::end(v),
				      [this](BulletData *d) {
					      d->set_appear_time(get_count());
                                              // スケジュールに追加
                                              bullets_sched.add(d);
					});
			} else {
                        /*
                         * この待ち行列（のようなもの）は既にスケジュール時刻でソートされているため、
                         * 先頭が達していない場合は、それ以降すべて達していないことになり、breakする
                         */
				break;
			}
		}
}

void RaceSceneMaster::proceed_bullets_schedule(void)
{
        /*
         * 弾幕データがスケジュールされているか？
         */
	while (bullets_sched.size()) {
                // 出現時間を迎えているか？
		if (bullets_sched.next()->appear_time <= get_count()) {
                        // ターゲットの弾幕データを取り出し
			BulletData *target = bullets_sched.next();

                        // 取り出したので、削除
			bullets_sched.drop();

                        /*
                         * 動的なマクロか？
                         */
			if (target->flags & DYNAMIC_MACRO) {
                                // 動的なマクロを展開
                                // これにより、BulletDataのvectorが得られる
				auto &&gen = macro::expand_dynamic_macro(
					target->original_data, running_char);

                                // 実体化し、表示する弾丸のグループに加える
				for (auto &elem : gen) {
					util::concat_container<
						std::vector<Bullet *> >(
						bullets,
						BulletGenerator::generate(
							elem, running_char,
							get_count()));
				}
			} else {
                                // 実体化し、表示する弾丸のグループに加える
				util::concat_container<std::vector<Bullet *> >(
					bullets, BulletGenerator::generate(
							 target, running_char,
							 get_count()));
			}
		} else {
                        // スケジュールは出現時間でソートされているため、先頭が達していない場合はbreak
			break;
		}
	}
}

void RaceSceneMaster::pre_process(sf::RenderWindow &window)
{
        add_new_functional_bullets_to_schedule();
        proceed_bullets_schedule();

	for (auto &&bullet : bullets) {
		if (bullet->check_conflict(running_char)) {
			bullet->hide();
			junko_param.add(10);
		}
	}

	if (test_bullet->check_conflict(running_char)) {
		puts("CONFLICT");
		test_bullet->hide();
	}

	player_move();

	score_counter.counter_method().add(1);

	for (u32 i = 0; i < bullets.size(); i++) {
		if (bullets[i]->is_finish(
			    sf::IntRect(-1368, -768, 1368 * 3, 768 * 3)) ||
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

	tachie_container.remove_if([](Tachie *p) { return !p->visible(); });
        // 立ち絵の移動
        for(auto &&p : tachie_container){
                p->move(get_count());
		p->effect(get_count());
	}
        
	backgroundTile.scroll(-8);

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

        switch_view("bullets", window);

	for (u32 i = 0; i < bullets.size(); i++) {
		bullets[i]->draw(window);
	}

	test_bullet->draw(window);

	rec_label.draw(window);

	//window_frame.draw(window);

        switch_view("params", window);
	score_counter.draw(window);
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
	return RACE;
}
