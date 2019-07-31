#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include "utility.hpp"
#include "character.hpp"
#include "picojson.h"
#include "textures.hpp"
#include "game_component.hpp"

#define DEF_MOVE_FUNC(name, ...) std::function<sf::Vector2f(MoveObject *, u64, u64)> \
        name(__VA_ARGS__);

namespace mf {
        
        sf::Vector2f stop(MoveObject *, u64 now, u64 begin);
        DEF_MOVE_FUNC(sin, float bias, float dx);
        DEF_MOVE_FUNC(cos, float bias, float dx);
        DEF_MOVE_FUNC(linear, float bias, float dx, float c);
	DEF_MOVE_FUNC(getting_slower, float init_speed, float angle, u64 limit);
	DEF_MOVE_FUNC(up, float c);
        DEF_MOVE_FUNC(aim_self_linear, sf::Vector2f &target, float speed, sf::Vector2f begin_point);
	DEF_MOVE_FUNC(tachie_move_constant, float dx, float dy);
        DEF_MOVE_FUNC(move_point_constant, sf::Vector2f dest, sf::Vector2f now, u64 start_time, u64 end_time);
	DEF_MOVE_FUNC(uzumaki, sf::Vector2f origin,
		      sf::Vector2f begin, float speed, float angle,
		      float r_bias);
	DEF_MOVE_FUNC(active_homing, sf::Vector2f origin, float speed, sf::Vector2f *target);
}


enum BulletFunctionID {
        SIN_1 = 0,
        COS_1,
        LINEAR,
        AIM_SELF_LINEAR,
        UP,
        UZUMAKI,
        SLOWER1,
        ACTIVE_HOMING,
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
        }else if(!strcmp(str, enum_to_str(UZUMAKI))){
                return UZUMAKI;
	}else if (!strcmp(str, enum_to_str(SLOWER1))) {
		return SLOWER1;
	}

	std::cout << "Unknown Bullet Function ID: " << str << std::endl;
        
        return UNKNOWN_BFID;
}

constexpr u64 BEGIN_AT_RUNNING_CHARACTER_X = 0x01;
constexpr u64 BEGIN_AT_RUNNING_CHARACTER_Y = 0x02;
constexpr u64 DYNAMIC_MACRO = 0x04;
constexpr u64 AIMING_SELF = 0x08;
constexpr u64 LASER_BULLET = 0x10;

class BulletData {
private:
        void init_texture_data(TextureID id);
        
public:
        BulletFunctionID id;
        std::function<sf::Vector2f(MoveObject *, u64, u64)> func;
        u64 appear_time;
        u64 offset;
        u64 flags;
        sf::Vector2f appear_point;
        sf::Texture *texture;
        sf::Vector2f scale;
        float radius;
        picojson::object original_data;
        bool conflictable;
        bool grazable;
        float init_rotation;

        BulletData(picojson::object &json_data);
        BulletData(picojson::object &json_data, u64 flg);
        
        BulletData(BulletFunctionID id, TextureID tid,
                   std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                   u64 time, sf::Vector2f appear_point, float init_rotate = 0);

        BulletData()
        {}

        Bullet *generate(DrawableCharacter &running_char, u64 count);

        void set_appear_time(u64 current);
};

class BulletGenerator {

private:
        static std::vector<Bullet *> generate_laser(BulletData *data, DrawableCharacter &running_char, u64 count);
        static std::vector<Bullet *> generate_bullet(BulletData *data, DrawableCharacter &running_char, u64 count);
        
public:
        static std::vector<Bullet *> generate(BulletData *data, DrawableCharacter &running_char, u64 count);
};


inline std::function<sf::Vector2f(MoveObject *, u64, u64)>
select_bullet_function(BulletFunctionID id, picojson::object &data)
{
        switch(id){
        case SIN_1:
                return mf::sin(data["bias"].get<double>(), data["dx"].get<double>());
        case COS_1:
                return mf::cos(data["bias"].get<double>(), data["dx"].get<double>());
        case LINEAR:
                return mf::linear(data["bias"].get<double>(), data["dx"].get<double>(), data["c"].get<double>());
        case SLOWER1:
		return mf::getting_slower(data["speed"].get<double>(),
					  data["angle"].get<double>(),
					  0);
        case UP:
                return mf::up(data["c"].get<double>());
	case UZUMAKI:
		return mf::uzumaki(sf::Vector2f(data["origin.x"].get<double>(),
						data["origin.y"].get<double>()),
				   sf::Vector2f(data["x"].get<double>(),
						data["y"].get<double>()),
				   data["speed"].get<double>(),
				   data["angle"].get<double>(),
				   data["r-bias"].get<double>());
	default:
                return mf::stop;
        }
}

#define TAKE_DEFAULT_ARG(obj, key, type, val) (obj.find(key) == std::end(obj) ? val : obj[(key)].get<type>())
