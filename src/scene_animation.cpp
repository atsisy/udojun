#include "gm.hpp"


SceneAnimation::SceneAnimation()
{}

void SceneAnimation::add_animation_object(MoveObject *obj)
{
        move_objects.push_back(obj);
}

void SceneAnimation::flush_effect_buffer(u64 count)
{
	for (MoveObject *obj : move_objects) {
		obj->effect(count);
	}
}

void SceneAnimation::draw_animation(sf::RenderWindow &window)
{
        for(MoveObject *obj : move_objects){
                obj->draw(window);
        }
}
