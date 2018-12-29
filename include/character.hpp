#pragma once

#include <string>
#include "game_component.hpp"
#include "utility.hpp"

class CharacterAttribute {
private:
        std::string name;

public:
        CharacterAttribute(std::string name);

        std::string get_name();
        void set_name(std::string name);
        
};

class DrawableCharacter : public DrawableObject, public Conflictable {
private:
        CharacterAttribute char_info;

public:
        DrawableCharacter(CharacterAttribute attribute, sf::Texture *t, sf::Vector2f p);

        void draw(sf::RenderWindow &window);
        
        void move_diff(sf::Vector2f diff);
        void jump_to(sf::Vector2f p);

        void change_textures(sf::Texture *t);
};
