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
        size_t buffer_length_limit;

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

        void set_buffer_length_limit(size_t len);

        std::string get_buffer(void);
        void clear_buffer(void);
        
        void register_handler_function(std::string key, std::function<void(key::KeyStatus)> fn);
};

class EffectableGroup : public DrawableComponent {
public:
        virtual void draw(sf::RenderWindow &window) = 0;
        virtual void effect(u64 count) = 0;
        virtual sf::Vector2f get_position(void) = 0;
};

class RainWave : public EffectableGroup {
private:
        std::vector<MoveObject *> waves;
        sf::Vector2f position;
        
public:
        RainWave(sf::Vector2f pos, u64 count);
        
        void effect(u64 count) override;
        void draw(sf::RenderWindow &window) override;
        sf::Vector2f get_position(void) override;
};

class FallingLeaf : public EffectableGroup {
private:
        MoveObject *move_obj;
        
public:
        FallingLeaf(sf::Vector2f pos, u64 count);
        
        void effect(u64 count) override;
        void draw(sf::RenderWindow &window) override;
        sf::Vector2f get_position(void) override;
};

class ConflictableObject : public MoveObject, public Conflictable {
public:
        ConflictableObject(sf::Texture *t, sf::Vector2f p,
                           std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                           std::function<float(Rotatable *, u64, u64)> r_fn,
                           u64 begin_count, sf::Vector2f scale, float radius);

        void move(u64 count) override;
};

class ScreenSaverElement : public ConflictableObject {
private:
        sf::Vector2f move_speed;
        sf::Vector2f next_move_speed;
        float rotate_speed;

public:
        ScreenSaverElement(sf::Texture *t, sf::Vector2f p,
                           std::function<float(Rotatable *, u64, u64)> r_fn,
                           u64 begin_count, sf::Vector2f scale, float radius,
                           sf::Vector2f init_move_speed, float init_rotate_speed);

        sf::Vector2f get_move_speed(void);
        void set_next_move_speed(sf::Vector2f speed);
        void apply_next_speed(void);
        
        float get_rotate_speed(void);

        void set_move_speed(sf::Vector2f speed);
        void set_rotate_speed(float speed);
};

class ScreenSaver : public EffectableGroup {
private:
        std::vector<ScreenSaverElement *> obj_group;

        void judge_conflict_window_edge(ScreenSaverElement *p);
        void judge_conflict_each_object(void);
        void judge_conflict_each_object_sub(ScreenSaverElement *p1, ScreenSaverElement *p2);

public:
        ScreenSaver(std::vector<ScreenSaverElement *> objs);
        void effect(u64 count) override;
        void draw(sf::RenderWindow &window) override;
        sf::Vector2f get_position(void) override;
};


class DrawableStackCounter : public EffectableGroup {
private:
        util::VariableCounter<i16> counter;
        std::vector<MoveObject *> stack_objects;
        sf::Vector2f position;
        sf::Texture *texture;
        sf::Vector2f scale;

        void shrink_fit(u64 count, i16 offset);
        void grow_fit(u64 count, i16 offset);

public:

        DrawableStackCounter(sf::Vector2f pos, sf::Vector2f _scale, sf::Texture *t, i16 init, u64 count);
        void add(u64 count, i16 value);
        void draw(sf::RenderWindow &window);
        void set_value(i16 value);
        void effect(u64 count);
        sf::Vector2f get_position(void);
        i16 get_count(void);
};

