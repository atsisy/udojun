#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include "utility.hpp"

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
        UNKNOWN_TXID,
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
        }

        std::cout << "Unknown texture: " << str << std::endl;
        
        return UNKNOWN_TXID;
}

using TextureTable = std::unordered_map<TextureID, sf::Texture *>;


