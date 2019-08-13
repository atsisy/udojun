#pragma once

#include "character.hpp"
#include "sched.hpp"
#include <optional>

class GameData;

class EnemyCharacterMaterial {
public:
        std::string name;
        TextureID txid;
        sf::Vector2f point;
        sf::Vector2f scale;
        std::function<sf::Vector2f(MoveObject *, u64, u64)> move_func;
        std::function<float(Rotatable *, u64, u64)> rot_func;
        float max_hp;
        float init_hp;
        std::vector<FunctionCallEssential> shot_data;
        float radius;
        u64 time;
};

class EnemyCharacterSchedule {
private:
        std::vector<EnemyCharacterMaterial> data_list;

public:
        EnemyCharacterSchedule(GameData *game_data, const char *path);

        EnemyCharacterMaterial get_front(void);
        void pop_front(void);
        size_t size(void);
        EnemyCharacterMaterial at(int index);
        void sort(void);
};

class EnemyCharacter : public DrawableCharacter {
private:
        float hp_actual;
        float hp_max;
        bool damage_enable;

        std::vector<FunctionCallEssential> shot_data;
        
public:
        EnemyCharacter(CharacterAttribute attribute, sf::Texture *t, sf::Vector2f p,
                       sf::Vector2f scale, u64 begin_count,
                       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                       std::function<float(Rotatable *, u64, u64)> r_fn,
                       float hp_max, float hp_init,
                bool damage_flag);
        EnemyCharacter(EnemyCharacterMaterial material, u64 time);
        float get_hp(void);
	void set_hp(float val);
        void set_hp_max(void);
	void damage(float value);
        bool dead(void);
        void move(u64 count);

        void damage_on(void);
        void damage_off(void);

        
        std::optional<FunctionCallEssential> shot(u64 now);
};

extern DanmakuScheduler danmaku_empty_sched;
