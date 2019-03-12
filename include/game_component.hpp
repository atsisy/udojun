#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "types.hpp"

class DrawableComponent {
private:
        bool hide_flag;
        
public:
        DrawableComponent();
        virtual ~DrawableComponent(){}
        
        virtual void draw(sf::RenderWindow &window) = 0;

        bool visible();
        void hide();
        void appear();
};

template <typename T>
class ScoreCounter {
private:
        T score;
public:
        ScoreCounter(T initial)
        {
                set_score(initial);
        }

        void add(T val)
        {
                this->score += val;
        }

        void sub(T val)
        {
                this->score -= val;
        }

        T get_score()
        {
                return this->score;
        }
        
        void set_score(T val)
        {
                this->score = val;
        }
};

class Label : public DrawableComponent {
private:
        sf::Text text;
        sf::Font font;
        sf::Vector2f place;
        sf::Color color;
        u8 font_size;
        
public:
        Label(const wchar_t *str);
        Label(const char *str);

        void set_place(i16 x, i16 y);
        void set_color(sf::Color color);
        void set_font_size(u8 size);
        void set_text(const char *text);
        
        void draw(sf::RenderWindow &window) override;
};

class WindowFrame : public DrawableComponent {
private:
        std::vector<sf::RectangleShape> frames;

public:
        WindowFrame(sf::IntRect window_rect, sf::IntRect hole);
        void draw(sf::RenderWindow &window) override;
};

class Meter : public DrawableComponent {
private:
        sf::Vector2f place;
        float value;
        float max_value;
        sf::RectangleShape outer;
        sf::RectangleShape inner;
        sf::RectangleShape inner_shadow;
        sf::Vector2f default_inner_size;
        sf::Vector2f frame_width;
        sf::Color inner_color;
        sf::Color outer_color;
        sf::Color inner_shadow_color;
        
public:
        Meter(sf::Vector2f size, sf::Vector2f frame_width, float initial, float max_val,
              sf::Color outer_color, sf::Color inner_color, sf::Color inner_shadow_color);

        void set_place(i16 x, i16 y);

        sf::Vector2f get_place();
        sf::Vector2f get_size();
        float get_value();

        void set_inner_color(sf::Color color);
        void set_outer_color(sf::Color color);
        void set_inner_shadow_color(sf::Color color);
        void add(float d);
        void set_value(float val);
        
        void draw(sf::RenderWindow &window) override;
};


class DrawableScoreCounter : public DrawableComponent {
private:
        ScoreCounter<i64> score_counter;
        Label label;

public:
        DrawableScoreCounter(i64 initial);
        void draw(sf::RenderWindow &window) override;
        ScoreCounter<i64> &counter_method();
};

class DrawableObject : public DrawableComponent {
protected:
        sf::Texture texture;
        sf::Sprite sprite;
        sf::Vector2f place;

        void set_place(sf::Vector2f &&np);
public:
        DrawableObject(sf::Texture *t, sf::Vector2f p, sf::Vector2f texture_scale = sf::Vector2f(1.0, 1.0));
        void draw(sf::RenderWindow &window) override;
        sf::Vector2f get_origin();
        void set_scale(sf::Vector2f scale);
        void set_scale(float x, float y);
        void set_color(sf::Color color);
        sf::Vector2f displaying_size();
        void rotate(float rad);
};

class BackgroundTile : public DrawableObject {
private:

public:
        BackgroundTile(sf::Texture *t, sf::Vector2f p, sf::IntRect sprite_rect, sf::Vector2f texture_scale);
        void draw(sf::RenderWindow &window) override;
        void scroll(i32 speed);
};

class MoveObject : public DrawableObject {
protected:
        u64 begin_count;
        std::function<sf::Vector2f(sf::Vector2f &, u64, u64)> move_func;
        
public:
        MoveObject(sf::Texture *t, sf::Vector2f p,
                   std::function<sf::Vector2f(sf::Vector2f &, u64, u64)> f, u64 begin_count);
        void move(u64 count);
        void draw(sf::RenderWindow &window) override;
};

class Conflictable {
protected:
        sf::Vector2f center;
        float r;
        bool enable;
        
public:
        Conflictable(sf::Vector2f &p, bool default_on);
        Conflictable(bool default_on);
        
        void conflict_on();
        void conflict_off();
        bool check_conflict(Conflictable &obj);
        void update_center(sf::Vector2f p);
        void move_center(sf::Vector2f d);
        void set_radius(float r);
};

class Bullet : public MoveObject, public Conflictable {
        
public:
        Bullet(sf::Texture *t, sf::Vector2f p,
               std::function<sf::Vector2f(sf::Vector2f &, u64, u64)> f, u64 begin_count);
        bool is_finish(sf::IntRect window_rect);
        void move(u64 count);
};
