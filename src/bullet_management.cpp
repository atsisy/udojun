#include "bullet_management.hpp"
#include "gm.hpp"

void BulletPipeline::add_function(FunctionCallEssential *e)
{
        func_sched.add_function(e);
}

void BulletPipeline::clear_killed_shot(SHOT_MASTER_ID id)
{
        func_sched.clear_func_sched(id);
        bullet_sched.clear(id);
}

void BulletPipeline::flush_called_function(u64 now, BulletFuncTable &func_table)
{
        func_sched.remove_if(
                [&](FunctionCallEssential *f_essential){
                        /*
                         * スケジュールされる予定のカウントに達した
                         */
                        if (f_essential->time <= now) {
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
                                return true;
                        } else {
                                /*
                                 * 発射時刻では無いので、falseを返してremoveしない
                                 */
                                return false;
                        }        
                });
}

void BulletPipeline::schedule_bullet(u64 now, PlayerCharacter &player, DrawableCharacter &udon)
{
        bullet_sched.remove_if(
                [&, this](BulletData *target){
                        // 出現時間を迎えているか？
                        if (target->appear_time <= now) {
                                /*
                                 * 動的なマクロか？
                                 */
                                if (target->flags & DYNAMIC_MACRO) {
                                        // 動的なマクロを展開
                                        // これにより、BulletDataのvectorが得られる
                                        auto &&gen = macro::expand_dynamic_macro(
                                                target->original_data, &player, &udon, target);
                                        
                                        // 実体化し、表示する弾丸のグループに加える
                                        for (auto &elem : gen) {
                                                auto generated = BulletGenerator::generate(
                                                        elem, player,
                                                        now);
                                                std::copy(
                                                        std::begin(generated),
                                                        std::end(generated),
                                                        std::back_inserter(actual_bullets));
                                        }
                                } else if (target->flags & LASER_BULLET) {
                                        /*
                                         * レーザーか？
                                         */
                                        actual_lasers.push_back(BulletGenerator::generate_laser(target->original_data,
                                                                                                target, udon, now));
                                } else {
                                        // 実体化し、表示する弾丸のグループに加える
                                        auto generated = BulletGenerator::generate(
                                                target, player,
                                                now);
                                        std::copy(std::begin(generated), std::end(generated), std::back_inserter(actual_bullets));
                                }
                                //GameMaster::sound_player->add(sound::SHOOT1);
                                return true;
                        } else {
                                return false;
                        }
                });
}

void BulletPipeline::clear_all_bullets(void)
{
        //func_table.clear_func_sched();

	for (Bullet *b : actual_bullets) {
		delete b;
	}
        for (Laser *l : actual_lasers) {
		delete l;
	}
        actual_bullets.clear();
        actual_lasers.clear();
	bullet_sched.clear();
}

void BulletPipeline::draw(sf::RenderWindow &window)
{
        for(Bullet *b : actual_bullets){
                b->draw(window);
        }

        for(Laser *p : actual_lasers){
                p->draw(window);
        }
}

void BulletPipeline::direct_insert_bullet(Bullet *bullet)
{
        actual_bullets.push_back(bullet);
}

void BulletPipeline::direct_insert_bullet_data(std::vector<BulletData *> data)
{
        for(BulletData *p : data){
                bullet_sched.add(p);
        }
}

void BulletPipeline::move_all_bullets(u64 count)
{
        for(auto &&p : actual_bullets){
                p->move(count);
                p->effect(count);
        }

        for(auto &&p : actual_lasers){
                p->move(count);
        }
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
        PlayerCharacter &player, DrawableCharacter &udon)
{
        player_pipeline.schedule_bullet(now, player, udon);
        enemy_pipeline.schedule_bullet(now, player, udon);
        special_pipeline.schedule_bullet(now, player, udon);
}
