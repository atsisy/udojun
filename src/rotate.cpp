#include <cmath>
#include "rotate_func.hpp"

namespace rotate {
        float stop(Rotatable *r, u64 now, u64 begin)
        {
                return r->get_angle();
        }

        std::function<float(Rotatable *, u64, u64)>
	pendulum(float max, u64 period, float offset)
	{
                return [=](Rotatable *bullet, u64 now_lmd, u64 begin_lmd){
                               float pos = (float)(now_lmd - begin_lmd) / (float)period;
                               return max * std::cos(pos) + offset;
                       };
        }
}
