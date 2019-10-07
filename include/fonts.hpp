#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "utility.hpp"

enum FontID {
        JP_DEFAULT = 0,
        UNKNOWN_FID,
};

class FontContainer {
private:
        std::unordered_map<FontID, sf::Font *> font_map;
        
public:
        FontContainer(const char *path);
        sf::Font *get(FontID id)
        {
                return font_map[id];
        }
};

inline FontID str_to_fid(const char *str)
{
        str_to_idx_sub("JP_DEFAULT", JP_DEFAULT);
        return UNKNOWN_FID;
}



