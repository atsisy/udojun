#pragma once

#include <random>
#include "types.hpp"

namespace util {
        inline float count_to_second(u64 c1, u64 c2, u64 fps)
        {
                return (float)(c1 - c2) / (float)fps;
        }

        class xor128 {
        private:
                u32 x = 123456789, y = 362436069u, z = 521288629, w;
                u32 random()
                {
                        u32 t;
                        t = x ^ (x << 11);
                        x = y;
                        y = z;
                        z = w;
                                
                        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
                }
                
        public:
                u32 operator()(){ return random(); }
                xor128()
                {
                        std::random_device rd;
                        w = rd();
                }
                xor128(u32 s)
                {
                        w = s;
                }
        };
}

#define enum_to_str(var) #var
