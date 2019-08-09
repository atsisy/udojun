#pragma once

#include "character.hpp"
#include "sched.hpp"

class EnemyCharacter : public DrawableCharacter {
private:
        float hp_actual;
        float hp_max;
        bool damage_enable;

        DanmakuScheduler danmaku_sched;
        
public:
        EnemyCharacter(CharacterAttribute attribute, sf::Texture *t, sf::Vector2f p,
                       sf::Vector2f scale, u64 begin_count,
                       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                       std::function<float(Rotatable *, u64, u64)> r_fn,
                       float hp_max, float hp_init, DanmakuScheduler d_sched,
                bool damage_flag);
        float get_hp(void);
	void set_hp(float val);
        void set_hp_max(void);
	void damage(float value);
        bool dead(void);
        void move(u64 count);

        void damage_on(void);
        void damage_off(void);

        DanmakuCallEssential shot(u64 now);
};

extern DanmakuScheduler danmaku_empty_sched;
