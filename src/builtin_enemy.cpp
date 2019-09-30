#include "gm.hpp"

namespace builtin_enemy {
        std::vector<EnemyCharacterMaterial> ghost_group1(GameData *game_data)
        {
                std::vector<EnemyCharacterMaterial> ret;
                EnemyCharacterMaterial *p = game_data->get_enemy_material("GHOST_ENEMY");
                
                for(int i = 0;i < 70;i++){
                        p->time = 800 + (i * 8);
                        p->point.y = 100 + util::generate_random(0, 15);
                        ret.push_back(*p);
                }

                return ret;
        }
}

std::vector<std::function<std::vector<EnemyCharacterMaterial>(GameData *)>> builtin_enemy_funcs = {
        builtin_enemy::ghost_group1
};
