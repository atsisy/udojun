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
        set_radius(10);
        sprite.setScale(0.80, 0.75);
        update_center(sf::Vector2f(
                              place.x + ((texture.getSize().x * sprite.getScale().x) / 2),
                              place.y + ((texture.getSize().y * sprite.getScale().y) / 2)));
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

PlayerCharacter::PlayerCharacter(CharacterAttribute attribute,
                                 sf::Texture *character, sf::Texture *core, sf::Vector2f p)
        : DrawableCharacter(attribute, character, p)
{
        this->core_texture = *core;
        core_sprite.setTexture(core_texture);
        set_core_place();
        core_sprite.setScale(0.14, 0.14);
        core_sprite.setColor(sf::Color(255, 230, 230));
        set_radius(9);
        core_off();
}

void PlayerCharacter::core_on()
{
        enable_core = true;
}

void PlayerCharacter::core_off()
{
        enable_core = false;
}

void PlayerCharacter::move_diff(sf::Vector2f diff)
{
        if(center.x + diff.x < 32){
                return;
        }
        if(center.x + diff.x > 992){
                return;
        }
        if(center.y + diff.y < 32){
                return;
        }
        if(center.y + diff.y > 768){
                return;
        }

        this->place.x += diff.x;
        this->place.y += diff.y;
        
        sprite.setPosition(place);
        move_center(diff);
        set_core_place();
}

void PlayerCharacter::draw(sf::RenderWindow &window)
{
        DrawableCharacter::draw(window);
        if(enable_core){
                window.draw(core_sprite);
        }
}

void PlayerCharacter::set_core_place()
{
        auto scale = core_sprite.getScale();
        core_sprite.setPosition(
                center.x - ((core_sprite.getTextureRect().width * scale.x) / 2),
                center.y - ((core_sprite.getTextureRect().height * scale.y) / 2));
}
