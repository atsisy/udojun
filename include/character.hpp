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
protected:
        CharacterAttribute char_info;

public:
        DrawableCharacter(CharacterAttribute attribute, sf::Texture *t, sf::Vector2f p);

        void draw(sf::RenderWindow &window);
        
        void move_diff(sf::Vector2f diff);
        void jump_to(sf::Vector2f p);

        void change_textures(sf::Texture *t);
};

class PlayerCharacter : public DrawableCharacter {
private:
        sf::Texture core_texture;
        sf::Sprite core_sprite;
        bool enable_core;

        void set_core_place();

public:
        PlayerCharacter(CharacterAttribute attribute, sf::Texture *character, sf::Texture *core, sf::Vector2f p);

        void core_on();
        void core_off();
        void move_diff(sf::Vector2f diff);
        void draw(sf::RenderWindow &window);
};
