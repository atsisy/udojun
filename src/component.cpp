#include "game_component.hpp"
#include "utility.hpp"
#include <iostream>
#include <cmath>
#include "geometry.hpp"
#include "rotate_func.hpp"
#include "value.hpp"

DrawableComponent::DrawableComponent()
{
        this->draw_depth = 128;
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

void DrawableComponent::set_drawing_depth(u8 depth)
{
        this->draw_depth = depth;
}

u8 DrawableComponent::get_drawing_depth(void)
{
        return this->draw_depth;
}

Label::Label(const wchar_t *str, sf::Font *f)
	: place(0, 0), color(sf::Color(0xa2, 0x93, 0xbd)),
          font(*f)
{
	if (!this->font.loadFromFile("/home/takai/.fonts/azuki.ttf")) {
		std::cout << "failed to load font" << std::endl;
	}
	text.setFont(this->font);
	text.setString(str);
	text.setFillColor(color);
	set_font_size(26);
	text.setOutlineColor(sf::Color(0, 0, 0, 255));
	text.setOutlineThickness(2);
	text.setStyle(sf::Text::Bold);
}

Label::Label(const char *str) : place(0, 0), color(sf::Color::White)
{
	if (!this->font.loadFromFile(
		    "/home/takai/.fonts/NotoSans-Medium.ttf")) {
		std::cout << "failed to load font" << std::endl;
	}
	text.setFont(this->font);
	text.setString(str);
	text.setFillColor(color);
	set_font_size(25);
}

sf::Vector2f Label::get_place(void)
{
        return this->place;
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

std::string Label::get_text(void)
{
        return text.getString();
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

void Label::change_status(GlyphInformation &info)
{
        set_color(info.text_color);
        text.setOutlineColor(info.outline_color);
	text.setOutlineThickness(info.thickness);
	text.setStyle(info.style);
}

void Label::set_alpha(u8 alpha)
{
        sf::Color fill_color = text.getFillColor();
        sf::Color outline_color = text.getOutlineColor();

        fill_color.a = alpha;
        outline_color.a = alpha;

        set_color(fill_color);
        text.setOutlineColor(outline_color);
}

WindowFrame::WindowFrame(sf::IntRect window_rect, sf::IntRect hole)
{
	frames.emplace_back(sf::Vector2f(window_rect.width, hole.top));
	frames.back().setFillColor(sf::Color::Black);
	frames.back().setPosition(0, 0);

	frames.emplace_back(
		sf::Vector2f(window_rect.width,
			     window_rect.height - (hole.top + hole.height)));
	frames.back().setPosition(0, hole.top + hole.height);
	frames.back().setFillColor(sf::Color::Black);

	frames.emplace_back(sf::Vector2f(hole.left, hole.height));
	frames.back().setFillColor(sf::Color::Black);
	frames.back().setPosition(0, hole.top);

	frames.emplace_back(sf::Vector2f(
		window_rect.width - (hole.left + hole.width), hole.height));
	frames.back().setFillColor(sf::Color::Black);
	frames.back().setPosition((hole.left + hole.width), hole.top);
}

void WindowFrame::draw(sf::RenderWindow &window)
{
	for (auto &shape : frames) {
		window.draw(shape);
	}
}

Meter::Meter(sf::Vector2f size, sf::Vector2f frame_w, float initial,
	     float max_val, sf::Color outer_color, sf::Color inner_color,
	     sf::Color inner_shadow_color)
	: place(0, 0), outer(size),
	  inner(sf::Vector2f(size.x - (frame_w.x * 2),
			     size.y - (frame_w.y * 2)))
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
	if (this->value + d < 0) {
		set_value(0);
	} else {
		set_value(this->value + d);
	}
}

void Meter::set_value(float val)
{
	if (val > this->max_value) {
		this->value = max_value;
	} else {
		this->value = val;
	}
	inner.setSize(sf::Vector2f(default_inner_size.x * (value / max_value),
				   default_inner_size.y));
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

float Meter::get_value()
{
	return value;
}

ElapsedCounter::ElapsedCounter(u64 initial, sf::Font *f, float rate)
        : DrawableScoreCounter<u64>(initial, f, rate)
{
        reset_counter(initial);
}

u64 ElapsedCounter::get_elapsed(void)
{
        return last_set - this->counter_method().get_raw_score();
}

u64 ElapsedCounter::get_last_set(void)
{
        return this->last_set;
}

void ElapsedCounter::reset_counter(float value)
{
        this->last_set = value;
        this->counter_method().set_score(value);
}

DrawableObject::DrawableObject(sf::Texture *t, sf::Vector2f p,
			       sf::Vector2f texture_scale)
	: place(p)
{
        change_texture(t, texture_scale);
	sprite.setPosition(place);
}

sf::Vector2f DrawableObject::get_scale(void)
{
        return this->sprite.getScale();
}

void DrawableObject::rotate(float rad)
{
	sprite.rotate(rad);
}

void DrawableObject::set_place(sf::Vector2f np)
{
	this->place = np;
	sprite.setPosition(this->place);
}

void DrawableObject::set_scale(sf::Vector2f scale)
{
	sprite.setScale(scale);
}

void DrawableObject::set_color(sf::Color color)
{
	sprite.setColor(color);
}

void DrawableObject::set_alpha(u8 alpha)
{
        auto col = sprite.getColor();
        col.a = alpha;
        set_color(col);
}

void DrawableObject::set_scale(float x, float y)
{
	sprite.setScale(x, y);
}

void DrawableObject::move_sprite(sf::Vector2f offset)
{
        sf::IntRect rect = sprite.getTextureRect();
        rect.left += offset.x;
        rect.top += offset.y;
        sprite.setTextureRect(rect);
}

void DrawableObject::set_default_origin(void)
{
        sprite.setOrigin(texture->getSize().x / 2,
                         texture->getSize().y / 2);
}

sf::Vector2f DrawableObject::get_origin(void)
{
	return sf::Vector2f(
		place.x + ((texture->getSize().x * sprite.getScale().x) / 2),
		place.y + ((texture->getSize().y * sprite.getScale().y) / 2));
}

sf::Vector2f DrawableObject::get_place(void)
{
        return this->place;
}
       
void DrawableObject::draw(sf::RenderWindow &window)
{
	window.draw(sprite);
}

sf::Vector2f DrawableObject::displaying_size()
{
	sf::Vector2f scale = sprite.getScale();
	sf::Vector2u size = texture->getSize();
	return sf::Vector2f((float)size.x * scale.x, (float)size.y * scale.y);
}

void DrawableObject::set_repeat_flag(bool flag)
{
        texture->setRepeated(flag);
}

void DrawableObject::change_texture(sf::Texture *t)
{
        this->texture = t;
	sprite.setTexture(*texture);
}

void DrawableObject::change_texture(sf::Texture *t, sf::Vector2f scale)
{
        this->texture = t;
	sprite.setTexture(*texture);
        sprite.setScale(scale);
}

BackgroundTile::BackgroundTile(sf::Texture *t, sf::Vector2f p,
			       sf::IntRect sprite_rect,
			       sf::Vector2f texture_scale)
	: DrawableObject(t, p, texture_scale), init_position(p)
{
	texture->setRepeated(true);
	sprite.setTextureRect(
                sf::IntRect(
                        sprite_rect.left, sprite_rect.top,
                        sprite_rect.width / texture_scale.x,
                        sprite_rect.height / texture_scale.y));
        scroll_speed = 0;
}

void BackgroundTile::draw(sf::RenderWindow &window)
{
	window.draw(sprite);
}

void BackgroundTile::set_scroll_speed(i32 speed)
{
        this->scroll_speed = speed;
}

void BackgroundTile::scroll(i32 speed)
{
	static float remain;
	const float mod = (u64)(speed + remain) % texture->getSize().y;
        
        set_place(sf::Vector2f(init_position.x, init_position.y + mod - init_position.y));

        remain = mod;
}

void BackgroundTile::scroll(void)
{
        scroll(scroll_speed);
}

void BackgroundTile::scroll(
	std::function<sf::Vector2f(sf::Vector2f init, sf::Vector2f current)> f)
{
        sf::Vector2f &&p = f(init_position, this->get_place());
        set_place(p);
}

MoveObject::MoveObject(sf::Texture *t, sf::Vector2f p,
		       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                       std::function<float(Rotatable *, u64, u64)> r_fn,
		       u64 begin_count)
	: DrawableObject(t, p), Rotatable(r_fn)
{
        this->init_pos = p;
	this->move_func = f;
	this->begin_count = begin_count;
}

void MoveObject::add_effect(std::vector<std::function<void(MoveObject *, u64, u64)>> fn)
{
        util::concat_container<std::vector<std::function<void(MoveObject *, u64, u64)> > >(effects, fn);
}

void MoveObject::move(u64 count)
{
	set_place(move_func(this, count, begin_count));
}

void MoveObject::draw(sf::RenderWindow &window)
{
	window.draw(sprite);
}

void MoveObject::effect(u64 count)
{
	for (auto &&fn : effects) {
                try {
                        fn(this, count, begin_count);
                }
                catch (std::bad_function_call& e) {
                        std::cout << "bad function call" << std::endl;
                }
	}
}

void MoveObject::override_move_func(std::function<sf::Vector2f(MoveObject *, u64, u64)> fn)
{
        this->move_func = fn;
}

void MoveObject::rotate(float a)
{
        sprite.rotate(-this->angle * 180 / M_PI);
        this->angle = a;
        
        sprite.rotate(this->angle * 180 / M_PI);
}

void MoveObject::call_rotate_func(u64 now, u64 begin)
{
        sprite.rotate(-this->angle * 180 / M_PI);

        this->angle = rotate_func(this, now, begin);
        
        sprite.rotate(this->angle * 180 / M_PI);
}

void MoveObject::rotate_with_func(u64 now)
{
        this->call_rotate_func(now, this->begin_count);
}

sf::Vector2f MoveObject::get_initial_position(void)
{
        return this->init_pos;
}

void MoveObject::move_diff(sf::Vector2f diff)
{
        this->place += diff;
}

void MoveObject::clear_effect_queue(void)
{
        this->effects.clear();
}

Conflictable::Conflictable(sf::Vector2f &p, bool default_on)
        : center(p), offset(0, 0)
{
	enable = default_on;
}

Conflictable::Conflictable(bool default_on)
        : offset(0, 0)
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
	if (!this->enable || !obj.enable)
		return false;
        
	return (std::pow(obj.center.x - this->center.x, 2) +
		std::pow(obj.center.y - this->center.y, 2)) <
	       std::pow(obj.r + this->r, 2);
}

void Conflictable::update_center(sf::Vector2f p)
{
	this->center = p + offset;
}

void Conflictable::set_radius(float r)
{
	this->r = r;
}

float Conflictable::get_radius(void)
{
        return r;
}

void Conflictable::move_center(sf::Vector2f d)
{
	center.x += d.x;
	center.y += d.y;
}

float Conflictable::distance(Conflictable *c)
{
        return std::sqrt(std::pow(this->center.x - c->center.x, 2)
                         + std::pow(this->center.y - c->center.y, 2));
}

float Conflictable::outer_distance(Conflictable *c)
{
	return distance(c) - (this->r + c->r);
}

sf::Vector2f *Conflictable::get_homing_point(void)
{
        return &center;
}

void Conflictable::set_conflict_offset(sf::Vector2f offset)
{
        this->offset = offset;
}

Rotatable::Rotatable(void)
{
        this->angle = 0;
        rotate_func = rotate::stop;
}

Rotatable::Rotatable(std::function<float(Rotatable *, u64, u64)> fn)
{
        this->angle = 0;
        rotate_func = fn;
}

float Rotatable::get_angle(void)
{
        return angle;
}

Bullet::Bullet(sf::Texture *t, sf::Vector2f p,
	       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
	       u64 begin_count, sf::Vector2f scale, float radius,
               bool conflictable, bool grazable, float init_rotate)
	: MoveObject(t, p, f, rotate::stop, begin_count), Conflictable(true), grazable(grazable)
{
        set_scale(scale);
        rotate(init_rotate);
	set_radius(radius);
}

bool Bullet::is_finish(sf::IntRect window_rect)
{
 	return !window_rect.contains(center.x, center.y);
}

void Bullet::move(u64 count)
{
	MoveObject::move(count);
	update_center(get_place());
}

bool Bullet::is_grazable(void)
{
        return this->grazable;
}

void Bullet::disable_graze(void)
{
        grazable = false;
}

void Bullet::enable_graze(void)
{
	grazable = true;
}

void Bullet::rotate(float a)
{
        MoveObject::rotate(a);
        
        sf::Vector2f relative_center = sf::Vector2f(
                ((texture->getSize().x / 2) * get_scale().x),
                ((texture->getSize().y / 2) * get_scale().y)
                );

        relative_center = geometry::rotate_point(get_angle(), relative_center);
        
        set_conflict_offset(relative_center);
        update_center(get_place());
}

void Bullet::call_rotate_func(u64 now, u64 begin)
{
        MoveObject::call_rotate_func(now, begin);
        
        sf::Vector2f relative_center = sf::Vector2f(
                ((texture->getSize().x / 2) * get_scale().x),
                ((texture->getSize().y / 2) * get_scale().y)
                );

        relative_center = geometry::rotate_point(get_angle(), relative_center);
        
        set_conflict_offset(relative_center);
        update_center(get_place());
}

void Bullet::rotate_with_func(u64 now)
{
        MoveObject::rotate_with_func(now);
}


void Bullet::draw(sf::RenderWindow &window)
{
        MoveObject::draw(window);
        /*
        sf::CircleShape shape(r);
        shape.setPosition(sf::Vector2f(center.x - r, center.y - r));
        shape.setFillColor(sf::Color(255, 0, 0));
        window.draw(shape);
        */
}

SHOT_MASTER_ID Bullet::get_shot_master_id(void)
{
        return id;
}

void Bullet::set_shot_master_id(SHOT_MASTER_ID id)
{
        this->id = id;
}


SpecialBulletAttribute::SpecialBulletAttribute(float power, i64 score)
{
        this->power = power;
        this->score = score;
}

SpecialBullet::SpecialBullet(sf::Texture *t, sf::Vector2f p,
                           std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                           u64 begin_count, sf::Vector2f scale, float radius,
                           bool conflictable, bool grazable, float init_rotate,
                           SpecialBulletAttribute _attribute)
        : Bullet(t, p, f, begin_count, scale, radius, conflictable, grazable, init_rotate),
          attribute(_attribute)
{}

SpecialBulletAttribute SpecialBullet::get_attribute(void)
{
        return this->attribute;
}

ItemOrder::ItemOrder(u16 power, u16 score)
{
        this->power = power;
        this->score = score;
}

ScoreInformation::ScoreInformation(double power, u64 score, u64 graze, u64 _hit)
        : power(Score::MAX_POWER, power),
          score(Score::MAX_SCORE, score),
          graze(Score::MAX_GRAZE, graze),
          hit(Score::MAX_HIT, _hit)
{}

RaceStatus::RaceStatus(void)
{
        hit_count = 0;
}

u64 RaceStatus::get_hit_count(void)
{
        return this->hit_count;
}

void RaceStatus::hit(void)
{
        hit_count++;
}
