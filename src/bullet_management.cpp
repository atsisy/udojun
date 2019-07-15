#include "bullet_management.hpp"

void BulletPipeline::add_function(FunctionCallEssential *e)
{
        func_sched.add_function(e);
}

void BulletPipeline::flush_called_function(u64 now, BulletFuncTable &func_table)
{
        // スケジュールされる予定の弾幕がまだある場合継続
        while (func_sched.size()) {
		/*
                 * スケジュールされる予定のカウントに達した
                 */
		if (func_sched.head()->time <= now) {
                        // 関数生成のためのデータを取り出す
			FunctionCallEssential *f_essential =
                                func_sched.pop();
			// 生成
			std::vector<BulletData *> &&v =
				func_table.call_function(*f_essential);
			// 生成されたそれぞれの弾丸データに出現時刻をセットする
                        // 基本的には即時出現
			std::for_each(std::begin(v), std::end(v),
				      [this, now](BulletData *d) {
					      d->set_appear_time(now);
                                              // スケジュールに追加
                                              this->bullet_sched.add(d);
                                      });
                        delete f_essential;
                } else {
                        /*
                         * この待ち行列（のようなもの）は既にスケジュール時刻でソートされているため、
                         * 先頭が達していない場合は、それ以降すべて達していないことになり、breakする
                         */
                        break;
                }
        }
}

void BulletPipeline::schedule_bullet(u64 now, PlayerCharacter &player)
{
        /*
         * 弾幕データがスケジュールされているか？
         */
	while (bullet_sched.size()) {
                // 出現時間を迎えているか？
		if (bullet_sched.next()->appear_time <= now) {
                        // ターゲットの弾幕データを取り出し
			BulletData *target = bullet_sched.next();

                        // 取り出したので、削除
			bullet_sched.drop();

                        /*
                         * 動的なマクロか？
                         */
			if (target->flags & DYNAMIC_MACRO) {
                                // 動的なマクロを展開
                                // これにより、BulletDataのvectorが得られる
				auto &&gen = macro::expand_dynamic_macro(
					target->original_data, player);

                                // 実体化し、表示する弾丸のグループに加える
				for (auto &elem : gen) {
					util::concat_container<
						std::vector<Bullet *> >(
						actual_bullets,
						BulletGenerator::generate(
							elem, player,
							now));
				}
			} else {
                                // 実体化し、表示する弾丸のグループに加える
				util::concat_container<std::vector<Bullet *> >(
					actual_bullets, BulletGenerator::generate(
							 target, player,
							 now));
			}
		} else {
                        // スケジュールは出現時間でソートされているため、先頭が達していない場合はbreak
			break;
		}
	}
}

void BulletPipeline::clear_all_bullets(void)
{
        //func_table.clear_func_sched();

	for (Bullet *b : actual_bullets) {
		delete b;
	}
        actual_bullets.clear();
	bullet_sched.clear();
}

void BulletPipeline::draw(sf::RenderWindow &window)
{
        for(Bullet *b : actual_bullets){
                b->draw(window);
        }
}

void BulletPipeline::direct_insert_bullet(Bullet *bullet)
{
        actual_bullets.push_back(bullet);
}

void BulletPipelineContainer::draw(sf::RenderWindow &window)
{
        player_pipeline.draw(window);
        enemy_pipeline.draw(window);
        special_pipeline.draw(window);
}

void BulletPipelineContainer::all_flush_called_function(
        u64 now,
        BulletFuncTable &func_table)
{
        player_pipeline.flush_called_function(now, func_table);
        enemy_pipeline.flush_called_function(now, func_table);
        special_pipeline.flush_called_function(now, func_table);
}

void BulletPipelineContainer::all_schedule_bullet(
        u64 now,
        PlayerCharacter &player)
{
        player_pipeline.schedule_bullet(now, player);
        enemy_pipeline.schedule_bullet(now, player);
        special_pipeline.schedule_bullet(now, player);
}
