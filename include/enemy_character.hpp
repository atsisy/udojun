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
        std::vector<std::function<void(MoveObject *, u64, u64)> > effects;
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
        EnemyCharacterSchedule(GameData *game_data, std::vector<const char *> path_vec);

        void push_back(EnemyCharacterMaterial enemy_material);
        void push_back(std::vector<EnemyCharacterMaterial> materials);
        EnemyCharacterMaterial get_front(void);
        void pop_front(void);
        size_t size(void);
        EnemyCharacterMaterial at(int index);
        void sort(void);
};


class DanmakuCallEssential {
public:
        FunctionCallEssential func_essential;
        u64 time_limit;
        DanmakuType type;
        std::wstring *danmaku_name;
        EnemyCharacterSchedule enemy_sched;
        
        DanmakuCallEssential(FunctionCallEssential fe, u64 sec,
                             DanmakuType type,
                             std::wstring *name,
                             std::string enemy_sched_json,
                             GameData *game_data);
};

class DanmakuScheduler {
private:
        std::vector<DanmakuCallEssential> schedule;
        
public:
        DanmakuScheduler(std::vector<DanmakuCallEssential> s);
        bool function_is_coming(u64 count);
        DanmakuCallEssential drop_top(void);
	DanmakuCallEssential top(void);
        size_t size(void);
        void push_back(DanmakuCallEssential e);
};


class EnemyCharacter : public DrawableCharacter {
private:
        static SHOT_MASTER_ID NEXT_SHOT_MASTER_ID;
        float hp_actual;
        float hp_max;
        bool damage_enable;
        bool dead_flag;
        SHOT_MASTER_ID id;

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
        void make_dead(void);
        bool hp_zero(void);
        void move(u64 count);

        void damage_on(void);
        void damage_off(void);

        
        std::optional<FunctionCallEssential> shot(u64 now);
        SHOT_MASTER_ID get_shot_master_id(void);
};

extern DanmakuScheduler danmaku_empty_sched;
