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
#include <forward_list>
#include <chrono>
#include <ctime>
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

                float floating(float min, float max)
                {
                        std::uniform_real_distribution<float> range(min, max);
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

        template <typename ... Args>
        std::string format_string(const std::string &format, Args ... args )
        {
                char *buf = new char[1024];
                std::snprintf(buf, 1024, format.c_str(), args ...);
                std::string ret(buf);
                delete[] buf;
                
                return ret;
        }

        class Date {
        public:
                u16 year;
                u8 month;
                u8 day;
                u8 hour;
                u8 minute;
                u8 second;

                Date(void)
                {
                        this->year = 0;
                        this->month = 0;
                        this->day = 0;
                        this->hour = 0;
                        this->minute = 0;
                        this->second = 0;
                }
                
                Date(void *p)
                {
                        auto now = std::chrono::system_clock::now();
                        auto legacy = std::chrono::system_clock::to_time_t(now);
                        struct tm *c_time = std::localtime(&legacy);

                        this->year = c_time->tm_year + 1900;
                        this->month = c_time->tm_mon + 1;
                        this->day = c_time->tm_mday;
                        this->hour = c_time->tm_hour;
                        this->minute = c_time->tm_min;
                        this->second = c_time->tm_sec;
                }
                
                Date(u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second)
                {
                        this->year = year;
                        this->month = month;
                        this->day = day;
                        this->hour = hour;
                        this->minute = minute;
                        this->second = second;
                }

                std::string to_string(void)
                {
                        return std::string(
                                format_string("%04d", (int)year) +
                                "/" +
                                format_string("%02d", (int)month) +
                                "/" +
                                format_string("%02d", (int)day) +
                                " " +
                                format_string("%02d", (int)hour) +
                                ":" +
                                format_string("%02d", (int)minute) +
                                ":" +
                                format_string("%02d", (int)second)
                                );
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
			move_index_offset(1);
		}

		void up(void)
		{
			move_index_offset(-1);
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

        template<typename T>
        double distance(T x1, T y1, T x2, T y2)
        {
                return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
        }

        template<typename T>
        double abs_value(T x1, T y1)
        {
                return std::sqrt(std::pow(x1, 2) + std::pow(y1, 2));
        }

        std::wstring *utf8_str_to_widechar_str(std::string str);

        template <typename T>
        class FixedCounter {
        private:
                T max;
                T current;

        public:
                FixedCounter(T max, T init)
                {
                        this->max = max;
                        this->current = init;
                }

                T get_max(void)
                {
                        return max;
                }

                T get_current(void)
                {
                        return current;
                }

        };

        template <typename T>
        class VariableCounter {
        private:
                T value;
                
        public:
                VariableCounter(T init)
                {
                        value = init;
                }

                VariableCounter(void)
                {
                        value = 0;
                }
                
                T get_value(void)
                {
                        return value;
                }

                void add(T _value)
                {
                        value += _value;
                }

                void set_value(T _value)
                {
                        this->value = _value;
                }

                T get(void)
                {
                        return this->value;
                }
        };

        template<typename type, template<typename T> class container>
        class GPContainer {
        private:
                container<type> buffer;
                
        public:
                GPContainer(void)
                {}
                
                typename container<type>::iterator begin(void)
                {
                        return std::begin(buffer);
                }
                
                typename container<type>::iterator end(void)
                {
                        return std::end(buffer);
                }

                void add(type t)
                {
                        buffer.push_back(t);
                }

                void add(type &t)
                {
                        buffer.push_back(t);
                }

                void erase(typename container<type>::iterator it)
                {
                        buffer.erase(it);
                }

                container<type> &native_method(void)
                {
                        return buffer;
                }
        };

        inline size_t wstrlen(wchar_t *s)
        {
                size_t count = 0;
                u32 *wide_p = (u32 *)s;
                while(*wide_p++)
                        count++;
                return count;
        }
}

#define enum_to_str(var) #var
#define str_to_idx_sub(str, _enum) do { if (!strcmp(str, enum_to_str(_enum))) { \
                        return _enum; } }while(0)

#define container_entire_range(x) std::begin(x), std::end(x)
#define DEBUG_PRINT_HERE() printf("%s:%d", __FILE__, __LINE__)

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
