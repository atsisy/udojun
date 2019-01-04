#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include "utility.hpp"

class TextureGenerator {
private:
        static void draw_rectangle(sf::Uint8 *buf, sf::Vector2f buf_size,
                            sf::Vector2f pos, sf::Vector2f size, sf::Color color);
        
public:
        static sf::Texture *ichimatsu(sf::Vector2f size, sf::Vector2f part_size,
                              std::vector<sf::Color> colors, u64 type);
};

/**
 * テクスチャ追加の仕方
 * 1. textures.jsonにIDとファイルパスのペアを記述する。
 * 2. enum TextureIDにtextures.jsonに書いたIDをそのまま追加する。
 * 3. str_to_txid関数に追記する。
 */
enum TextureID {
        BULLET_HART = 0,
        TILE_HART,
        MOON_CITY_TILE,
        STICK_MAN,
        UDON1,
        UDON2,
        UDON3,
        UDON4,
        UDON5,
        PLAYER_CORE,
        ICHIMATSU1,
        ICHIMATSU2,
        LASER_HEAD1,
        LASER_BODY1,
        LASER_TAIL1,
        UNKNOWN_TXID,
};

enum LaserTextureID {
        
};

inline TextureID str_to_txid(const char *str)
{
        if(!strcmp(str, enum_to_str(BULLET_HART))){
                return BULLET_HART;
        }else if(!strcmp(str, enum_to_str(TILE_HART))){
                return TILE_HART;
        }else if(!strcmp(str, enum_to_str(STICK_MAN))){
                return STICK_MAN;
        }else if(!strcmp(str, enum_to_str(MOON_CITY_TILE))){
                return MOON_CITY_TILE;
        }else if(!strcmp(str, enum_to_str(UDON1))){
                return UDON1;
        }else if(!strcmp(str, enum_to_str(UDON2))){
                return UDON2;
        }else if(!strcmp(str, enum_to_str(UDON3))){
                return UDON3;
        }else if(!strcmp(str, enum_to_str(UDON4))){
                return UDON4;
        }else if(!strcmp(str, enum_to_str(UDON5))){
                return UDON5;
        }else if(!strcmp(str, enum_to_str(PLAYER_CORE))){
                return PLAYER_CORE;
        }else if(!strcmp(str, enum_to_str(ICHIMATSU1))){
                return ICHIMATSU1;
        }else if(!strcmp(str, enum_to_str(ICHIMATSU2))){
                return ICHIMATSU2;
        }else if(!strcmp(str, enum_to_str(LASER_HEAD1))){
                return LASER_HEAD1;
        }else if(!strcmp(str, enum_to_str(LASER_BODY1))){
                return LASER_BODY1;
        }else if(!strcmp(str, enum_to_str(LASER_TAIL1))){
                return LASER_TAIL1;
        }
        
        std::cout << "Unknown texture: " << str << std::endl;
        
        return UNKNOWN_TXID;
}

using TextureTable = std::unordered_map<TextureID, sf::Texture *>;

