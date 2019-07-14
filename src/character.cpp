#include "character.hpp"
#include "gm.hpp"
#include "value.hpp"
#include <iostream>

CharacterAttribute::CharacterAttribute(std::string name)
{
        this->name = name;
}

DrawableCharacter::DrawableCharacter(CharacterAttribute attribute,
				     sf::Texture *t, sf::Vector2f p,
				     sf::Vector2f scale)
	: DrawableObject(t, p), Conflictable(true), char_info(attribute)
{
        set_radius(6);
        sprite.setScale(scale.x, scale.y);
        update_center(sf::Vector2f(
                              place.x + ((texture.getSize().x * scale.x) / 2),
                              place.y + ((texture.getSize().y * scale.y) / 2)));
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
        : DrawableCharacter(attribute, character, p,
                            sf::Vector2f(
                                    TextureSize::PLAYER_CHARACTER_SIZE_X / character->getSize().x,
                                    TextureSize::PLAYER_CHARACTER_SIZE_Y / character->getSize().y))
{
        this->core_texture = *core;
        core_sprite.setTexture(core_texture);
        set_core_place();
        core_sprite.setScale(0.11, 0.11);
        core_sprite.setColor(sf::Color(255, 230, 230));
        set_radius(5);
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
        if(center.y + diff.y > 736){
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

EnemyCharacter::EnemyCharacter(CharacterAttribute attribute, sf::Texture *t,
			       sf::Vector2f p, sf::Vector2f scale, float hp_max,
			       float hp_init)
	: DrawableCharacter(attribute, t, p, scale)
{
        this->hp_actual = hp_init;
        this->hp_max = hp_max;
}

float EnemyCharacter::get_hp(void)
{
        return hp_actual;
}

void EnemyCharacter::set_hp(float val)
{
        this->hp_actual = val;
}

void EnemyCharacter::set_hp_max(void)
{
        this->hp_actual = this->hp_max;
}

void EnemyCharacter::damage(float value)
{
        this->hp_actual -= value;
}

bool EnemyCharacter::dead(void)
{
        return hp_actual <= 0;
}
