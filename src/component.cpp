#include "game_component.hpp"
#include <iostream>
#include <cmath>

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

Label::Label(const wchar_t *str)
        : place(0, 0), color(sf::Color::White)
{      
        if(!this->font.loadFromFile("/home/takai/.fonts/NotoSansCJKjp-Medium.otf")){
                std::cout << "failed to load font" << std::endl;
        }
        text.setFont(this->font);
        text.setString(str);
        text.setFillColor(color);
        set_font_size(25);
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
        text.setPosition(place);
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

WindowFrame::WindowFrame(sf::IntRect window_rect, sf::IntRect hole)
{
        frames.emplace_back(sf::Vector2f(window_rect.width, hole.top));
        frames.back().setFillColor(sf::Color::Black);
        frames.back().setPosition(0, 0);

        frames.emplace_back(sf::Vector2f(window_rect.width, window_rect.height - (hole.top + hole.height)));
        frames.back().setPosition(0, hole.top + hole.height);
        frames.back().setFillColor(sf::Color::Black);

        frames.emplace_back(sf::Vector2f(hole.left, hole.height));
        frames.back().setFillColor(sf::Color::Black);
        frames.back().setPosition(0, hole.top);

        frames.emplace_back(sf::Vector2f(window_rect.width - (hole.left + hole.width), hole.height));
        frames.back().setFillColor(sf::Color::Black);
        frames.back().setPosition((hole.left + hole.width), hole.top);
}

void WindowFrame::draw(sf::RenderWindow &window)
{
        for(auto &shape : frames){
                window.draw(shape);
        }
}

Meter::Meter(sf::Vector2f size, sf::Vector2f frame_w, float initial, float max_val,
             sf::Color outer_color, sf::Color inner_color, sf::Color inner_shadow_color)
        : place(0, 0), outer(size),
          inner(sf::Vector2f(size.x - (frame_w.x * 2), size.y - (frame_w.y * 2)))
{
        this->frame_width = frame_w;
        this->default_inner_size = inner.getSize();
        this->max_value = max_val;
        this->inner_shadow = inner;
        this->inner_color = inner_color;
        this->outer_color = outer_color;
        this->inner_shadow_color = inner_shadow_color;

        set_value(initial);

        set_inner_color(inner_color);
        set_outer_color(outer_color);
        set_inner_shadow_color(inner_shadow_color);

}

void Meter::set_place(i16 x, i16 y)
{
        place.x = x;
        place.y = y;
        outer.setPosition(x, y);
        inner.setPosition(x + frame_width.x, y + frame_width.y);
        inner_shadow.setPosition(inner.getPosition());
}

void Meter::set_inner_color(sf::Color color)
{
        inner.setFillColor(color);
}

void Meter::set_outer_color(sf::Color color)
{
        outer.setFillColor(color);
}

void Meter::add(float d)
{
        set_value(this->value + d);
}

void Meter::set_value(float val)
{
        if(val > this->max_value){
                this->value = max_value;
        }else{
                this->value = val;
        }
        inner.setSize(sf::Vector2f(default_inner_size.x * (value / max_value), default_inner_size.y));
}

void Meter::set_inner_shadow_color(sf::Color color)
{
        inner_shadow.setFillColor(color);
}

void Meter::draw(sf::RenderWindow &window)
{
        window.draw(outer);
        window.draw(inner_shadow);
        window.draw(inner);
}

sf::Vector2f Meter::get_size()
{
        return outer.getSize();
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

sf::Vector2f DrawableObject::get_origin()
{
        return sf::Vector2f(
                place.x + (texture.getSize().x >> 1),
                place.y + (texture.getSize().y >> 1)
                );
}

BackgroundTile::BackgroundTile(sf::Texture *t, sf::Vector2f p)
        : DrawableObject(t, p)
{
        texture.setRepeated(true);
        sprite.setTextureRect(sf::IntRect(0, 0, 960, 736));
}

void BackgroundTile::draw(sf::RenderWindow &window)
{
        window.draw(sprite);
}

void BackgroundTile::scroll(i32 speed)
{
        static i32 i;
        set_place(sf::Vector2f(place.x, place.y + speed));
        if(i % (texture.getSize().y / speed) == 0){
                set_place(sf::Vector2f(place.x, place.y - texture.getSize().y));
        }
        i++;
}

MoveObject::MoveObject(sf::Texture *t, sf::Vector2f p,
                       std::function<sf::Vector2f(sf::Vector2f &, u64, u64)> f, u64 begin_count)
        : DrawableObject(t, p)
{
        this->move_func = f;
        this->begin_count = begin_count;
}

void MoveObject::move(u64 count)
{
        set_place(move_func(place, count, begin_count));
}

void MoveObject::draw(sf::RenderWindow &window)
{
        window.draw(sprite);
}

Conflictable::Conflictable(sf::Vector2f &p, bool default_on)
        : center(p)
{
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



        return (std::pow(obj.center.x - this->center.x, 2) + std::pow(obj.center.y - this->center.y, 2)) <
                std::pow(obj.r + this->r, 2);
}

void Conflictable::update_center(sf::Vector2f p)
{
        this->center = p;
}

void Conflictable::set_radius(float r)
{
        this->r = r;
}

void Conflictable::move_center(sf::Vector2f d)
{
        center.x += d.x;
        center.y += d.y;
}

Bullet::Bullet(sf::Texture *t, sf::Vector2f p,
               std::function<sf::Vector2f(sf::Vector2f &, u64, u64)> f, u64 begin_count)
        : MoveObject(t, p, f, begin_count), Conflictable(true)
{
        update_center(sf::Vector2f(place.x + (texture.getSize().x / 2), place.y + (texture.getSize().y / 2)));
        set_radius(16);
}

bool Bullet::is_finish(sf::IntRect window_rect)
{
        return !window_rect.contains(center.x, center.y);
}

void Bullet::move(u64 count)
{
        MoveObject::move(count);
        update_center(sf::Vector2f(place.x + (texture.getSize().x / 2), place.y + (texture.getSize().y / 2)));
}
