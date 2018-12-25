#include <functional>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "types.hpp"
#include "utility.hpp"

class TimeFunction {
protected:
        u64 begin;
        u64 fps;
        
public:
        TimeFunction(u64 begin, u64 fps)
        {
                this->begin = begin;
                this->fps = fps;
        }
        
        virtual float operator()(u64 count);
        
};

class SinFunction : public TimeFunction {
private:
        float bias;
        
public:
        SinFunction(float bias)
        {
                this->bias = bias;
        }
        
        float operator()(u64 count)
        {
                return bias * std::sin(util::count_to_second(count, begin, fps));
        }
};

inline std::function<sf::Vector2f(sf::Vector2f &, u64)> concat_function_2d(TimeFunction *fx, TimeFunction *fy)
{
        return [=](sf::Vector2f &p, u64 count){
                       return sf::Vector2f(p.x + (*fx)(count), p.y + (*fy)(count));
               };
}
