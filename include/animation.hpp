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

class DynamicText : public Label, public DynamicComponent<DynamicText> {
private:
        sf::Vector2f init_position;
        
public:
        DynamicText(const wchar_t *str, sf::Font *f, sf::Vector2f init)
                : Label(str, f), init_position(init)
        {
                Label::set_place(init.x, init.y);
        }

	void set_move_func(std::function<void(DynamicText *, u64, u64)> f, u64 begin)
        {
                set_move_start_count(begin);
                DynamicComponent::set_move_func(f);
        }
        
        void run_animation(
                std::function<void(DynamicText *, u64, u64)> effect)
	{
                add_effect(effect);
        }

        void move(u64 current_count)
        {
                if(move_func_is_available()){
                        if (get_move_start_count() < current_count) {
                                call_move_func(this, current_count,
                                               get_move_start_count());
                        }
                }
        }
        
        void draw(sf::RenderWindow &window)
        {
                Label::draw(window);
        }

        sf::Vector2f get_init_position(void)
        {
                return init_position;
        }
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
