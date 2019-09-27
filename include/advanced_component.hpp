#pragma once

#include "game_component.hpp"
#include "animation.hpp"
#include "move_func.hpp"
#include "rotate_func.hpp"
#include "key_input.hpp"

class Tachie : public MoveObject {
private:
        std::string name;
public:
	Tachie(sf::Texture *t, sf::Vector2f p,
	       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
               std::function<float(Rotatable *, u64, u64)> r_fn,
               u64 begin_count, std::string name);
        void move(u64 count);
        bool are_you(std::string name);
};

class DrawableKeyboard : public DrawableComponent {
private:
        key::KeyboardListener listener;
        std::vector<std::vector<DynamicText *>> keymap;
        MoveObject *cursor_object;
        sf::Vector2f position;
        sf::Vector2i cursor;
        sf::Vector2i map_size;
        std::string buffer;
        util::str_hash<std::vector<std::function<void(key::KeyStatus)>>> user_handler_func;

        void arrow_right_handler(key::KeyStatus status);
        void arrow_left_handler(key::KeyStatus status);
        void arrow_up_handler(key::KeyStatus status);
        void arrow_down_handler(key::KeyStatus status);

        void typed_handler(key::KeyStatus status);

        void realloc_cursor(void);

public:
        DrawableKeyboard(sf::Vector2f pos, sf::Font *font, u64 count);
        
        void draw(sf::RenderWindow &window) override;
        void move(u64 count);
        void check(void);

        std::string get_buffer(void);
        void clear_buffer(void);
        
        void register_handler_function(std::string key, std::function<void(key::KeyStatus)> fn);
};

class EffectableGroup : public DrawableComponent {
public:
        virtual void draw(sf::RenderWindow &window) = 0;
        virtual void effect(u64 count) = 0;
};

class RainWave : public EffectableGroup {
private:
        std::vector<MoveObject *> waves;
        
public:
        RainWave(sf::Vector2f pos, u64 count);
        
        void effect(u64 count) override;
        void draw(sf::RenderWindow &window) override;
};
