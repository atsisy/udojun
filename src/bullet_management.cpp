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

void BulletContainer::draw(sf::RenderWindow &window)
{
	for (Bullet *b : enemy_bullets) {
		b->draw(window);
	}
	for (Bullet *b : player_bullets) {
		b->draw(window);
	}
	for (Bullet *b : special_bullets) {
		b->draw(window);
	}
}
