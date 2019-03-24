#include <cmath>
#include "geometry.hpp"
#include "gm.hpp"
#include "utility.hpp"
#include "laser.hpp"
#include "value.hpp"

namespace mf {
        sf::Vector2f stop(sf::Vector2f &init, sf::Vector2f &p, u64 now, u64 begin)
        {
                return p;
        }

        std::function<sf::Vector2f(sf::Vector2f& ,sf::Vector2f&, u64, u64)>
        sin(float bias, float dx)
        {
                return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (bias * std::sin(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       p_lmd.y - dx);
                       };
        }

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	horizon_sin(float bias, float dx)
	{
                return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (bias * std::sin(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       p_lmd.y - dx);
                       };
        }

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	cos(float bias, float dx)
	{
                return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + (bias * std::cos(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       p_lmd.y - dx);
                       };
        }

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	linear(float bias, float dx, float c)
	{
                return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x + dx,
                                       p_lmd.y - (bias * dx) + c);
                       };
        }

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	up(float c)
	{
                return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd, u64 begin_lmd){
                               return sf::Vector2f(
                                       p_lmd.x,
                                       p_lmd.y - c);
                       };
        }

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	aim_self_linear(sf::Vector2f &target, float speed,
			sf::Vector2f &begin_point)
	{
                float rad = std::atan((float)(target.x - begin_point.x) / (float)(target.y - begin_point.y));
                
                if(target.y < begin_point.y){
                        rad -= (M_PI);
                }

		return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd,
			   u64 begin_lmd) {
			return sf::Vector2f(p_lmd.x + (std::sin(rad) * speed),
					    p_lmd.y + (std::cos(rad) * speed));
		};
	}

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	uzumaki(sf::Vector2f origin, sf::Vector2f begin, float speed,
		float angle, float r_bias)
	{
		float init_r = std::sqrt(
                        std::pow(begin.x - origin.x, 2) +
                        std::pow(begin.y - origin.y, 2));
                float angle_rad = util::degree_to_radian(angle);

		return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd,
			   u64 begin_lmd) {
			float s = util::count_to_second(now_lmd, begin_lmd, 60);
			float r = init_r + (s * r_bias);
			if (r == 0)
				r = 0.00001;
			float theta = angle_rad + speed / r;

			sf::Vector2f diff = geometry::rotate_point(
				angle_rad, sf::Vector2f(r * std::cos(theta),
							r * std::sin(theta)));

			return sf::Vector2f(init.x + diff.x,
					    init.y + diff.y);
		};
	}

	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)>
	tachie_move_constant(float dx, float dy)
	{
		return [=](sf::Vector2f &init, sf::Vector2f &p_lmd, u64 now_lmd,
			   u64 begin_lmd) {
			return sf::Vector2f(p_lmd.x + dx, p_lmd.y + dy);
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

        if(original_data.find("texture") == std::end(original_data)){
		init_texture_data(BULLET_HART);
	}else{
		init_texture_data(str_to_txid(
			original_data["texture"].get<std::string>().c_str()));
	}
}

BulletData::BulletData(picojson::object &json_data, u64 flg)
        : original_data(json_data)
{       
	this->offset = original_data["time"].get<double>();
        this->flags = flg;

	if (original_data.find("extra") != std::end(original_data)) {
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

	if (original_data.find("texture") == std::end(original_data)) {
                init_texture_data(BULLET_HART);
	} else {
		init_texture_data(BULLET_HART);
		init_texture_data(str_to_txid(original_data["texture"].get<std::string>().c_str()));
	}
}


void BulletData::init_texture_data(TextureID id)
{
	this->texture = GameMaster::texture_table[id];
	switch(id){
        case BULLET_HART:
                scale = sf::Vector2f(1.0, 1.0);
                radius = BulletSize::HART_NORMAL_BULLET;
                break;
        case BULLET_BIG_RED:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BIG_CIRCLE_RED;
		break;
        default:
                std::cerr << "Invalid Texture ID" << std::endl;
		scale = sf::Vector2f(1.0, 1.0);
		radius = BulletSize::HART_NORMAL_BULLET;
		break;
	}
}

BulletData::BulletData(
	BulletFunctionID id,
	std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)> f,
	u64 time, sf::Vector2f appear_point)
{
	this->id = id;
	this->func = f;
	this->offset = time;
	this->appear_point = appear_point;
	this->flags = 0;
        init_texture_data(BULLET_HART);
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
                this->texture,
                p,
                func, count,
                scale, radius);
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
                        data->texture,
                        p,
                        data->func, count,
                        data->scale,
                        data->radius)
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
