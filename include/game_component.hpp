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
        Label(const char *str);

        void set_place(i16 x, i16 y);
        void set_color(sf::Color color);
        void set_font_size(u8 size);
        void set_text(const char *text);
        
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

        DrawableObject(sf::Texture *t, sf::Vector2f p);

        void set_place(sf::Vector2f &&np);
};

class BackgroundTile : public DrawableObject {
private:

public:
        BackgroundTile(sf::Texture *t, sf::Vector2f p);
        void draw(sf::RenderWindow &window) override;
};

class MoveObject : public DrawableObject {
protected:
        u64 begin_count;
        std::function<sf::Vector2f(sf::Vector2f &, u64)> move_func;
        
public:
        MoveObject(sf::Texture *t, sf::Vector2f p,
                   std::function<sf::Vector2f(sf::Vector2f &, u64)> f, u64 begin_count);
        void move(u64 count);
        void draw(sf::RenderWindow &window) override;
};

class Conflictable {
protected:
        sf::IntRect rect;
        bool enable;
        
public:
        Conflictable(sf::IntRect &rect, bool default_on);
        Conflictable(bool default_on);
        
        void conflict_on();
        void conflict_off();
        bool check_conflict(Conflictable &obj);
        void set_rect(sf::IntRect rect);
        void update_left_top(sf::Vector2f p);
};

class Bullet : public MoveObject, public Conflictable {
        
public:
        Bullet(sf::Texture *t, sf::Vector2f p,
               std::function<sf::Vector2f(sf::Vector2f &, u64)> f, u64 begin_count);
        bool is_finish(sf::IntRect window_rect);
        void move(u64 count);
};
