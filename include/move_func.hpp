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
        DEF_MOVE_FUNC(sin, float bias, float y_origin, float dx);
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
        DEF_MOVE_FUNC(vector_linear, sf::Vector2f speed);
        DEF_MOVE_FUNC(vector_linear_with_noise, sf::Vector2f speed);
        DEF_MOVE_FUNC(curve, sf::Vector2f begin, sf::Vector2f middle, sf::Vector2f end, u64 time);
        DEF_MOVE_FUNC(curve2, sf::Vector2f begin, sf::Vector2f end,
                      sf::Vector2f begin_velocity, sf::Vector2f end_velocity, u64 time); 
        DEF_MOVE_FUNC(same_position, MoveObject *p);
        DEF_MOVE_FUNC(step, sf::Vector2f begin, sf::Vector2f first_speed, u64 limit,
                      u64 stop, sf::Vector2f second_speed);
        DEF_MOVE_FUNC(shadow_vector_linear, sf::Vector2f speed, u64 enable_time, u64 disable_time);
        DEF_MOVE_FUNC(ratio_step, sf::Vector2f goal, float ratio);
        DEF_MOVE_FUNC(accelerating, sf::Vector2f init_speed, sf::Vector2f accel, sf::Vector2f x_speed_range, sf::Vector2f y_speed_range);
        DEF_MOVE_FUNC(udon_double_step_getting_slower, float init_speed, float angle, u64 change_course_time, float sub_angle);
        DEF_MOVE_FUNC(random_vibration, sf::Vector2f origin, u64 r);

        DEF_MOVE_FUNC(random_turning, sf::Vector2f *origin, float speed, u64 r, u64 width);

	DEF_MOVE_FUNC(nokogiri, sf::Vector2f v1, sf::Vector2f v2, u64 time_offset);
}


enum BulletFunctionID {
        SIN_1 = 0,
        STOP,
        COS_1,
        LINEAR,
        NOKOGIRI,
        VECTOR_LINEAR,
        AIM_SELF_LINEAR,
        UP,
        UZUMAKI,
        SLOWER1,
        UDON_SLOWER,
        BEZIER_CURVE,
        ACTIVE_HOMING,
        STEP,
        SHADOW_VECTOR_LINEAR,
        UNKNOWN_BFID,
        ACCELERATING,
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
	}else if (!strcmp(str, enum_to_str(VECTOR_LINEAR))) {
		return VECTOR_LINEAR;
	}

        str_to_idx_sub(str, BEZIER_CURVE);
        str_to_idx_sub(str, STOP);
        str_to_idx_sub(str, STEP);
        str_to_idx_sub(str, SHADOW_VECTOR_LINEAR);
        str_to_idx_sub(str, ACCELERATING);
        str_to_idx_sub(str, UDON_SLOWER);
        str_to_idx_sub(str, NOKOGIRI);

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

        void __constructor_for_dynamic_macro(picojson::object &json_data, u64 flg);
        void __constructor_for_laser(picojson::object &json_data, u64 flg);
        
        
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
        SHOT_MASTER_ID shot_master_id;

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

class Laser;

class BulletGenerator {

private:
        static std::vector<Bullet *> generate_bullet(BulletData *data, DrawableCharacter &running_char, u64 count);
        
public:
        static std::vector<Bullet *> generate(BulletData *data, DrawableCharacter &running_char, u64 count);
        static Laser *generate_laser(picojson::object &original_data, BulletData *data, DrawableCharacter &running_char, u64 count);
};

class FunctionCallEssential {
public:
        std::string func_name;
        u64 time;
        SHOT_MASTER_ID shot_master_id;
        sf::Vector2f origin;

	FunctionCallEssential(std::string fn, u64 t, SHOT_MASTER_ID sm_id,
                              sf::Vector2f origin_p = sf::Vector2f(0, 0));
};


inline std::function<sf::Vector2f(MoveObject *, u64, u64)>
select_bullet_function(BulletFunctionID id, picojson::object &data)
{
        switch(id){
        case STOP:
                return mf::stop;
        case SIN_1:
                return mf::sin(data["bias"].get<double>(), data["y_origin"].get<double>(), data["dx"].get<double>());
        case COS_1:
                return mf::cos(data["bias"].get<double>(), data["dx"].get<double>());
        case LINEAR:
                return mf::linear(data["bias"].get<double>(), data["dx"].get<double>(), data["c"].get<double>());
        case VECTOR_LINEAR:
                return mf::vector_linear(sf::Vector2f(data["speed_x"].get<double>(), data["speed_y"].get<double>()));
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
        case BEZIER_CURVE:
        {
                auto &begin_object = data["begin"].get<picojson::object>();
                sf::Vector2f begin(begin_object["x"].get<double>(), begin_object["y"].get<double>());
                auto &middle_object = data["middle"].get<picojson::object>();
                sf::Vector2f middle(middle_object["x"].get<double>(), middle_object["y"].get<double>());
                auto &end_object = data["end"].get<picojson::object>();
                sf::Vector2f end(end_object["x"].get<double>(), end_object["y"].get<double>());
                return mf::curve(begin, middle, end, data["time"].get<double>());
        }
        case STEP:
        {
                auto &begin_object = data["begin"].get<picojson::object>();
                sf::Vector2f begin(begin_object["x"].get<double>(), begin_object["y"].get<double>());
                auto &first_speed_object = data["first_speed"].get<picojson::object>();
                sf::Vector2f first_speed(first_speed_object["x"].get<double>(), first_speed_object["y"].get<double>());
                auto &second_speed_object = data["second_speed"].get<picojson::object>();
                sf::Vector2f second_speed(second_speed_object["x"].get<double>(), second_speed_object["y"].get<double>());
                return mf::step(
                        begin,
                        first_speed,
                        data["first_limit"].get<double>(),
                        data["stop"].get<double>(),
                        second_speed);
        }
        case ACCELERATING:
        {
                auto &init_speed_object = data["init_speed"].get<picojson::object>();
                sf::Vector2f init(init_speed_object["x"].get<double>(), init_speed_object["y"].get<double>());
                auto &acc_object = data["accel"].get<picojson::object>();
                sf::Vector2f accel(acc_object["x"].get<double>(), acc_object["y"].get<double>());
                auto &x_range_object = data["x_speed_range"].get<picojson::object>();
                sf::Vector2f x_speed_range(x_range_object["min"].get<double>(), x_range_object["max"].get<double>());
                auto &y_range_object = data["y_speed_range"].get<picojson::object>();
                sf::Vector2f y_speed_range(y_range_object["min"].get<double>(), y_range_object["max"].get<double>());
                return mf::accelerating(init, accel, x_speed_range, y_speed_range);
        }
        case NOKOGIRI:
        {
                auto &v_vec1 = data["v_vec1"].get<picojson::object>();
                auto &v_vec2 = data["v_vec2"].get<picojson::object>();
                
                return mf::nokogiri(
                        sf::Vector2f(v_vec1["x"].get<double>(), v_vec1["y"].get<double>()),
                        sf::Vector2f(v_vec2["x"].get<double>(), v_vec2["y"].get<double>()),
                        data["time_offset"].get<double>());
        }
        
	default:
                return mf::stop;
        }
}

#define TAKE_DEFAULT_ARG(obj, key, type, val) (obj.find(key) == std::end(obj) ? val : obj[(key)].get<type>())
