#include "character.hpp"
#include "gm.hpp"
#include <iostream>

CharacterAttribute::CharacterAttribute(std::string name)
{
        this->name = name;
}

DrawableCharacter::DrawableCharacter(CharacterAttribute attribute, sf::Texture *t, sf::Vector2f p)
        : DrawableObject(t, p), Conflictable(true), char_info(attribute)
{
        update_center(sf::Vector2f(place.x + (texture.getSize().x / 2), place.y + (texture.getSize().y / 2)));
        set_radius(14);
        sprite.setScale(0.85, 0.8);
}

void DrawableCharacter::move_diff(sf::Vector2f diff)
{
        this->place.x += diff.x;
        this->place.y += diff.y;
        
        sprite.setPosition(place);
        move_center(diff);
}

void DrawableCharacter::jump_to(sf::Vector2f diff)
{
        this->place.x = diff.x;
        this->place.y = diff.y;
        sprite.setPosition(place);
        update_center(sf::Vector2f(place.x + (texture.getSize().x / 2), place.y + (texture.getSize().y / 2)));
}

void DrawableCharacter::draw(sf::RenderWindow &window)
{
        window.draw(sprite);
}

void DrawableCharacter::change_textures(sf::Texture *t)
{
        this->texture = *t;
        sprite.setTexture(texture);
        sprite.setPosition(place);
}
