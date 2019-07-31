#pragma once

#include "character.hpp"
#include "sched.hpp"

class EnemyCharacter : public DrawableCharacter {
private:
        float hp_actual;
        float hp_max;

        DanmakuScheduler danmaku_sched;
        
public:
        EnemyCharacter(CharacterAttribute attribute, sf::Texture *t, sf::Vector2f p,
                       sf::Vector2f scale, u64 begin_count,
                       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                       float hp_max, float hp_init, DanmakuScheduler d_sched);
        float get_hp(void);
	void set_hp(float val);
        void set_hp_max(void);
	void damage(float value);
        bool dead(void);
        void move(u64 count);

        DanmakuCallEssential shot(u64 now);
};

extern DanmakuScheduler danmaku_empty_sched;
