#pragma once

#include "move_func.hpp"

enum MacroID {
        CIRCLE = 0,
        HART,
        N_WAY,
        UNKNOWN_MACRO,
};

inline MacroID str_to_macroid(const char *str)
{
        if(!strcmp(str, enum_to_str(CIRCLE))){
                return CIRCLE;
        }else if(!strcmp(str, enum_to_str(HART))){
                return HART;
        }else if(!strcmp(str, enum_to_str(N_WAY))){
                return N_WAY;
        }

        std::cout << "Unknown Macro ID: " << str << std::endl;
        
        return UNKNOWN_MACRO;
}


namespace macro {
        std::vector<BulletData *> expand_macro(picojson::object &data);
        std::vector<BulletData *> expand_dynamic_macro(picojson::object &data, DrawableCharacter running_char);
        std::vector<BulletData *> circle(sf::Vector2f origin, float r, u8 num, u64 time, float phase);
        std::vector<BulletData *> hart(sf::Vector2f origin, float r, u8 num, u64 time);

        std::vector<BulletData *> odd_n_way(sf::Vector2f origin, float r,
                                            float toward, float unit_rad, u8 num, u64 time);
        std::vector<BulletData *> even_n_way(sf::Vector2f origin, float r,
                                             float toward, float unit_rad, u8 num, u64 time);
        std::vector<BulletData *> n_way(sf::Vector2f origin, float r,
                                        float toward, float unit_rad, u8 num, u64 time);
}
