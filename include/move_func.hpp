#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include "utility.hpp"
#include "character.hpp"
#include "picojson.h"
#include "game_component.hpp"

#define DEF_MOVE_FUNC(name, ...) std::function<sf::Vector2f(sf::Vector2f&, u64, u64)> \
        name(__VA_ARGS__);

namespace mf {
        
        sf::Vector2f stop(sf::Vector2f &p, u64 now, u64 begin);
        DEF_MOVE_FUNC(sin, float bias, float dx);
        DEF_MOVE_FUNC(cos, float bias, float dx);
        DEF_MOVE_FUNC(linear, float bias, float dx, float c);
        DEF_MOVE_FUNC(up, float c);
        DEF_MOVE_FUNC(aim_self_linear, sf::Vector2f &target, float speed, sf::Vector2f &begin_point);
}


enum BulletFunctionID {
        SIN_1 = 0,
        COS_1,
        LINEAR,
        AIM_SELF_LINEAR,
        UP,
        UNKNOWN_BFID,
};

inline BulletFunctionID str_to_bfid(const char *str)
{
        if(!strcmp(str, enum_to_str(SIN_1))){
                return SIN_1;
        }else if(!strcmp(str, enum_to_str(COS_1))){
                return COS_1;
        }else if(!strcmp(str, enum_to_str(LINEAR))){
                return LINEAR;
        }else if(!strcmp(str, enum_to_str(UP))){
                return UP;
        }else if(!strcmp(str, enum_to_str(UP))){
                return UP;
        }

        std::cout << "Unknown Bullet Function ID: " << str << std::endl;
        
        return UNKNOWN_BFID;
}

constexpr u64 BEGIN_AT_RUNNING_CHARACTER_X = 0x01;
constexpr u64 BEGIN_AT_RUNNING_CHARACTER_Y = 0x02;
constexpr u64 DYNAMIC_MACRO = 0x04;
constexpr u64 AIMING_SELF = 0x08;

class BulletData {        
public:
        BulletFunctionID id;
        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)> func;
        u64 appear_time;
        u64 offset;
        u64 flags;
        sf::Vector2f appear_point;
        picojson::object original_data;

        BulletData(picojson::object &json_data);
        BulletData(picojson::object &json_data, u64 flg);
        
        BulletData(BulletFunctionID id, std::function<sf::Vector2f(sf::Vector2f&, u64, u64)> f,
                   u64 time, sf::Vector2f appear_point)
        {
                this->id = id;
                this->func = f;
                this->offset = time;
                this->appear_point = appear_point;
                this->flags = 0;
        }

        BulletData()
        {}

        Bullet *generate(DrawableCharacter &running_char, u64 count);

        void set_appear_time(u64 current);
};

inline std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
select_bullet_function(BulletFunctionID id, picojson::object &data)
{
        switch(id){
        case SIN_1:
                return mf::sin(data["bias"].get<double>(), data["dx"].get<double>());
        case COS_1:
                return mf::cos(data["bias"].get<double>(), data["dx"].get<double>());
        case LINEAR:
                return mf::linear(data["bias"].get<double>(), data["dx"].get<double>(), data["c"].get<double>());
        case UP:
                return mf::up(data["c"].get<double>());
        default:
                return mf::stop;
        }
}

#define TAKE_DEFAULT_ARG(obj, key, type, val) (obj.find(key) == std::end(obj) ? val : obj[(key)].get<type>())
