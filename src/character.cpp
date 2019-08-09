#include "character.hpp"
#include "gm.hpp"
#include "value.hpp"
#include <iostream>
#include "enemy_character.hpp"

#include "move_func.hpp"
#include "rotate_func.hpp"
#include "geometry.hpp"

DanmakuScheduler danmaku_empty_sched =
        DanmakuScheduler({});

CharacterAttribute::CharacterAttribute(std::string name)
{
        this->name = name;
}

DrawableCharacter::DrawableCharacter(CharacterAttribute attribute,
				     sf::Texture *t, sf::Vector2f p,
				     sf::Vector2f scale,
                                     std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                                     std::function<float(Rotatable *, u64, u64)> r_fn,
                                     u64 begin_count)
	: MoveObject(t, p, f, begin_count), Conflictable(true), Rotatable(r_fn), char_info(attribute)
{
        set_radius(6);
        sprite.setScale(scale.x, scale.y);
        update_center(sf::Vector2f(
                              place.x + ((texture.getSize().x * scale.x) / 2),
                              place.y + ((texture.getSize().y * scale.y) / 2)));
        set_conflict_offset(sf::Vector2f(
                                    ((texture.getSize().x * scale.x) / 2),
                                    ((texture.getSize().y * scale.y) / 2)));
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
        sf::CircleShape shape(r);
        shape.setPosition(sf::Vector2f(center.x - r, center.y - r));
        shape.setFillColor(sf::Color(255, 0, 0));
        window.draw(shape);
        window.draw(sprite);
}

void DrawableCharacter::rotate(float a)
{
        this->angle = a;
}

void DrawableCharacter::call_rotate_func(u64 now, u64 begin)
{
        sprite.rotate(-this->angle * 180 / M_PI);
        
        this->angle = rotate_func(this, now, begin);
        sprite.rotate(this->angle * 180 / M_PI);
        
        sf::Vector2f relative_center = sf::Vector2f(
                ((texture.getSize().x / 2) * get_scale().x),
                ((texture.getSize().y / 2) * get_scale().y)
                );

        relative_center = geometry::rotate_point(get_angle(), relative_center);
        
        set_conflict_offset(relative_center);
        update_center(get_place());
}

void DrawableCharacter::rotate_with_func(u64 now)
{
        this->call_rotate_func(now, this->begin_count);
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
                                    TextureSize::PLAYER_CHARACTER_SIZE_Y / character->getSize().y),
                            mf::stop, rotate::stop, 0)
{
        this->core_texture = *core;
        core_sprite.setTexture(core_texture);
        set_core_place();
        core_sprite.setScale(0.11, 0.11);
        core_sprite.setColor(sf::Color(255, 230, 230));
        set_radius(6);
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
			       sf::Vector2f p, sf::Vector2f scale, u64 begin_count,
                               std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                               std::function<float(Rotatable *, u64, u64)> r_fn,
                               float hp_max, float hp_init, DanmakuScheduler d_sched, bool damage_flag)
	: DrawableCharacter(attribute, t, p, scale, f, r_fn, begin_count), danmaku_sched(d_sched)
{
        this->hp_actual = hp_init;
        this->hp_max = hp_max;
        set_radius(12);
        if(damage_flag)
                damage_on();
        else
                damage_off();
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
        if(damage_enable)
                this->hp_actual -= value;
}

bool EnemyCharacter::dead(void)
{
        return hp_actual <= 0;
}

void EnemyCharacter::move(u64 count)
{
	MoveObject::move(count);
	update_center(get_place());
}

void EnemyCharacter::damage_on(void)
{
        this->damage_enable = true;
}

void EnemyCharacter::damage_off(void)
{
        this->damage_enable = false;
}
