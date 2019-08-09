#pragma once

#include "move_func.hpp"

enum MacroID {
	CIRCLE = 0,
        ELLIPSE,
        UDON_ELLIPSE,
	HART,
	UDON_TSUJO1,
	UDON_TSUJO2,
        UDON_SPELL1,
	N_WAY,
        RANDOM_CIRCLES,
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
	} else if (!strcmp(str, enum_to_str(UDON_TSUJO1))) {
		return UDON_TSUJO1;
	} else if (!strcmp(str, enum_to_str(UDON_TSUJO2))) {
		return UDON_TSUJO2;
	}

        str_to_idx_sub(str, RANDOM_CIRCLES);
        str_to_idx_sub(str, ELLIPSE);
        str_to_idx_sub(str, UDON_ELLIPSE);
        str_to_idx_sub(str, UDON_SPELL1);

	std::cout << "Unknown Macro ID: " << str << std::endl;
        
        return UNKNOWN_MACRO;
}


namespace macro {
        std::vector<BulletData *> expand_macro(picojson::object &data);
        std::vector<BulletData *> expand_dynamic_macro(picojson::object &data, DrawableCharacter running_char);
        std::vector<BulletData *> circle(sf::Vector2f origin, float r, u8 num, u64 time, float phase);
        std::vector<BulletData *> ellipse(sf::Vector2f origin, float r, float a, float b,
                                          u8 num, u64 time, float phase);
        std::vector<BulletData *> hart(sf::Vector2f origin, float r, u8 num, u64 time);
	std::vector<BulletData *> udon_tsujo1(sf::Vector2f origin, u64 time);
        
        std::vector<BulletData *> odd_n_way(sf::Vector2f origin,
                                            float r, float toward,
                                            float unit_rad, u8 num,
                                            u64 time);
	std::vector<BulletData *> even_n_way(sf::Vector2f origin, float r,
                                             float toward, float unit_rad, u8 num, u64 time);
        std::vector<BulletData *> n_way(sf::Vector2f origin, float r,
                                        float toward, float unit_rad, u8 num, u64 time);
	std::vector<BulletData *> udon_circle2(sf::Vector2f origin, float speed,
					       float r, u16 num, u64 time,
					       float phase, float unit_rad);
        std::vector<BulletData *> random_circles(u16 circle_num, u16 num,
                                                 float speed, u64 time, u16 distance);
        std::vector<BulletData *> udon_spellcard1(sf::Vector2f origin, sf::Vector2f target,
                                                  float speed, u64 change_time, u64 time,
                                                  float r);
}
