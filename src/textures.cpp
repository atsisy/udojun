#include "textures.hpp"
#include <iostream>

void TextureGenerator::draw_rectangle(sf::Uint8 *buf, sf::Vector2f buf_size, sf::Vector2f pos, sf::Vector2f size, sf::Color color)
{
        for(u32 x = pos.x, cnt_x = 0;cnt_x < size.x;x++, cnt_x++){
                for(u32 y = pos.y, cnt_y = 0;cnt_y < size.y;y++, cnt_y++){
                        *(u32 *)(buf + (u32)((u32)(x + (y * buf_size.x)) << 2)) = color.toInteger();
                }
        }
}

sf::Texture *TextureGenerator::ichimatsu(sf::Vector2f size, sf::Vector2f part_size,
                                        std::vector<sf::Color> colors, u64 type)
{
        sf::Texture *t = new sf::Texture;
        if(!t->create(size.x, size.y)){
                std::cerr << "Failed to create texture. fn -> ichimatsu" << std::endl;
                exit(-1);
        }

        sf::Uint8 *pixels = new sf::Uint8[(u32)(size.x * size.y) * 4];
        
        for(u32 x = 0, imt = 0;x < size.x - part_size.x;x += part_size.x, imt++){
                for(u32 y = 0, color_cnt = 0;y < size.y - part_size.y;y += part_size.y, color_cnt++){
                        draw_rectangle(pixels, size, sf::Vector2f(x, y), part_size,
                                       colors.at((color_cnt + (imt & 1)) % colors.size()));
                }
        }

        t->update(pixels);

        //delete pixels;

        return t;
}
