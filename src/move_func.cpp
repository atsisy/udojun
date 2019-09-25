#include <cmath>
#include "geometry.hpp"
#include "gm.hpp"
#include "utility.hpp"
#include "laser.hpp"
#include "value.hpp"
#include "effect.hpp"

namespace mf {
        sf::Vector2f stop(MoveObject *bullet, u64 now, u64 begin)
        {
                return bullet->get_place();
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        sin(float bias, float dx)
        {
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x + (bias * std::sin(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       now.y - dx);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	horizon_sin(float bias, float dx)
	{
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x + (bias * std::sin(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       now.y - dx);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	cos(float bias, float dx)
	{
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x + (bias * std::cos(util::count_to_second(now_lmd, begin_lmd, 60))),
                                       now.y - dx);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	linear(float bias, float dx, float c)
	{
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x + dx,
                                       now.y - (bias * dx) + c);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	vector_linear(sf::Vector2f speed)
	{
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x + speed.x,
                                       now.y - speed.y);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	shadow_vector_linear(sf::Vector2f speed, u64 enable_time, u64 disable_time)
	{
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               u64 past = now_lmd - begin_lmd;
                               if(past == enable_time){
                                       // disable time
                                       bullet->clear_effect_queue();
                                       bullet->add_effect({ effect::fade_out(20, now_lmd) });
                                       dynamic_cast<Bullet *>(bullet)->conflict_off();
                               }else if(past == (enable_time + disable_time)){
                                       bullet->clear_effect_queue();
                                       bullet->add_effect({ effect::fade_in(20, now_lmd) });
                                       dynamic_cast<Bullet *>(bullet)->conflict_on();
                               }
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x + speed.x,
                                       now.y + speed.y);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	accelerating(sf::Vector2f init_speed, sf::Vector2f accel, sf::Vector2f x_speed_range, sf::Vector2f y_speed_range)
        {
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               u64 past = now_lmd - begin_lmd;
                               sf::Vector2f speed(
                                       init_speed.x + (accel.x * past),
                                       init_speed.y + (accel.y * past));
                               
                               if(speed.x < x_speed_range.x){
                                       speed.x = x_speed_range.x;
                               }else if(speed.x > x_speed_range.y){
                                       speed.x = x_speed_range.y;
                               }

                               if(speed.y < y_speed_range.x){
                                       speed.y = y_speed_range.x;
                               }else if(speed.y > y_speed_range.y){
                                       speed.y = y_speed_range.y;
                               }

                               const sf::Vector2f &&now = bullet->get_place();
                               return now + speed;
                       };
        }

	std::function<sf::Vector2f(MoveObject *, u64, u64)>
	getting_slower(float init_speed, float angle, u64 limit)
	{
                float x_speed = init_speed * std::cos(angle);
                float y_speed = init_speed * std::sin(angle);

                float x_min_speed = std::cos(angle);
		float y_min_speed = std::sin(angle);

		return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd) {
                               const sf::Vector2f &&now = bullet->get_place();
                               if(init_speed / (now_lmd - begin_lmd) < 1){
				       return sf::Vector2f(
					       now.x + x_min_speed,
					       now.y + y_min_speed);
			       }

			       return sf::Vector2f(
				       now.x + (x_speed /
                                                (float)((now_lmd + 1) - begin_lmd)),
				       now.y + (y_speed /
                                                (float)((now_lmd + 1) - begin_lmd)));
                       };
	}

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        udon_double_step_getting_slower(float init_speed, float angle, u64 change_course_time, float sub_angle)
	{
                float x_speed = init_speed * std::cos(angle);
                float y_speed = init_speed * std::sin(angle);

                float x_min_speed = std::cos(angle);
		float y_min_speed = std::sin(angle);

		return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd) {
                               const sf::Vector2f &&now = bullet->get_place();
                               u64 past = now_lmd - begin_lmd;
                               if(past > change_course_time){
                                       float new_rotation_angle = bullet->get_angle() + sub_angle;
                                       float new_vector_angle = angle + sub_angle;
                                       std::function<sf::Vector2f(MoveObject *, u64, u64)> f =
                                               mf::vector_linear(sf::Vector2f(
                                                                         std::cos(new_vector_angle),
                                                                         -std::sin(new_vector_angle)
                                                                         ));
                                       bullet->override_move_func(f);
                                       bullet->rotate(new_rotation_angle);
                               }else if(init_speed / past < 1){
				       return sf::Vector2f(
					       now.x + x_min_speed,
					       now.y + y_min_speed);
			       }

			       return sf::Vector2f(
				       now.x + (x_speed /
                                                (float)((now_lmd + 1) - begin_lmd)),
				       now.y + (y_speed /
                                                (float)((now_lmd + 1) - begin_lmd)));
                       };
	}

	std::function<sf::Vector2f(MoveObject *, u64, u64)>
	up(float c)
	{
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd){
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(
                                       now.x,
                                       now.y - c);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	aim_self_linear(sf::Vector2f &target, float speed,
			sf::Vector2f begin_point)
	{
                float rad = geometry::calc_angle(target, begin_point);
                
		return [=](MoveObject *bullet, u64 now_lmd,
			   u64 begin_lmd) {
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(now.x + (std::cos(rad) * speed),
                                                   now.y + (std::sin(rad) * speed));
                       };
        }
        
        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        curve(sf::Vector2f begin, sf::Vector2f middle,
              sf::Vector2f end, u64 time)
        {
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd) {
                               sf::Vector2f p1_mid, p2_mid, p3_mid;
                               u64 t = now_lmd - begin_lmd;
                               float frac = (float)t / (float)time;
                               p1_mid = ((1 - frac) * begin) + (frac * middle);
                               p2_mid = ((1 - frac) * middle) + (frac * end);
                               p3_mid = ((1 - frac) * p1_mid) + (frac * p2_mid);
                               return p3_mid;
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        curve2(sf::Vector2f begin, sf::Vector2f end,
               sf::Vector2f begin_velocity, sf::Vector2f end_velocity, u64 time)
        {
                return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd) {
                               u64 t = now_lmd - begin_lmd;
                               float frac = (float)t / (float)time;
                               sf::Vector2f p = geometry::spline_curve(begin, end, begin_velocity, end_velocity, frac);
                               std::cout << p.x << ":" << p.y << std::endl;
                               return p;
                       };
        }
        
        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	uzumaki(sf::Vector2f origin, sf::Vector2f begin, float speed,
		float angle, float r_bias)
	{
		float init_r = std::sqrt(
                        std::pow(begin.x - origin.x, 2) +
                        std::pow(begin.y - origin.y, 2));
                float angle_rad = util::degree_to_radian(angle);

		return [=](MoveObject *bullet, u64 now_lmd,
			   u64 begin_lmd) {
                               const sf::Vector2f &&init = bullet->get_initial_position();
                               
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
        
        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	tachie_move_constant(float dx, float dy)
	{
		return [=](MoveObject *bullet, u64 now_lmd,
			   u64 begin_lmd) {
                               const sf::Vector2f &&now = bullet->get_place();
                               return sf::Vector2f(now.x + dx, now.y + dy);
                       };
	}
                
        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	move_point_constant(sf::Vector2f dest, sf::Vector2f now, u64 start_time, u64 end_time)
	{
                float rad = std::atan((float)(dest.x - now.x) / (float)(dest.y - now.y));
                double distance = util::distance<double>(dest.x, dest.y, now.x, now.y);
                u64 took_time = end_time - start_time;
                float speed = distance / (double)took_time;
                
                if(dest.y < now.y){
                        rad -= (M_PI);
                }

		return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd) {
                               if (now_lmd < end_time) {
                                       const sf::Vector2f &&nowp = bullet->get_place();
                                       return sf::Vector2f(
                                               nowp.x + (std::sin(rad) * speed),
                                               nowp.y + (std::cos(rad) * speed));
                               }else{
                                       return bullet->get_place();
                               }
                       };
	}
        
	std::function<sf::Vector2f(MoveObject *, u64, u64)>
	active_homing(sf::Vector2f origin, float speed, sf::Vector2f *target)
	{
		return [=](MoveObject *bullet, u64 now_lmd, u64 begin_lmd) {
			const sf::Vector2f &&now = bullet->get_place();
                        float angle = geometry::calc_angle(*target, now);
                        
			float x = speed * std::cos(angle);
			float y = speed * std::sin(angle);
			return sf::Vector2f(now.x + x, now.y + y);
		};
	}

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        same_position(MoveObject *p)
        {
                return [=](MoveObject *me, u64 now_lmd, u64 begin_lmd){
                               return p->get_origin();
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	step(sf::Vector2f begin, sf::Vector2f first_speed, u64 limit,
             u64 stop, sf::Vector2f second_speed)
	{
		return [=](MoveObject *p, u64 now_lmd, u64 begin_lmd) {
                               u64 past = now_lmd - begin_lmd;
                               sf::Vector2f now = p->get_place();
                               if(limit > past){
                                       return sf::Vector2f(
                                               now.x + first_speed.x,
                                               now.y + first_speed.y);
                               }else if((limit + stop) > past){
                                       return p->get_place();
                               }else{
                                       return sf::Vector2f(
                                               now.x + second_speed.x,
                                               now.y + second_speed.y);
                               }
                       };
	}

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
	ratio_step(sf::Vector2f goal, float ratio)
	{
		return [=](MoveObject *p, u64 now_lmd, u64 begin_lmd) {
                               sf::Vector2f now = p->get_place();
                               sf::Vector2f offset = goal - now;

                               if(std::abs(offset.x) < 1.f && std::abs(offset.y) < 1.f){
                                       p->override_move_func(mf::stop);
                                       return p->get_origin();
                               }
                               
                               offset *= ratio;
                               return now + offset;
                       };
	}

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        random_vibration(sf::Vector2f origin, u64 r)
        {
                return [=](MoveObject *p, u64 now_lmd, u64 begin_lmd) {
                               return origin + sf::Vector2f(
                                       util::generate_random() % r,
                                       util::generate_random() % r);
                       };
        }

        std::function<sf::Vector2f(MoveObject *, u64, u64)>
        random_turning(sf::Vector2f *origin, float speed, u64 r, u64 width)
        {
                float phase = geometry::convert_to_radian(util::generate_random() % 360);
                r += (util::generate_random() % 32);
                
                return [=](MoveObject *p, u64 now_lmd, u64 begin_lmd) {
                               u64 past = now_lmd - begin_lmd;
                               u64 random_r = r + (util::generate_random() % width);
                               return sf::Vector2f(
                                       origin->x + (random_r * std::sin((past * speed) + phase)),
                                       origin->y + (random_r * std::cos((past * speed) + phase))
                                       );
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

	if (original_data.find("conflictable") != std::end(original_data)){
		this->conflictable = (original_data["conflictable"].get<std::string>() == "true");
	}else{
		this->conflictable = true;
	}

	if (original_data.find("grazable") != std::end(original_data)) {
		this->grazable =
			original_data["grazable"].get<std::string>() ==
			"true";
	}else{
                this->grazable = true;
        }

	if (original_data.find("texture") == std::end(original_data)) {
                init_texture_data(BULLET_HART);
        } else {
                init_texture_data(
                        str_to_txid(original_data["texture"]
                                    .get<std::string>()
                                    .c_str()));
        }

        if (original_data.find("rotate") == std::end(original_data)) {
                init_rotation = 0;
        } else {
                init_rotation = original_data["rotate"].get<double>();
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

        if (original_data.find("rotate") == std::end(original_data)) {
                init_rotation = 0;
        } else {
                init_rotation = original_data["rotate"].get<double>();
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
        case BULLET1:
		scale = sf::Vector2f(0.12, 0.12);
                radius = BulletSize::BULLET1;
		break;
        case CIRCLE_BLUE:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
		break;
        case CIRCLE_RED:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
		break;
        case CIRCLE_GREEN:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
		break;
        case CIRCLE_ORANGE:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
		break;
        case CIRCLE_PURPLE:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
		break;
        case JUNKO_CORE_BULLET:
		scale = sf::Vector2f(0.12, 0.12);
                radius = BulletSize::JUNKO_CORE_BULLET;
		break;
        case JUNKO_HART_BULLET:
		scale = sf::Vector2f(0.12, 0.12);
                radius = BulletSize::BULLET1;
		break;
        case CIRCLE_LIGHT_GREEN:
		scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
		break;
        case ELLIPSE_BLUE:
                scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
                break;
        case ELLIPSE_ORANGE:
                scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
                break;
        case ELLIPSE_GREEN:
                scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
                break;
        case ELLIPSE_RED:
                scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
                break;
        case ELLIPSE_PURPLE:
                scale = sf::Vector2f(0.05, 0.05);
                radius = BulletSize::BULLET1;
                break;
        default:
                DEBUG_PRINT_HERE();
                std::cerr << "Invalid Texture ID: " << std::endl;
		scale = sf::Vector2f(1.0, 1.0);
		radius = BulletSize::HART_NORMAL_BULLET;
		break;
	}
}

BulletData::BulletData(
	BulletFunctionID id,
        TextureID tid,
	std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
	u64 time, sf::Vector2f appear_point, float init_rotate)
{
	this->id = id;
	this->func = f;
	this->offset = time;
	this->appear_point = appear_point;
	this->flags = 0;
        init_texture_data(tid);
        this->init_rotation = init_rotate;
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
                scale, radius,
                this->conflictable, this->grazable, init_rotation);
}

void BulletData::set_appear_time(u64 current)
{
        this->appear_time = current + this->offset;
}

FunctionCallEssential::FunctionCallEssential(std::string fn, u64 t, SHOT_MASTER_ID sm_id,
					     sf::Vector2f origin_p)
	: func_name(fn), time(t), shot_master_id(sm_id), origin(origin_p)
{}

std::vector<Bullet *> BulletGenerator::generate_laser(BulletData *data, DrawableCharacter &running_char, u64 count)
{
        /*
        Laser *l = new Laser(
                data->appear_point,
                data->original_data["length"].get<double>(),
                data->original_data["angle"].get<double>(),
                data->original_data["speed"].get<double>(),
                data->appear_time
                );
        return l->get_bullet_stream();
        */

        return {};
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
                auto &&target = running_char.get_origin();
                auto texture_scale = data->scale;
                auto texture_size = data->texture->getSize();
                
                data->func = mf::aim_self_linear(target, 7, sf::Vector2f(
                                                         p.x + (texture_size.x * texture_scale.x / 2),
                                                         p.y + (texture_size.y * texture_scale.y / 2)));
        }
        
        return {
                new Bullet(
                        data->texture,
                        p,
                        data->func, count,
                        data->scale,
                        data->radius,
                        data->conflictable, data->grazable, data->init_rotation)
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
