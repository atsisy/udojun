#pragma once

#include "game_component.hpp"

namespace rotate {
        float stop(Rotatable *r, u64 now, u64 begin);

        std::function<float(Rotatable *, u64, u64)>
        constant(float speed);

        std::function<float(Rotatable *, u64, u64)>
	pendulum(float max, u64 period, float offset);
}

