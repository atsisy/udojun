#pragma once

#include "game_component.hpp"

template <class T>
class DynamicComponent {
private:
        std::function<void(T *, u64, u64)> move_func;
        std::vector<std::function<void(T *, u64, u64)> > effects;
	u64 move_start_count;
        u64 kill_count;

protected:
        void set_move_start_count(u64 count)
        {
                this->move_start_count = count;
        }

        u64 get_move_start_count(void)
        {
                return this->move_start_count;
        }

        void call_move_func(T *p, u64 current, u64 begin)
        {
                move_func(p, current, begin);
        }
public:
        DynamicComponent()
        {
                kill_count = -1;
                move_start_count = 0;
        }


        void set_kill_count(u64 c)
        {
                this->kill_count = c;
        }
        
        void set_move_func(std::function<void(T *, u64, u64)> f)
        {
                move_func = f;
        }
        
        void add_effect(std::function<void(T *, u64, u64)> effect)
        {
                effects.push_back(effect);
        }

        bool move_func_is_available(void)
        {
                return (bool)move_func;
        }

	bool is_zombie(u64 current)
        {
                return current > kill_count;
        }
};

class DynamicText : public MoveObject {
private:
        Label label;
        
public:
        DynamicText(const wchar_t *str, sf::Font *font, sf::Vector2f init,
                    std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                    std::function<float(Rotatable *, u64, u64)> r_fn,
                    u64 begin_count, u8 font_size);

        void move(u64 count);
        void draw(sf::RenderWindow &window);
        void set_font_size(u8 size);
        std::string get_text(void);
};

class NovelText : public DrawableComponent {
private:
        std::vector<DynamicText *> text_lines;
        
public:
        NovelText(std::vector<wchar_t *> list, sf::Font *font,
                  sf::Vector2f pos, float offset, u8 font_size);
	void draw(sf::RenderWindow &window) override;
};

class SceneAnimation {
    protected:
	std::vector<MoveObject *> move_objects;

    public:
	SceneAnimation();
	void add_animation_object(MoveObject *obj);
	void draw_animation(sf::RenderWindow &window);
        void flush_effect_buffer(u64 count);
};
