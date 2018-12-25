#include "game_component.hpp"
#include <iostream>

DrawableComponent::DrawableComponent()
{
        hide_flag = false;
}

void DrawableComponent::hide()
{
        hide_flag = true;
}

void DrawableComponent::appear()
{
        hide_flag = false;
}

bool DrawableComponent::visible()
{
        return !hide_flag;
}

Label::Label(const char *str)
        : place(0, 0), color(sf::Color::White)
{      
        if(!this->font.loadFromFile("/home/takai/.fonts/NotoSans-Medium.ttf")){
                std::cout << "failed to load font" << std::endl;
        }
        text.setFont(this->font);
        text.setString(str);
        text.setFillColor(color);
        set_font_size(25);
}

void Label::set_place(i16 x, i16 y)
{
        place.x = x;
        place.y = y;
}

void Label::set_color(sf::Color color)
{
        this->color = color;
        text.setFillColor(color);
}


void Label::set_font_size(u8 size)
{
        this->font_size = size;
        text.setCharacterSize(this->font_size);
}

void Label::set_text(const char *text)
{
        this->text.setString(text);
}

void Label::draw(sf::RenderWindow &window)
{
        window.draw(text);
}

DrawableScoreCounter::DrawableScoreCounter(i64 initial)
        : score_counter(initial), label("0")
{
        label.set_color(sf::Color::White);
}

ScoreCounter<i64> &DrawableScoreCounter::counter_method()
{
        return score_counter;
}

void DrawableScoreCounter::draw(sf::RenderWindow &window)
{
        label.set_text(std::to_string(score_counter.get_score()).c_str());
        label.draw(window);
}

DrawableObject::DrawableObject(sf::Texture *t, sf::Vector2f p)
        : texture(*t), place(p)
{
        sprite.setTexture(texture);
        sprite.setPosition(place);
}

void DrawableObject::set_place(sf::Vector2f &&np)
{
        this->place = np;
        sprite.setPosition(this->place);
}

BackgroundTile::BackgroundTile(sf::Texture *t, sf::Vector2f p)
        : DrawableObject(t, p)
{
        texture.setRepeated(true);
        sprite.setTextureRect(sf::IntRect(0, 0, 1000, 1000));
}

void BackgroundTile::draw(sf::RenderWindow &window)
{
        window.draw(sprite);
}

MoveObject::MoveObject(sf::Texture *t, sf::Vector2f p,
                       std::function<sf::Vector2f(sf::Vector2f &, u64)> f, u64 begin_count)
        : DrawableObject(t, p)
{
        this->move_func = f;
        this->begin_count = begin_count;
}

void MoveObject::move(u64 count)
{
        set_place(move_func(place, begin_count));
}

void MoveObject::draw(sf::RenderWindow &window)
{
        window.draw(sprite);
}

Conflictable::Conflictable(sf::IntRect &rect, bool default_on)
{
        set_rect(rect);
        enable = default_on;
}

Conflictable::Conflictable(bool default_on)
{
        enable = default_on;
}

void Conflictable::conflict_on()
{
        enable = true;
}

void Conflictable::conflict_off()
{
        enable = false;
}

bool Conflictable::check_conflict(Conflictable &obj)
{
        if(!this->enable || !obj.enable)
                return false;

        return this->rect.intersects(obj.rect);
}

void Conflictable::set_rect(sf::IntRect rect)
{
        this->rect = rect;
}

void Conflictable::update_left_top(sf::Vector2f p)
{
        rect.left = p.x;
        rect.top = p.y;
}

Bullet::Bullet(sf::Texture *t, sf::Vector2f p,
               std::function<sf::Vector2f(sf::Vector2f &, u64)> f, u64 begin_count)
        : MoveObject(t, p, f, begin_count), Conflictable(true)
{
        set_rect(sprite.getTextureRect());
        update_left_top(p);
}

bool Bullet::is_finish(sf::IntRect window_rect)
{
        return !window_rect.intersects(rect);
}

void Bullet::move(u64 count)
{
        MoveObject::move(count);
        update_left_top(place);
}
