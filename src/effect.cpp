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
