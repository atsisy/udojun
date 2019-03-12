#include <cmath>
#include "gm.hpp"
#include "utility.hpp"
#include "laser.hpp"

namespace mf {
        sf::Vector2f stop(sf::Vector2f &p, u64 now, u64 begin)
        {
                return p;
        }

        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
        sin(float bias, float dx)
        {
                return [=](sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (bias * std::sin(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       p_lmd.y - dx);
                       };
        }

        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
        horizon_sin(float bias, float dx)
        {
                return [=](sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (bias * std::sin(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       p_lmd.y - dx);
                       };
        }
        
        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
        cos(float bias, float dx)
        {
                return [=](sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (bias * std::cos(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       p_lmd.y - dx);
                       };
        }

        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
        linear(float bias, float dx, float c)
        {
                return [=](sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + dx,
                                       p_lmd.y - (bias * dx) + c);
                       };
        }

        
        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
        up(float c)
        {
                return [=](sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x,
                                       p_lmd.y - c);
                       };
        }

        std::function<sf::Vector2f(sf::Vector2f&, u64, u64)>
        aim_self_linear(sf::Vector2f &target, float speed, sf::Vector2f &begin_point)
        {
                float rad = std::atan((float)(target.x - begin_point.x) / (float)(target.y - begin_point.y));
                
                if(target.y < begin_point.y){
                        rad -= (M_PI);
                }
                
                return [=](sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (std::sin(rad) * speed),
                                       p_lmd.y + (std::cos(rad) * speed));
                       };
        }

        
}

BulletData::BulletData(picojson::object &json_data)
        : original_data(json_data)
{
        this->id = str_to_bfid(original_data["ID"].get<std::string>().c_str());
        this->func = select_bullet_function(this->id, original_data);
        this->offset = original_data["time"].get<double>();
        this->flags = 0;

        if(original_data.find("x") != std::end(original_data) && original_data.find("y") != std::end(original_data)){
                this->appear_point = sf::Vector2f(original_data["x"].get<double>(), original_data["y"].get<double>());
        }else if(original_data.find("x") != std::end(original_data)){
                this->appear_point = sf::Vector2f(original_data["x"].get<double>(), -1);
        }else if(original_data.find("y") != std::end(original_data)){
                this->appear_point = sf::Vector2f(-1, original_data["y"].get<double>());
        }

        if(original_data.find("extra") != std::end(original_data)){
                std::string &extra = original_data["extra"].get<std::string>();
                if(extra.find("begin-at-self") != std::string::npos){
                        flags |= (BEGIN_AT_RUNNING_CHARACTER_X | BEGIN_AT_RUNNING_CHARACTER_Y);
                }
                if(extra.find("begin-at-self-x") != std::string::npos){
                        flags |= (BEGIN_AT_RUNNING_CHARACTER_X);
                }
                if(extra.find("begin-at-self-x") != std::string::npos){
                        flags |= (BEGIN_AT_RUNNING_CHARACTER_Y);
                }
        }
}

BulletData::BulletData(picojson::object &json_data, u64 flg)
        : original_data(json_data)
{
        this->offset = original_data["time"].get<double>();
        this->flags = flg;

        if(original_data.find("extra") != std::end(original_data)){
                std::string &extra = original_data["extra"].get<std::string>();
                if(extra.find("begin-at-self") != std::string::npos){
                        flags |= (BEGIN_AT_RUNNING_CHARACTER_X | BEGIN_AT_RUNNING_CHARACTER_Y);
                }
                if(extra.find("begin-at-self-x") != std::string::npos){
                        flags |= (BEGIN_AT_RUNNING_CHARACTER_X);
                        this->appear_point = sf::Vector2f(-1, original_data["y"].get<double>());
                }
                if(extra.find("begin-at-self-y") != std::string::npos){
                        flags |= (BEGIN_AT_RUNNING_CHARACTER_Y);
                        this->appear_point = sf::Vector2f(-1, original_data["x"].get<double>());
                }
        }else{
                this->appear_point = sf::Vector2f(original_data["x"].get<double>(), original_data["y"].get<double>());
        }
}

Bullet *BulletData::generate(DrawableCharacter &running_char, u64 count)
{
        sf::Vector2f p = appear_point;
        if(flags & BEGIN_AT_RUNNING_CHARACTER_X){
                p.x = running_char.get_origin().x;
        }
        if(flags & BEGIN_AT_RUNNING_CHARACTER_Y){
                p.y = running_char.get_origin().y;
        }
        if(flags & AIMING_SELF){
                auto target = running_char.get_origin();
                this->func = mf::aim_self_linear(target, 8, p);
        }
        
        return new Bullet(
                GameMaster::texture_table[BULLET_HART],
                p,
                func, count);
}

void BulletData::set_appear_time(u64 current)
{
        this->appear_time = current + this->offset;
}

std::vector<Bullet *> BulletGenerator::generate_laser(BulletData *data, DrawableCharacter &running_char, u64 count)
{
        Laser *l = new Laser(
                data->appear_point,
                data->original_data["length"].get<double>(),
                data->original_data["angle"].get<double>(),
                data->original_data["speed"].get<double>(),
                data->appear_time
                );
        return l->get_bullet_stream();
}

std::vector<Bullet *> BulletGenerator::generate_bullet(BulletData *data, DrawableCharacter &running_char, u64 count)
{
        
        sf::Vector2f p = data->appear_point;
        if(data->flags & BEGIN_AT_RUNNING_CHARACTER_X){
                p.x = running_char.get_origin().x;
        }
        if(data->flags & BEGIN_AT_RUNNING_CHARACTER_Y){
                p.y = running_char.get_origin().y;
        }
        if(data->flags & AIMING_SELF){
                auto target = running_char.get_origin();
                data->func = mf::aim_self_linear(target, 8, p);
        }
        
        return {
                new Bullet(
                        GameMaster::texture_table[BULLET_HART],
                        p,
                        data->func, count)
        };
}

std::vector<Bullet *> BulletGenerator::generate(BulletData *data, DrawableCharacter &running_char, u64 count)
{
        if(data->flags & LASER_BULLET){
                return generate_laser(data, running_char, count);
        }else{
                return generate_bullet(data, running_char, count);                
        }
}

