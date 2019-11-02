#include <cmath>
#include "gm.hpp"
#include "macro.hpp"
#include "geometry.hpp"

namespace macro {
        
        std::vector<BulletData *> circle(sf::Vector2f origin, float r, u8 num, u64 time, float phase)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        float c = (rad > (M_PI / 2) && rad < (3 * M_PI / 2) ? 2 : -2);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              BULLET_HART,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       -(r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       2 * std::cos(rad), 0)),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y + (r * std::sin(rad)))
                                              ));
                }
                
                return ret;
        }

        std::vector<BulletData *> delay_circle(TextureID txid, sf::Vector2f origin, float r,
                                               float speed, u8 num, u64 delay, u64 time, float phase)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad, time += delay){
                        ret.push_back(new BulletData(
                                              BEZIER_CURVE,
                                              txid,
                                              mf::vector_linear(
                                                      sf::Vector2f(
                                                              speed * std::cos(rad),
                                                              -speed * std::sin(rad))),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y + (r * std::sin(rad)))
                                              ));
                }
                
                return ret;
        }
        
        std::vector<BulletData *> ellipse(sf::Vector2f origin, float r, float a, float b,
                                          u8 num, u64 time, float phase)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        float c = (rad > (M_PI / 2) && rad < (3 * M_PI / 2) ? 2 : -2);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              BULLET_HART,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       -(r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       2 * std::cos(rad), 0)),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (a * r * std::cos(rad)),
                                                      origin.y + (b * r * std::sin(rad)))
                                              ));
                }

                return ret;
        }

        std::vector<BulletData *> udon_ellipse(sf::Vector2f origin, sf::Vector2f target,
                                               sf::Vector2f init_speed, u64 change_time,
                                               float r, float a, float b,
                                               u8 num, u64 time)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = 0;

                float target_angle = std::atan((float)(target.x - origin.x) / (float)(target.y - origin.y));

                if(target.y < origin.y){
                        target_angle -= (M_PI);
                }

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        sf::Vector2f point = sf::Vector2f(
                                origin.x + (a * r * std::cos(rad)),
                                origin.y + (b * r * std::sin(rad)));
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              CIRCLE_ORANGE,
                                              mf::vector_linear(sf::Vector2f(
                                                                        init_speed.x * std::sin(target_angle),
                                                                        -(init_speed.y * std::cos(target_angle)))),
                                              time,
                                              geometry::rotate_point2(target_angle, point, origin)));
                }


                sf::Vector2f small_origin = origin + sf::Vector2f(0, 200);

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        sf::Vector2f point = sf::Vector2f(
                                small_origin.x + (r * 0.3 * std::cos(rad)),
                                small_origin.y + (r * 0.3 * std::sin(rad)));
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              CIRCLE_GREEN,
                                              mf::vector_linear(sf::Vector2f(
                                                                        init_speed.x * std::sin(target_angle),
                                                                        -(init_speed.y * std::cos(target_angle)))),
                                              time,
                                              geometry::rotate_point2(target_angle, point, origin)));
                }

                return ret;
        }

        std::vector<BulletData *> normal_enemy_shot1(TextureID txid, sf::Vector2f origin,
                                                     float r, float speed, float phase, u64 num, u64 time)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        BulletData *p = new BulletData(
                                VECTOR_LINEAR,
                                txid,
                                mf::vector_linear(sf::Vector2f(speed * std::cos(rad),
                                                               -speed * std::sin(rad))),
                                time,
                                sf::Vector2f(origin.x + (r * std::cos(rad)), origin.y + (r * std::sin(rad)))
                                );
                        p->init_rotation = rad + (M_PI / 2.0);
                        ret.push_back(p);
                }

                return ret;
        }
        
        std::vector<BulletData *> udon_circle(sf::Vector2f origin, float speed,
                                              float r, u8 num, u64 time, float phase,
                                              u64 change_course_time, float sub_angle)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        ret.push_back(new BulletData(
                                              UDON_SLOWER,
                                              BULLET1,
                                              mf::udon_double_step_getting_slower(speed, rad, change_course_time, sub_angle),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y + (r * std::sin(rad))),
                                              rad + M_PI_2));
                }

                return ret;
        }

        std::vector<BulletData *> udon_circle2(sf::Vector2f origin, float speed,
					      float r, u16 num, u64 time,
                                               float phase, float unit_rad)
	{
		std::vector<BulletData *> ret;
		float rad1 = phase;
                float rad2 = phase + M_PI;

		for (u16 i = 0; i < num; i++, rad1 += unit_rad, rad2 += unit_rad, time += 3) {
			ret.push_back(new BulletData(
                                              str_to_bfid("SLOWER1"),
                                              BULLET1,
                                              mf::getting_slower(speed, rad1, 0), time,
                                              sf::Vector2f(origin.x + (r * std::cos(rad1)),
                                                           origin.y + (r * std::sin(rad1))),
                                              rad1 + M_PI_2));
			ret.push_back(new BulletData(
                                              str_to_bfid("SLOWER1"),
                                              BULLET_BLUE,
                                              mf::getting_slower(speed, rad2, 0), time,
                                              sf::Vector2f(origin.x + (r * std::cos(rad2)),
                                                           origin.y + (r * std::sin(rad2))),
                                              rad2 + M_PI_2));
		}

		return ret;
	}

	std::vector<BulletData *> udon_tsujo2(sf::Vector2f origin, u64 time, float phase)
	{
		return udon_circle2(origin, 90, 10, 1024, time, phase, 0.06);
	}

        std::vector<BulletData *> udon_tsujo2_hard(sf::Vector2f origin, u64 time, float phase)
	{
		return udon_circle2(origin, 90, 10, 1500, time, phase, 0.04);
	}

        std::vector<BulletData *> udon_tsujo3_sub(TextureID txid, sf::Vector2f origin, u64 count, u64 time, float phase, float r, float speed)
	{
                std::vector<BulletData *> ret;
                float rad = phase;
                float unit = (2.f * M_PI) / count;

                for(u64 i = 0;i < count;i++, rad += unit){
                        ret.push_back(new BulletData(str_to_bfid("SLOWER1"),
                                                     txid,
                                                     mf::linear_getting_slower(speed, rad, 0.08, 1.2),
                                                     time,
                                                     sf::Vector2f(origin.x + (r * std::cos(rad)),
                                                                  origin.y + (r * std::sin(rad))),
                                                     rad + M_PI_2));
                }
                
                return ret;
	}
        
        std::vector<BulletData *> udon_tsujo3_sub2(TextureID txid, sf::Vector2f origin, u64 count, u64 time, float phase, float r, float speed)
	{
                std::vector<BulletData *> ret;
                float rad = phase;
                float unit = (2.f * M_PI) / count;

                for(u64 i = 0;i < count;i++, rad -= unit){
                        ret.push_back(new BulletData(str_to_bfid("SLOWER1"),
                                                     txid,
                                                     mf::linear_getting_slower(speed, rad, 0.08, 1.2),
                                                     time,
                                                     sf::Vector2f(origin.x + (r * std::cos(rad)),
                                                                  origin.y + (r * std::sin(rad))),
                                                     rad + M_PI_2));
                }
                
                return ret;
	}

        std::vector<BulletData *> udon_tsujo3(sf::Vector2f origin, u64 time, u64 offset, float r, float speed)
	{
                std::vector<BulletData *> ret;
                
                for(int i = 0;i < 1024;i++, time += offset){
                        auto && elem = udon_tsujo3_sub(BULLET1, origin, 4, time, (2 * M_PI) * std::sin((float)i / 32.f), r, speed);
                        util::concat_container<std::vector<BulletData *>>(ret, elem);
                }

                return ret;
	}

        std::vector<BulletData *> udon_tsujo3_hard(sf::Vector2f origin, u64 time, u64 offset, float r, float speed)
	{
                std::vector<BulletData *> ret;
                
                for(int i = 0;i < 1024;i++, time += offset){
                        auto && elem1 = udon_tsujo3_sub(BULLET1, origin, 3, time, ((2 * M_PI) * std::sin((float)i / 32.f)) - 0.75, r, speed);
                        auto && elem2 = udon_tsujo3_sub(BULLET_BLUE, origin, 3, time, ((2 * M_PI) * std::sin(-(float)i / 32.f)) - 0.75, r, speed);
                        util::concat_container<std::vector<BulletData *>>(ret, elem1);
                        util::concat_container<std::vector<BulletData *>>(ret, elem2);
                }

                return ret;
	}

        std::vector<BulletData *> nerai_circle1(TextureID txid, sf::Vector2f origin, float angle, float offset_angle, u64 num, u64 time, float r, float speed)
	{
                std::vector<BulletData *> ret;
                float rad = angle + offset_angle;
                float unit = (2.f * M_PI) / num;
                
                for(u64 i = 0;i < num;i++, rad += unit){
                        ret.push_back(new BulletData(str_to_bfid("SLOWER1"),
                                                     txid,
                                                     mf::vector_linear(sf::Vector2f(speed * std::cos(rad), -speed * std::sin(rad))),
                                                     time,
                                                     sf::Vector2f(origin.x + (r * std::cos(rad)),
                                                                  origin.y + (r * std::sin(rad))),
                                                     rad + M_PI_2));
                }
                
                return ret;
	}

        std::vector<BulletData *> udon_nerai_circle1(TextureID txid, sf::Vector2f origin, float angle,  float offset_angle, u64 num, u64 time, float r, float speed, u64 stop)
	{
                std::vector<BulletData *> ret;
                float rad = angle + offset_angle;
                float unit = (2.f * M_PI) / num;

                for(u64 i = 0;i < num;i++, rad += unit){
                        ret.push_back(new BulletData(str_to_bfid("SLOWER1"),
                                                     txid,
                                                     mf::stop_and_override(stop,
                                                                           mf::vector_linear(
                                                                                   sf::Vector2f(speed * std::cos(rad),
                                                                                                -speed * std::sin(rad)))),
                                                     time,
                                                     sf::Vector2f(origin.x + (r * std::cos(rad)),
                                                                  origin.y + (r * std::sin(rad))),
                                                     rad + M_PI_2));
                }
                
                return ret;
	}

        std::vector<BulletData *> udon_spellcard1(sf::Vector2f origin, sf::Vector2f target,
                                                  sf::Vector2f speed, u64 change_time, u64 time,
                                                  float r)
	{
                std::vector<BulletData *> ret;
                std::vector<BulletData *> root_part =
                        udon_ellipse(origin, target, speed, change_time, r, 0.5, 1.0, 24, time);

                return root_part;
	}

        std::vector<BulletData *> delay_circle_move_linear(TextureID txid, sf::Vector2f origin, i16 rotate_times, float r,
                                                           float speed, float angle, u8 num, u64 delay, u64 time, float phase)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                sf::Vector2f speed_vec(speed * std::cos(angle), -speed * std::sin(angle));

                while(rotate_times--){
                        for(u8 i = 0;i < num;i++, rad += unit_rad, time += delay){
                                ret.push_back(new BulletData(
                                                      BEZIER_CURVE,
                                                      txid,
                                                      mf::vector_linear(speed_vec),
                                                      time,
                                                      sf::Vector2f(
                                                              origin.x + (r * std::cos(rad)),
                                                              origin.y + (r * std::sin(rad)))
                                                      ));
                        }
                }

                
                return ret;
        }
        

        std::vector<BulletData *> udon_spellcard2_sub(TextureID txid, sf::Vector2f origin, float speed,
                                                      u64 enable_time, u64 disable_time, u64 time, float r, u64 num)
	{
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = 0;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        ret.push_back(new BulletData(
                                              str_to_bfid("SHADOW_VECTOR_LINEAR"),
                                              txid,
                                              mf::shadow_vector_linear(
                                                      sf::Vector2f(speed * std::cos(rad),
                                                                   speed * std::sin(rad)),
                                                      enable_time, disable_time),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y + (r * std::sin(rad)))
                                              ));
                }
                
                return ret;
	}

        std::vector<BulletData *> udon_spellcard2(float speed,
                                                  u64 enable_time, u64 disable_time, u64 time, float r, u64 num)
        {
                std::vector<BulletData *> ret;
                std::vector<BulletData *> &&circle11 = udon_spellcard2_sub(CIRCLE_BLUE, sf::Vector2f(50, 50), speed,
                                                                         enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle11_1 = udon_spellcard2_sub(CIRCLE_BLUE, sf::Vector2f(150, 50), speed,
                                                                           enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle11_2 = udon_spellcard2_sub(CIRCLE_BLUE, sf::Vector2f(50, 150), speed,
                                                                             enable_time, disable_time, time, r, num);
                
                std::vector<BulletData *> &&circle12 = udon_spellcard2_sub(CIRCLE_GREEN, sf::Vector2f(900, 50), speed,
                                                                         enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle12_1 = udon_spellcard2_sub(CIRCLE_GREEN, sf::Vector2f(800, 50), speed,
                                                                             enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle12_2 = udon_spellcard2_sub(CIRCLE_GREEN, sf::Vector2f(900, 150), speed,
                                                                             enable_time, disable_time, time, r, num);
                
                std::vector<BulletData *> &&circle21 = udon_spellcard2_sub(CIRCLE_RED, sf::Vector2f(50, 700), speed,
                                                                          enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle21_1 = udon_spellcard2_sub(CIRCLE_RED, sf::Vector2f(150, 700), speed,
                                                                             enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle21_2 = udon_spellcard2_sub(CIRCLE_RED, sf::Vector2f(50, 600), speed,
                                                                             enable_time, disable_time, time, r, num);
                
                std::vector<BulletData *> &&circle22 = udon_spellcard2_sub(CIRCLE_LIGHT_GREEN, sf::Vector2f(900, 700), speed,
                                                                          enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle22_1 = udon_spellcard2_sub(CIRCLE_LIGHT_GREEN, sf::Vector2f(800, 700), speed,
                                                                           enable_time, disable_time, time, r, num);
                std::vector<BulletData *> &&circle22_2 = udon_spellcard2_sub(CIRCLE_LIGHT_GREEN, sf::Vector2f(900, 600), speed,
                                                                           enable_time, disable_time, time, r, num);

                util::concat_container<std::vector<BulletData *>>(ret, circle11);
                util::concat_container<std::vector<BulletData *>>(ret, circle11_1);
                util::concat_container<std::vector<BulletData *>>(ret, circle11_2);
                
                util::concat_container<std::vector<BulletData *>>(ret, circle12);
                util::concat_container<std::vector<BulletData *>>(ret, circle12_1);
                util::concat_container<std::vector<BulletData *>>(ret, circle12_2);
                
                util::concat_container<std::vector<BulletData *>>(ret, circle21);
                util::concat_container<std::vector<BulletData *>>(ret, circle21_1);
                util::concat_container<std::vector<BulletData *>>(ret, circle21_2);
                
                util::concat_container<std::vector<BulletData *>>(ret, circle22);
                util::concat_container<std::vector<BulletData *>>(ret, circle22_1);
                util::concat_container<std::vector<BulletData *>>(ret, circle22_2);
                
                return ret;
        }

        std::vector<BulletData *> udon_spellcard4_sub(TextureID txid, sf::Vector2f origin, float speed,
                                                      u64 trigger, u64 disable_time, u64 time, float r, u64 num)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = ((float)(util::generate_random() % 360) / 180.0) * M_PI;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        ret.push_back(new BulletData(
                                              str_to_bfid("SHADOW_VECTOR_LINEAR"),
                                              txid,
                                              mf::shadow_vector_linear2(
                                                      sf::Vector2f(speed * std::cos(rad),
                                                                   speed * std::sin(rad)),
                                                      trigger, disable_time),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y + (r * std::sin(rad)))
                                              ));
                }
                
                return ret;
        }

        std::vector<BulletData *> udon_spellcard4(TextureID txid, sf::Vector2f origin, float speed,
                                                  u64 trigger, u64 disable_time, u64 cast_times,
                                                  u64 time, u64 time_offset, float r, u64 num)
        {
                std::vector<BulletData *> ret;

                for(u64 i = 0;i < cast_times;i++, time += time_offset){
                        std::vector<BulletData *> &&circle = udon_spellcard4_sub(txid, sf::Vector2f(50, 50), speed,
                                                                                   trigger, disable_time, time, r, num);
                        util::concat_container<std::vector<BulletData *>>(ret, circle);
                }
                
                return ret;
        }
        
	std::vector<BulletData *> hart(sf::Vector2f origin, float r, u8 num, u64 time)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = 0;
                
                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        float c = (rad > (M_PI / 2) && rad < (3* M_PI / 2) ? -2 : 2);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              BULLET_HART,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       -(r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       2 * std::cos(rad), 0)),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * (16 * std::pow(std::sin(rad), 3))),
                                                      origin.y - (r * ((13 * std::cos(rad)) - (5 * std::cos(2 * rad)) - (2 * std::cos(3 * rad)) - std::cos(4 * rad)))
                                                      )));
                                      }
                return ret;
        }

	std::vector<BulletData *> udon_tsujo1(sf::Vector2f origin, u64 time)
        {
                std::vector<BulletData *> ret;
                float rad;
                for(int i = 0;i < 120;i++){
			rad = ((float)(util::generate_random() % 360) /
				 180.0) *
				M_PI;
			auto &&circle_data = udon_circle(origin, 120, 20, 36, time + (i * 15), rad, 180, 0.1);
			std::copy(std::begin(circle_data), std::end(circle_data), std::back_inserter(ret));
		}
                
                return ret;
        }
        
        std::vector<BulletData *> odd_n_way(TextureID txid, sf::Vector2f origin,
                                            float r, float toward,
                                            float unit_rad, u8 num,
                                            u64 time, float speed)
	{
                std::vector<BulletData *> ret;
                float rad = toward;

                rad -= (num / 2) * unit_rad;

                do{
                        float c = (rad > (M_PI / 2) && rad < (3* M_PI / 2) ? -speed : speed);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              txid,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       (r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       speed * std::cos(rad), 0)),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y - (r * std::sin(rad)))
                                              ));
                        rad += unit_rad;
                }while(--num);

                return ret;
        }

        
        std::vector<BulletData *> even_n_way(TextureID txid, sf::Vector2f origin, float r,
                                            float toward, float unit_rad,
                                             u8 num, u64 time, float speed)
        {
                std::vector<BulletData *> ret;
                float rad = toward;

                rad -= (((num / 2) - 1) * unit_rad) + (unit_rad / 2);

                do{
                        float c = (rad > (M_PI / 2) && rad < (3* M_PI / 2) ? -speed : speed);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              txid,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::vector_linear(sf::Vector2f(
                                                                                     speed * std::cos(rad),
                                                       speed * std::cos(rad)))),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y - (r * std::sin(rad)))
                                              ));
                        rad += unit_rad;
                }while(--num);

                return ret;
        }

        std::vector<BulletData *> n_way(TextureID txid, sf::Vector2f origin, float r,
                                        float toward, float unit_rad,
                                        u8 num, u64 time, float speed)
        {
                if(num % 2){
                        return odd_n_way(txid, origin, r, toward, unit_rad, num, time, speed);
                }else{
                        return even_n_way(txid, origin, r, toward, unit_rad, num, time, speed);
                }
        }

        std::vector<BulletData *> winder(TextureID txid, sf::Vector2f origin, float r,
                                         float toward, float unit_rad,
                                         u8 num, u64 time, float speed, u64 distance, u64 term)
        {
                std::vector<BulletData *> ret;
                u64 winder_num = term / distance;
                
                do{
                        auto &&wave = n_way(txid, origin, r, toward, unit_rad, num, time, speed);
                        util::concat_container<std::vector<BulletData *>>(ret, wave);
                        time += distance;
                }while(--winder_num);

                return ret;
        }

        std::vector<BulletData *> random_circles(u16 circle_num, u16 num,
                                                 float speed, u64 time, u16 distance)
        {
                std::vector<BulletData *> ret;
                
                for(u16 i = 0;i < circle_num;i++, time += distance){
                        sf::Vector2f p((util::generate_random() % 980) + 32, util::generate_random() % 320);
                        auto &&vec = circle(p, 12, num, time, (util::generate_random() % 100) / 3);
                        util::concat_container<std::vector<BulletData *>>(ret, vec);
                }

                return ret;
        }

        std::vector<BulletData *> multi_shot(TextureID txid, sf::Vector2f begin, u16 num,
                                             float speed, float angle,
                                             float bias,
                                             u64 time, u64 distance)
        {
                std::vector<BulletData *> ret;
                
                for(u16 i = 0;i < num;i++, time += distance, speed += bias){
                        ret.push_back(new BulletData(
                                str_to_bfid("LINEAR"),
                                txid,
                                mf::vector_linear(
                                        sf::Vector2f(speed * std::cos(angle),
                                                     -speed * std::sin(angle))),
                                time,
                                begin));
                }

                return ret;
        }

        std::vector<BulletData *> straight_circle(TextureID txid, sf::Vector2f origin, float r,
                                                  float speed, u8 num, u8 multi_shot_num, u64 delay, u64 time, float phase)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        auto vec = multi_shot(txid, origin + sf::Vector2f(r * std::cos(rad), r * std::sin(rad)),
                                              num, speed, rad + (M_PI / 2.f), 0, time, delay);
                        util::concat_container<std::vector<BulletData *>>(ret, vec);
                }
                
                return ret;
        }
        
        std::vector<BulletData *> junko_shot_fast_lv1(TextureID txid, sf::Vector2f center, float speed, u64 time)
        {
                std::vector<BulletData *> ret;
                BulletData *tmp;

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(0,
                                             speed)),
                        time,
                        center + sf::Vector2f(0, -15)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(speed * std::cos(geometry::convert_to_radian(110)),
                                             speed * std::sin(geometry::convert_to_radian(105)))),
                        time,
                        center + sf::Vector2f(-10, -12)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(speed * std::cos(geometry::convert_to_radian(70)),
                                             speed * std::sin(geometry::convert_to_radian(70)))),
                        time,
                        center + sf::Vector2f(10, -12)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                return ret;
        }

        std::vector<BulletData *> junko_shot_fast_lv2(TextureID txid1, TextureID txid2,
                                                      sf::Vector2f center, float speed, u64 time)
        {
                std::vector<BulletData *> ret;
                BulletData *tmp;

                auto &&lv1 = junko_shot_fast_lv1(txid1, center, speed, time);
                util::concat_container<std::vector<BulletData *>>(ret, lv1);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid2,
                        mf::vector_linear(
                                sf::Vector2f(0.6 * speed * std::cos(geometry::convert_to_radian(120)),
                                             0.6 * speed * std::sin(geometry::convert_to_radian(120)))),
                        time,
                        center + sf::Vector2f(-30, 16)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid2,
                        mf::vector_linear(
                                sf::Vector2f(0.6 * speed * std::cos(geometry::convert_to_radian(60)),
                                             0.6 * speed * std::sin(geometry::convert_to_radian(60)))),
                        time,
                        center + sf::Vector2f(30, 16)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                return ret;
        }

        std::vector<BulletData *> junko_shot_slow_lv1(TextureID txid, sf::Vector2f center, float speed, u64 time)
        {
                std::vector<BulletData *> ret;
                BulletData *tmp;

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(0,
                                             speed)),
                        time,
                        center + sf::Vector2f(-7, -15)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(0,
                                             speed)),
                        time,
                        center + sf::Vector2f(7, -15)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                return ret;
        }

        std::vector<BulletData *> junko_shot_slow_lv2(TextureID txid, sf::Vector2f center, float speed, u64 time)
        {
                std::vector<BulletData *> ret;
                BulletData *tmp;

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(0,
                                             speed)),
                        time,
                        center + sf::Vector2f(0, -15)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(0,
                                             speed)),
                        time,
                        center + sf::Vector2f(15, -12)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                tmp = new BulletData(
                        str_to_bfid("LINEAR"),
                        txid,
                        mf::vector_linear(
                                sf::Vector2f(0,
                                             speed)),
                        time,
                        center + sf::Vector2f(-15, -12)
                        );
                tmp->alpha = 180;
                ret.push_back(tmp);

                return ret;
        }

        std::vector<BulletData *> junko_spellcard_sub_hart(TextureID txid, sf::Vector2f origin, sf::Vector2f curve_middle,
                                                           sf::Vector2f curve_end, float speed, float r, u64 num, u64 time)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = 0;
                
                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        sf::Vector2f point(
                                origin.x + (r * (16 * std::pow(std::sin(rad), 3))),
                                origin.y - (r * ((13 * std::cos(rad)) - (5 * std::cos(2 * rad)) - (2 * std::cos(3 * rad)) - std::cos(4 * rad)))
                                );
                        sf::Vector2f offset = point - origin;
                        ret.push_back(new BulletData(
                                              BEZIER_CURVE,
                                              JUNKO_HART_BULLET,
                                              //mf::vector_linear(sf::Vector2f(0, -4)),
                                              mf::curve(point, curve_middle + offset,
                                                        curve_end + offset, 60),
                                              time,
                                              point));

                        std::cout << offset.x << ":" << offset.y << std::endl;
                }
                return ret;
        }
        
        
        std::vector<BulletData *> junko_spellcard(TextureID txid, sf::Vector2f junko_origin,
                                                  sf::Vector2f curve_middle, sf::Vector2f curve_end,
                                                  u64 time_offset, float speed, float r, float num, u64 time)
        {
                std::vector<BulletData *> ret;

                /*
                sf::Vector2f x_offset(0, 0);
                
                for(int cols = 0;cols < 16;cols++, time += time_offset){
                        auto &&right_side = junko_spellcard_sub_hart(txid, junko_origin, curve_middle + x_offset,
                                                                   curve_end + x_offset, speed, r, num, time);
                        auto &&left_side = junko_spellcard_sub_hart(txid, junko_origin, curve_middle - x_offset,
                                                                     curve_end - x_offset, speed, r, num, time);
                        util::concat_container<std::vector<BulletData *>>(ret, right_side);
                        util::concat_container<std::vector<BulletData *>>(ret, left_side);
                }
                */
                for(int cols = 0;cols < 16;cols++){
                        for(int rows = 0;rows < 90;rows++){
                                ret.push_back(
                                        new BulletData(
                                                VECTOR_LINEAR,
                                                JUNKO_HART_BULLET,
                                                mf::vector_linear(sf::Vector2f(0, 10)),
                                                time,
                                                sf::Vector2f(rows * 10, 700)
                                                ));
                                time += time_offset;
                        }
                }
                
                return ret;
        }
        
        std::vector<BulletData *> expand_macro(picojson::object &data)
        {
                switch(str_to_macroid(data["ID"].get<std::string>().c_str())){
                case CIRCLE:
                        return circle(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                case DELAY_CIRCLE:
                        return delay_circle(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["speed"].get<double>(),
                                data["quantity"].get<double>(),
                                data["delay"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                case ELLIPSE:
                        return macro::ellipse(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["a"].get<double>(),
                                data["b"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                        break;
                case HART:
                        return hart(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
                case UDON_CIRCLE1:
                        return udon_circle(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["speed"].get<double>(),
                                data["r"].get<double>(),
                                data["num"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0),
                                data["change_course_time"].get<double>(),
                                data["sub_angle"].get<double>()
                                );
                case UDON_SPELL2:
                        return macro::udon_spellcard2(data["speed"].get<double>(),
                                                      data["trigger"].get<double>(),
                                                      data["disable_time"].get<double>(),
                                                      data["time"].get<double>(),
                                                      data["r"].get<double>(),
                                                      data["num"].get<double>());
                case UDON_SPELL4:
                        return macro::udon_spellcard4(str_to_txid(data["texture"].get<std::string>().data()),
                                                      sf::Vector2f(data["x"].get<double>(),
                                                                   data["y"].get<double>()),
                                                      data["speed"].get<double>(),
                                                      data["trigger"].get<double>(),
                                                      data["disable_time"].get<double>(),
                                                      data["cast_times"].get<double>(),
                                                      data["time"].get<double>(),
                                                      data["time_offset"].get<double>(),
                                                      data["r"].get<double>(),
                                                      data["num"].get<double>());
                case UDON_TSUJO1:
			return udon_tsujo1(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>());
		case UDON_TSUJO2:
			return udon_tsujo2(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
				TAKE_DEFAULT_ARG(data, "phase", double, 0));
                case UDON_TSUJO2_HARD:
			return udon_tsujo2_hard(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
				TAKE_DEFAULT_ARG(data, "phase", double, 0));
                case UDON_TSUJO3:
                        return udon_tsujo3(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
                                data["offset"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>());
                case UDON_TSUJO3_HARD:
                        return udon_tsujo3_hard(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
                                data["offset"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>());
                case RANDOM_CIRCLES:
                        return random_circles(
                                (int)data["circle_num"].get<double>(),
				(int)data["num"].get<double>(),
                                data["speed"].get<double>(),
                                (int)data["time"].get<double>(),
                                (int)data["distance"].get<double>());
		case N_WAY:
                        return n_way(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                sf::Vector2f(data["x"].get<double>(),
                                             data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["angle"].get<double>(),
                                data["width"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                data["speed"].get<double>());
                case MULTI_SHOT:
                        return multi_shot(str_to_txid(data["texture"].get<std::string>().data()),
                                          sf::Vector2f(data["x"].get<double>(),
						       data["y"].get<double>()),
                                          (int)data["num"].get<double>(),
                                          data["speed"].get<double>(),
                                          data["angle"].get<double>(),
                                          data["bias"].get<double>(),
                                          data["time"].get<double>(),
                                          data["distance"].get<double>());
                case JUNKO_SHOT_FAST_LV1:
                        return junko_shot_fast_lv1(str_to_txid(data["texture"].get<std::string>().data()),
                                                   sf::Vector2f(data["x"].get<double>(),
                                                                data["y"].get<double>()),
                                                   data["speed"].get<double>(), data["time"].get<double>());
                case JUNKO_SHOT_FAST_LV2:
                        return junko_shot_fast_lv2(str_to_txid(data["texture1"].get<std::string>().data()),
                                                   str_to_txid(data["texture2"].get<std::string>().data()),
                                                   sf::Vector2f(data["x"].get<double>(),
                                                                data["y"].get<double>()),
                                                   data["speed"].get<double>(), data["time"].get<double>());
                case JUNKO_SHOT_SLOW_LV1:
                        return junko_shot_slow_lv1(str_to_txid(data["texture"].get<std::string>().data()),
                                                   sf::Vector2f(data["x"].get<double>(),
                                                                data["y"].get<double>()),
                                                   data["speed"].get<double>(), data["time"].get<double>());
                case JUNKO_SHOT_SLOW_LV2:
                        return junko_shot_slow_lv2(str_to_txid(data["texture"].get<std::string>().data()),
                                                   sf::Vector2f(data["x"].get<double>(),
                                                                data["y"].get<double>()),
                                                   data["speed"].get<double>(), data["time"].get<double>());
                case JUNKO_SPELL1:
                {
                        auto &curve_middle = data["curve_middle"].get<picojson::object>();
                        auto &curve_end = data["curve_end"].get<picojson::object>();
                        return junko_spellcard(str_to_txid(data["texture"].get<std::string>().data()),
                                               sf::Vector2f(450, 800),
                                               sf::Vector2f(curve_middle["x"].get<double>(), curve_middle["y"].get<double>()),
                                               sf::Vector2f(curve_end["x"].get<double>(), curve_end["y"].get<double>()),
                                               data["time_offset"].get<double>(),
                                               data["speed"].get<double>(),
                                               data["r"].get<double>(),
                                               data["num"].get<double>(),
                                               data["time"].get<double>());
                }
                case NORMAL_ENEMY_SHOT1:
                        return normal_enemy_shot1(str_to_txid(data["texture"].get<std::string>().data()),
                                                  sf::Vector2f(data["x"].get<double>(),
                                                               data["y"].get<double>()),
                                                  data["r"].get<double>(),
                                                  data["speed"].get<double>(),
                                                  data["phase"].get<double>(),
                                                  data["num"].get<double>(),
                                                  data["time"].get<double>());
                case WINDER1:
                        return winder(str_to_txid(data["texture"].get<std::string>().data()),
                                      sf::Vector2f(data["x"].get<double>(),
                                                   data["y"].get<double>()),
                                      data["r"].get<double>(),
                                      data["toward"].get<double>(),
                                      data["unit_rad"].get<double>(),
                                      data["num"].get<double>(),
                                      data["time"].get<double>(),
                                      data["speed"].get<double>(),
                                      data["distance"].get<double>(),
                                      data["term"].get<double>());
                case STRAGHT_CIRCLE:
                        return straight_circle(str_to_txid(data["texture"].get<std::string>().data()),
                                               sf::Vector2f(data["x"].get<double>(),
                                                            data["y"].get<double>()),
                                               data["r"].get<double>(),
                                               data["speed"].get<double>(),
                                               data["num"].get<double>(),
                                               data["multi_shot_num"].get<double>(),
                                               data["delay"].get<double>(),
                                               data["time"].get<double>(),
                                               data["phase"].get<double>());
                case DELAY_CIRCLE_MOVE_LINEAR:
                        return delay_circle_move_linear(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                sf::Vector2f(data["x"].get<double>(),
                                             data["y"].get<double>()),
                                data["rotate_times"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>(),
                                data["angle"].get<double>(),
                                data["num"].get<double>(),
                                data["delay"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
		default:
                        return std::vector<BulletData *>();
                }
        }

        std::vector<BulletData *> expand_dynamic_macro(picojson::object &data,
                                                       DrawableCharacter *running_char, DrawableCharacter *udon,
                                                       BulletData *bullet_data)
        {
                switch(str_to_macroid(data["ID"].get<std::string>().c_str())){
                case CIRCLE:
                        return macro::circle(
                                running_char->get_origin(),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                case ELLIPSE:
                        return macro::ellipse(
                                running_char->get_origin(),
                                data["r"].get<double>(),
                                data["a"].get<double>(),
                                data["b"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                        break;
                case UDON_SPELL1:
                        return macro::udon_spellcard1(sf::Vector2f(data["x"].get<double>(),
                                                                   data["y"].get<double>()),
                                                      running_char->get_origin(),
                                                      sf::Vector2f(data["speed_x"].get<double>(),
                                                                   data["speed_y"].get<double>()),
                                                      data["switch_time"].get<double>(),
                                                      data["time"].get<double>(),
                                                      data["r"].get<double>());
                case UDON_SPELL2:
                        return macro::udon_spellcard2(data["speed"].get<double>(),
                                                      data["enable_time"].get<double>(),
                                                      data["disable_time"].get<double>(),
                                                      data["time"].get<double>(),
                                                      data["r"].get<double>(),
                                                      data["num"].get<double>());
                case UDON_SPELL4:
                        return macro::udon_spellcard4(str_to_txid(data["texture"].get<std::string>().data()),
                                                      sf::Vector2f(data["x"].get<double>(),
                                                                   data["y"].get<double>()),
                                                      data["speed"].get<double>(),
                                                      data["enable_time"].get<double>(),
                                                      data["disable_time"].get<double>(),
                                                      data["cast_times"].get<double>(),
                                                      data["time"].get<double>(),
                                                      data["time_offset"].get<double>(),
                                                      data["r"].get<double>(),
                                                      data["num"].get<double>());
                case HART:
                        return hart(
                                running_char->get_origin(),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
                case N_WAY:
                        return n_way(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                running_char->get_origin(),
                                data["r"].get<double>(),
                                data["angle"].get<double>(),
                                data["width"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                data["speed"].get<double>()
                                );
		case UDON_TSUJO2:
			return udon_tsujo2(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
				TAKE_DEFAULT_ARG(data, "phase", double, 0));
                case UDON_TSUJO2_HARD:
			return udon_tsujo2_hard(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
				TAKE_DEFAULT_ARG(data, "phase", double, 0));
		case UDON_TSUJO1:
			return udon_tsujo1(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>());
                case MULTI_SHOT:
                        return multi_shot(str_to_txid(data["texture"].get<std::string>().data()),
                                          bullet_data->appear_point,
                                          (int)data["num"].get<double>(),
                                          data["speed"].get<double>(),
                                          geometry::calc_angle(running_char->get_origin(),
                                                               bullet_data->appear_point +
                                                               sf::Vector2f(
                                                                       bullet_data->scale.x * bullet_data->texture->getSize().x * 0.5,
                                                                       bullet_data->scale.y * bullet_data->texture->getSize().y * 0.5)),
                                          data["bias"].get<double>(),
                                          data["time"].get<double>(),
                                          data["distance"].get<double>());
                case JUNKO_SPELL1:
                {
                        auto &curve_middle = data["curve_middle"].get<picojson::object>();
                        auto &curve_end = data["curve_end"].get<picojson::object>();
                        return junko_spellcard(str_to_txid(data["texture"].get<std::string>().data()),
                                               running_char->get_origin(),
                                               sf::Vector2f(curve_middle["x"].get<double>(), curve_middle["y"].get<double>()),
                                               sf::Vector2f(curve_end["x"].get<double>(), curve_end["y"].get<double>()),
                                               data["time_offset"].get<double>(),
                                               data["speed"].get<double>(),
                                               data["r"].get<double>(),
                                               data["num"].get<double>(),
                                               data["time"].get<double>());
                }
                case NORMAL_ENEMY_SHOT1:
                        return normal_enemy_shot1(str_to_txid(data["texture"].get<std::string>().data()),
                                                  sf::Vector2f(data["x"].get<double>(),
                                                               data["y"].get<double>()),
                                                  data["r"].get<double>(),
                                                  data["speed"].get<double>(),
                                                  data["phase"].get<double>(),
                                                  data["num"].get<double>(),
                                                  data["time"].get<double>());
                case WINDER1:
                        return winder(str_to_txid(data["texture"].get<std::string>().data()),
                                      sf::Vector2f(data["x"].get<double>(),
                                                   data["y"].get<double>()),
                                      data["r"].get<double>(),
                                      data["toward"].get<double>(),
                                      data["unit_rad"].get<double>(),
                                      data["num"].get<double>(),
                                      data["time"].get<double>(),
                                      data["speed"].get<double>(),
                                      data["distance"].get<double>(),
                                      data["term"].get<double>());
                case UDON_DELAY_CIRCLE:
                        return delay_circle(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                udon->get_origin(),
                                data["r"].get<double>(),
                                data["speed"].get<double>(),
                                data["quantity"].get<double>(),
                                data["delay"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                case STRAGHT_CIRCLE:
                        return straight_circle(str_to_txid(data["texture"].get<std::string>().data()),
                                               sf::Vector2f(data["x"].get<double>(),
                                                            data["y"].get<double>()),
                                               data["r"].get<double>(),
                                               data["speed"].get<double>(),
                                               data["num"].get<double>(),
                                               data["multi_shot_num"].get<double>(),
                                               data["delay"].get<double>(),
                                               data["time"].get<double>(),
                                               data["phase"].get<double>());
                case DELAY_CIRCLE_MOVE_LINEAR:
                        return delay_circle_move_linear(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                bullet_data->appear_point,
                                data["rotate_times"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>(),
                                geometry::calc_angle(running_char->get_origin(),
                                                     bullet_data->appear_point +
                                                     sf::Vector2f(
                                                             bullet_data->scale.x * bullet_data->texture->getSize().x * 0.5,
                                                             bullet_data->scale.y * bullet_data->texture->getSize().y * 0.5)),
                                data["num"].get<double>(),
                                data["delay"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                case JIKI_NERAI_CIRCLE:
                        return nerai_circle1(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                bullet_data->appear_point,
                                geometry::calc_angle(running_char->get_origin(),
                                                     bullet_data->appear_point -
                                                     sf::Vector2f(
                                                             bullet_data->scale.x * bullet_data->texture->getSize().x * 0.5,
                                                             bullet_data->scale.y * bullet_data->texture->getSize().y * 0.5)),
                                0,
                                data["num"].get<double>(),
                                data["time"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>()
                                );
                case JIKI_HAZUSHI_CIRCLE:
                        return nerai_circle1(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                bullet_data->appear_point,
                                geometry::calc_angle(running_char->get_origin(),
                                                     bullet_data->appear_point -
                                                     sf::Vector2f(
                                                             bullet_data->scale.x * bullet_data->texture->getSize().x * 0.5,
                                                             bullet_data->scale.y * bullet_data->texture->getSize().y * 0.5)),
                                data["offset_angle"].get<double>(),
                                data["num"].get<double>(),
                                data["time"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>()
                                );
                case UDON_JIKI_NERAI_CIRCLE:
                        return udon_nerai_circle1(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                bullet_data->appear_point,
                                geometry::calc_angle(running_char->get_origin(),
                                                     bullet_data->appear_point +
                                                     sf::Vector2f(
                                                             bullet_data->scale.x * bullet_data->texture->getSize().x * 0.5,
                                                             bullet_data->scale.y * bullet_data->texture->getSize().y * 0.5)),
                                0,
                                data["num"].get<double>(),
                                data["time"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>(),
                                data["stop"].get<double>()
                                );
                case UDON_JIKI_HAZUSHI_CIRCLE:
                        return udon_nerai_circle1(
                                str_to_txid(data["texture"].get<std::string>().data()),
                                bullet_data->appear_point,
                                geometry::calc_angle(running_char->get_origin(),
                                                     bullet_data->appear_point +
                                                     sf::Vector2f(
                                                             bullet_data->scale.x * bullet_data->texture->getSize().x * 0.5,
                                                             bullet_data->scale.y * bullet_data->texture->getSize().y * 0.5)),
                                data["offset_angle"].get<double>(),
                                data["num"].get<double>(),
                                data["time"].get<double>(),
                                data["r"].get<double>(),
                                data["speed"].get<double>(),
                                data["stop"].get<double>()
                                );
		default:
                        return std::vector<BulletData *>();
                }
        }
        
}
