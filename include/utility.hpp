#pragma once

#include <random>
#include <vector>
#include <unordered_map>
#include <string>
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
        
        template<class T>
        inline void concat_container(T &c1, T &&c2)
        {
                std::copy(std::begin(c2), std::end(c2), std::back_inserter(c1));
        }

        template<class T>
        inline void concat_container(T &c1, T &c2)
        {
                std::copy(std::begin(c2), std::end(c2), std::back_inserter(c1));
        }

        template<class T>
        using str_hash = std::unordered_map<std::string, T>;
}

#define enum_to_str(var) #var
#define container_entire_range(x) std::begin(x), std::end(x)
