#pragma once

#include "game_component.hpp"
#include "sched.hpp"
#include "programable.hpp"
#include "character.hpp"

class BulletPipeline {
    private:
    public:
	FunctionScheduler func_sched;
	BulletScheduler bullet_sched;
	std::list<Bullet *> actual_bullets;

	void add_function(FunctionCallEssential *e);
	void flush_called_function(u64 now, BulletFuncTable &func_table);
	void schedule_bullet(u64 now, PlayerCharacter &player, DrawableCharacter &udon);
        void clear_all_bullets(void);
        
        void draw(sf::RenderWindow &window);
        void direct_insert_bullet(Bullet *bullet);
        
        void direct_insert_bullet_data(std::vector<BulletData *> data);

        void clear_killed_shot(SHOT_MASTER_ID id);
};

class BulletPipelineContainer {
    public:
        BulletPipeline player_pipeline;
	BulletPipeline enemy_pipeline;
	BulletPipeline special_pipeline;

	void all_flush_called_function(u64 now, BulletFuncTable &func_table);
        void all_schedule_bullet(u64 now, PlayerCharacter &player, DrawableCharacter &udon);
	void draw(sf::RenderWindow &window);
};
