#pragma once

#include <random>
#include <vector>
#include <unordered_map>
#include <climits>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
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

                static constexpr unsigned min(){ return 0; }
                static constexpr unsigned max(){ return UINT_MAX; }
                
                u32 operator()(){ return random(); }

                u32 operator()(u32 min, u32 max)
		{
			std::uniform_int_distribution<u32> range(min, max);
                        return range(*this);
		}
                
		xor128()
                {
                        std::random_device rd;
                        w = rd();
                }
                xor128(u32 s)
                {
                        w = s;
                }

		using result_type = u32;
	};

	extern xor128 generate_random;

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

	inline double radian_to_degree(double rad)
	{
		return (rad / M_PI) * 180;
	}

	inline double degree_to_radian(double deg)
	{
		return (deg / 180.0) * M_PI;
	}

	template <class T>
        using str_hash = std::unordered_map<std::string, T>;

        template <class T>
        class SelecterImplements {
        private:
                std::vector<T> items;
                std::int32_t index;

                void move_index_offset(std::int32_t offset)
                {
                        index += offset;
                        if(index >= 0){
                                index %= items.size();
                        }else{
                                index = items.size() - (std::abs(index) % items.size());
                        }
                 }

        public:
                SelecterImplements()
                {
                        index = 0;
                }

		void add_item(T item)
		{
			items.push_back(item);
		}

		void down(std::int32_t offset)
                {
                        move_index_offset(-offset);
                }

		void up(std::int32_t offset)
		{
			move_index_offset(offset);
		}

		void down(void)
		{
			move_index_offset(-1);
		}

		void up(void)
		{
			move_index_offset(1);
		}

                T get(void)
                {
                        return items[index];
                }
	};

        class SimpleTimer {
        private:
                std::function<void(void)> fn;
                u64 wakeup;
                u64 id;

        public:
                SimpleTimer(std::function<void(void)> f, u64 wakeup_abs, u64 id)
                        : fn(f)
                {
                        this->wakeup = wakeup_abs;
                        this->id = id;
                }

		SimpleTimer(std::function<void(void)> f, u64 wakeup_offset, u64 now, u64 id)
                        : fn(f)
		{
			this->wakeup = now + wakeup_offset;
			this->id = id;
		}

		bool try_call(u64 current)
                {
                        if(current >= wakeup){
                                fn();
                                return true;
                        }
                        
                        return false;
                }

                u64 get_id(void)
                {
                        return id;
                }
                
        };

        class SimpleTimerList {
        private:
                std::vector<SimpleTimer> timers;
                u64 seq;

        public:
                SimpleTimerList(void)
                {
                        seq = 0;
                }
                
                u64 add_timer(std::function<void(void)> f,
                               u64 wakeup_abs)
		{
			timers.emplace_back(f, wakeup_abs, seq);
                        return seq++;
		}

		u64 add_timer(std::function<void(void)> f,
                               u64 wakeup_offset, u64 now)
		{
			timers.emplace_back(f, wakeup_offset, now, seq);
                        return seq++;
		}

		void check_and_call(u64 current)
                {
                        for(size_t i = 0;i < timers.size();i++){
                                if(timers[i].try_call(current)){
                                        std::swap(timers[i], timers.back());
                                        timers.pop_back();
                                        i--;
                                }
                        }
                }

                void cancel(u64 id)
                {
                        for (size_t i = 0; i < timers.size(); i++) {
                                if (timers[i].get_id() == id) {
                                        std::swap(timers[i],
                                                  timers.back());
                                        timers.pop_back();
                                        break;
                                }
                        }
                }
                
        };
}

#define enum_to_str(var) #var
#define str_to_idx_sub(str, _enum) do { if (!strcmp(str, enum_to_str(_enum))) { \
                        return _enum; } }while(0)

#define container_entire_range(x) std::begin(x), std::end(x)
#define DEBUG_PRINT_HERE() printf("%s:%d", __FILE__, __LINE__)
