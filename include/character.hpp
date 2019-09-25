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

class DrawableCharacter : public MoveObject, public Conflictable {
protected:
        CharacterAttribute char_info;

public:
        DrawableCharacter(CharacterAttribute attribute, sf::Texture *t,
                          sf::Vector2f p, sf::Vector2f scale,
                          std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                          std::function<float(Rotatable *, u64, u64)> r_fn,
                          u64 begin_count);

        void draw(sf::RenderWindow &window) override;
        
        void move_diff(sf::Vector2f diff);
        void jump_to(sf::Vector2f p);

        void change_textures(sf::Texture *t);

        void rotate(float a) override;
        void call_rotate_func(u64 now, u64 begin) override;
        void rotate_with_func(u64 now) override;
        
};

class PlayerCharacter : public DrawableCharacter {
private:
        sf::Texture core_texture;
        sf::Sprite core_sprite;
        bool enable_core;
        bool enable_shot;
        std::vector<MoveObject *> slaves;

        void set_core_place();
        void update_shinrei_slaves(u64 time, float power);
        void move_shinrei_slaves(u64 time);

public:
        PlayerCharacter(CharacterAttribute attribute, sf::Texture *character,
                        sf::Texture *core, sf::Vector2f p);

        void shot_on(void);
        void shot_off(void);

        bool shot_is_enable(void);

        void core_on(void);
        void core_off(void);
        void move_diff(sf::Vector2f diff);
        void draw(sf::RenderWindow &window);
        void update_slaves(u64 time, float power);
};
