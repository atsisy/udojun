#pragma once

#include "move_func.hpp"

enum MacroID {
        CIRCLE = 0,
        HART,
        UNKNOWN_MACRO,
};

inline MacroID str_to_macroid(const char *str)
{
        if(!strcmp(str, enum_to_str(CIRCLE))){
                return CIRCLE;
        }else if(!strcmp(str, enum_to_str(HART))){
                return HART;
        }

        std::cout << "Unknown Macro ID: " << str << std::endl;
        
        return UNKNOWN_MACRO;
}


namespace macro {
        std::vector<BulletData *> expand_macro(picojson::object &data);
        std::vector<BulletData *> expand_dynamic_macro(picojson::object &data, DrawableCharacter running_char);
        std::vector<BulletData *> circle(sf::Vector2f origin, float r, u8 num, u64 time);
        std::vector<BulletData *> hart(sf::Vector2f origin, float r, u8 num, u64 time);
}
