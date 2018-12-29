#include <cmath>
#include "gm.hpp"
#include "macro.hpp"

namespace macro {
        
        std::vector<BulletData *> circle(sf::Vector2f origin, float r, u8 num, u64 time)
        {
                std::vector<BulletData *> ret;
                float unit_rad = (2 * M_PI) / (float)num;
                float rad = 0.0;

                for(u8 i = 0;i < num;i++, rad += unit_rad){
                        float c = (rad > (M_PI / 2) && rad < (3* M_PI / 2) ? -2 : 2);
                        ret.push_back(new BulletData(
                                              str_to_bfid("LINEAR"),
                                              (std::abs(std::cos(rad)) < 0.000001 ?
                                               mf::up(c) : mf::linear(
                                                       -(r * std::sin(rad)) / ((r * std::cos(rad))),
                                                       2 * std::cos(rad), 0)),
                                              time++,
                                              sf::Vector2f(
                                                      origin.x + (r * std::cos(rad)),
                                                      origin.y + (r * std::sin(rad)))
                                              ));
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

        std::vector<BulletData *> expand_macro(picojson::object &data)
        {
                switch(str_to_macroid(data["ID"].get<std::string>().c_str())){
                case CIRCLE:
                        return circle(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
                case HART:
                        return hart(
                                sf::Vector2f(data["x"].get<double>(), data["y"].get<double>()),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
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
                                             data["time"].get<double>());
                case HART:
                        return hart(
                                running_char.get_origin(),
                                data["r"].get<double>(),
                                data["quantity"].get<double>(),
                                data["time"].get<double>()
                                );
                default:
                        return std::vector<BulletData *>();
                }
        }
        
}
