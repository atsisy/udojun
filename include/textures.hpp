#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

enum TextureID {
        BULLET_HART = 0,
};

using TextureTable = std::unordered_map<TextureID, sf::Texture *>;

