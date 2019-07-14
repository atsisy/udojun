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
	std::vector<Bullet *> actual_bullets;

	void add_function(FunctionCallEssential *e);
	void flush_called_function(u64 now, BulletFuncTable &func_table);
	void schedule_bullet(u64 now, PlayerCharacter &player);
};

class BulletContainer : public DrawableComponent {
    public:
	std::vector<Bullet *> enemy_bullets;
	std::vector<Bullet *> player_bullets;
	std::vector<Bullet *> special_bullets;

	void draw(sf::RenderWindow &window) override;
};
