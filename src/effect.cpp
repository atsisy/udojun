#include "effect.hpp"

#include <iostream>

std::function<void(MoveObject *, u64, u64)> effect::fade_in(u64 distance)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
		obj->set_alpha(255.0 * (float)((float)(now - begin) /
						      (float)distance));
	};
}

std::function<void(MoveObject *, u64, u64)> effect::fade_out(u64 distance)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       obj->set_alpha(255.0 * (float)(1.0 - ((float)(now - begin) / (float)distance)));
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
