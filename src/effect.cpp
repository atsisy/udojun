#include "effect.hpp"

#include <iostream>

std::function<void(MoveObject *, u64, u64)> effect::fade_in(u64 distance)
{
	return [=](MoveObject *obj, u64 now, u64 begin){
                       float r;
		       if (now - begin > distance) {
			       r = 255;
		       }else{
			       r = 255.0 * (float)((float)(now - begin) /
						   (float)distance);
		       }
		       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_in(u64 distance, u64 called)
{
	return [=](MoveObject *obj, u64 now, u64 begin){
                       float r;
                       begin = called;
		       if (now - begin > distance) {
			       r = 255;
		       }else{
			       r = 255.0 * (float)((float)(now - begin) /
						   (float)distance);
		       }
		       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_out(u64 distance)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       float r;
                       if (now - begin > distance) {
                               r = 0;
                       } else {
                               r = 255.0 *
                                       (1 - (float)((float)(now - begin) / (float)distance));
                       }
                       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_out(u64 distance, u64 called)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       float r;
                       begin = called;
                       if (now - begin > distance) {
                               r = 0;
                       } else {
                               r = 255.0 *
                                       (1 - (float)((float)(now - begin) / (float)distance));
                       }
                       obj->set_alpha(r);
               };
}


std::function<void(MoveObject *, u64, u64)> effect::kill_at(u64 time)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       if (now == (time + begin)) {
                               obj->hide();
                       }
	};
}

std::function<void(MoveObject *, u64, u64)> effect::kill_at(u64 time, u64 called)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       begin = called;
                       if (now == (time + begin)) {
                               obj->hide();
                       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::scale_effect(sf::Vector2f init_scale, sf::Vector2f end_scale, u64 time)
{
        float unit_x = (end_scale.x - init_scale.x) / (float)(time);
        float unit_y = (end_scale.y - init_scale.y) / (float)(time);
        
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - begin;
                       if(past <= time){
                               sf::Vector2f next_scale = init_scale + sf::Vector2f(unit_x * past, unit_y * past);
                               obj->set_scale(next_scale);
                       }
               };
}
