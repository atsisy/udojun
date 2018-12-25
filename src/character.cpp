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
        set_rect(sprite.getTextureRect());
}

void DrawableCharacter::move_diff(sf::Vector2f diff)
{
        this->place.x += diff.x;
        this->place.y += diff.y;
        sprite.setPosition(place);
        update_left_top(place);
}

void DrawableCharacter::jump_to(sf::Vector2f diff)
{
        this->place.x = diff.x;
        this->place.y = diff.y;
        sprite.setPosition(place);
        update_left_top(place);
}

void DrawableCharacter::draw(sf::RenderWindow &window)
{
        window.draw(sprite);
}
