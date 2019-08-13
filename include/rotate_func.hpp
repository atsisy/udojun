#pragma once

#include "game_component.hpp"
#include "utility.hpp"
#include <iostream>
#include "picojson.h"

namespace rotate {

        enum RotationFunctionID {
                ROTF_STOP = 0,
                ROTF_CONSTANT,
                ROTF_PENDULUM,
                UNKNOWN_ROTF_ID,
        };

        inline RotationFunctionID str_to_rotf_id(const char *str)
        {
                str_to_idx_sub(str, ROTF_STOP);
                str_to_idx_sub(str, ROTF_CONSTANT);
                str_to_idx_sub(str, ROTF_PENDULUM);

                std::cout << "Unknown Rotation Function ID: " << str << std::endl;
        
                return UNKNOWN_ROTF_ID;
        }
        
        float stop(Rotatable *r, u64 now, u64 begin);

        std::function<float(Rotatable *, u64, u64)>
        constant(float speed);

        std::function<float(Rotatable *, u64, u64)>
	pendulum(float max, u64 period, float offset);

        inline std::function<float(Rotatable *, u64, u64)>
        select_rotation_function(RotationFunctionID id, picojson::object &data)
        {
                switch(id){
                case ROTF_STOP:
                        return stop;
                case ROTF_CONSTANT:
                        return constant(data["speed"].get<double>());
                case ROTF_PENDULUM:
                        return pendulum(data["max"].get<double>(),
                                        data["period"].get<double>(),
                                        data["offset"].get<double>());
                default:
                        return stop;
                }
        }
}

