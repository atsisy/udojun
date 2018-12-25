#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <cstring>
#include <iostream>

#define enum_to_str(var) #var

enum TextureID {
        BULLET_HART = 0,
        TILE_HART,
        STICK_MAN,
        UNKNOWN,
};

inline TextureID str_to_txid(const char *str)
{
        if(!strcmp(str, enum_to_str(BULLET_HART))){
                return BULLET_HART;
        }else if(!strcmp(str, enum_to_str(TILE_HART))){
                return TILE_HART;
        }else if(!strcmp(str, enum_to_str(STICK_MAN))){
                return STICK_MAN;
        }

        std::cout << "Unknown texture: " << str << std::endl;
        
        return UNKNOWN;
}

using TextureTable = std::unordered_map<TextureID, sf::Texture *>;

