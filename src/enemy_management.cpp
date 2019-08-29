#include "gm.hpp"
#include "effect.hpp"

EnemyManager::EnemyManager(void)
{}

MoveObject *EnemyManager::kill_enemy_with_normal_effect(EnemyCharacter *p, u64 now)
{
        auto bomb = new MoveObject(
                GameMaster::texture_table[MIST1],
                p->get_place(),
                mf::stop,
                rotate::stop,
                now);
        bomb->add_effect({ effect::kill_at(60), effect::fade_out(60),
                           effect::scale_effect(sf::Vector2f(0.03, 0.03), sf::Vector2f(0.2, 0.2), 60) });
        puts("dead");
        p->add_effect({ effect::fade_out(3, now), effect::kill_at(3, now) });
        p->make_dead();

        return bomb;
}

std::vector<MoveObject *> EnemyManager::kill_all_enemy_with_normal_effect(
        std::forward_list<EnemyCharacter *> &p_vec,
        u64 now)
{
        std::vector<MoveObject *> effects;
        
        for(EnemyCharacter *p : p_vec){
                effects.push_back(kill_enemy_with_normal_effect(p, now));
        }

        return effects;
}
