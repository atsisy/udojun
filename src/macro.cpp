#include <cmath>
#include "gm.hpp"
#include "macro.hpp"

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

        std::vector<BulletData *> udon_circle(sf::Vector2f origin, float speed, float r, u8 num, u64 time, float phase)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = phase;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        ret.push_back(new BulletData(
                                              str_to_bfid("SLOWER1"),
                                              BULLET1,
                                              mf::getting_slower(speed, rad, 0),
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
                                              BULLET1,
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
			auto &&circle_data = udon_circle(origin, 120, 20, 36, time + (i * 15), rad);
			std::copy(std::begin(circle_data), std::end(circle_data), std::back_inserter(ret));
		}
                
                return ret;
        }
        
        std::vector<BulletData *> odd_n_way(sf::Vector2f origin,
                                            float r, float toward,
                                            float unit_rad, u8 num,
                                            u64 time)
	{
                std::vector<BulletData *> ret;
                float rad = toward;

                rad -= (num / 2) * unit_rad;

                do{
                        float c = (rad > (M_PI / 2) && rad < (3* M_PI / 2) ? -6 : 6);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              BULLET_HART,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       (r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       6 * std::cos(rad), 0)),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y - (r * std::sin(rad)))
                                              ));
                        rad += unit_rad;
                }while(--num);

                return ret;
        }

        
        std::vector<BulletData *> even_n_way(sf::Vector2f origin, float r,
                                            float toward, float unit_rad, u8 num, u64 time)
        {
                std::vector<BulletData *> ret;
                float rad = toward;

                rad -= (((num / 2) - 1) * unit_rad) + (unit_rad / 2);

                do{
                        float c = (rad > (M_PI / 2) && rad < (3* M_PI / 2) ? -4 : 4);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              BULLET_HART,
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       (r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       2 * std::cos(rad), 0)),
                                              time,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y - (r * std::sin(rad)))
                                              ));
                        rad += unit_rad;
                }while(--num);

                return ret;
        }

        std::vector<BulletData *> n_way(sf::Vector2f origin, float r,
                                             float toward, float unit_rad, u8 num, u64 time)
        {
                if(num % 2){
                        return odd_n_way(origin, r, toward, unit_rad, num, time);
                }else{
                        return even_n_way(origin, r, toward, unit_rad, num, time);
                }
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
                case HART:
                        return hart(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
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
                case RANDOM_CIRCLES:
                        return random_circles(
                                (int)data["circle_num"].get<double>(),
				(int)data["num"].get<double>(),
                                data["speed"].get<double>(),
                                (int)data["time"].get<double>(),
                                (int)data["distance"].get<double>());
		case N_WAY
				: return n_way(
					  sf::Vector2f(data["x"].get<double>(),
						       data["y"].get<double>()),
					  data["r"].get<double>(),
					  data["angle"].get<double>(),
					  data["width"].get<double>(),
					  data["quantity"].get<double>(),
					  data["time"].get<double>());
		default:
                        return std::vector<BulletData *>();
                }
        }

        std::vector<BulletData *> expand_dynamic_macro(picojson::object &data, DrawableCharacter running_char)
        {
                switch(str_to_macroid(data["ID"].get<std::string>().c_str())){
                case CIRCLE:
                        return macro::circle(
                                running_char.get_origin(),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>(),
                                TAKE_DEFAULT_ARG(data, "phase", double, 0)
                                );
                case HART:
                        return hart(
                                running_char.get_origin(),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
                case N_WAY:
                        return n_way(
                                running_char.get_origin(),
                                data["r"].get<double>(),
                                data["angle"].get<double>(),
                                data["width"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
		case UDON_TSUJO2:
			return udon_tsujo2(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>(),
				TAKE_DEFAULT_ARG(data, "phase", double, 0));
		case UDON_TSUJO1:
			return udon_tsujo1(
				sf::Vector2f(data["x"].get<double>(),
					     data["y"].get<double>()),
				data["time"].get<double>());
		default:
                        return std::vector<BulletData *>();
                }
        }
        
}
