#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "types.hpp"
#include "utility.hpp"

class DrawableComponent {
private:
        bool hide_flag;
        u8 draw_depth;
        
public:
        DrawableComponent();
        virtual ~DrawableComponent(){}
        
        virtual void draw(sf::RenderWindow &window) = 0;

        bool visible(void);
        void hide(void);
        void appear(void);

        void set_drawing_depth(u8 depth);
        u8 get_drawing_depth(void);

};

template <typename T>
class ScoreCounter {
private:
        T score;
        T rate;
public:
        ScoreCounter(T initial, T _rate = 1)
        {
                set_score(initial);
                this->rate = _rate;
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
                return this->score / this->rate;
        }

        T get_raw_score()
        {
                return this->score;
        }
        
        void set_score(T val)
        {
                this->score = val;
        }

        T get_rate(void)
        {
                return this->rate;
        }
};

class GlyphInformation {
public:
        sf::Color text_color;
        sf::Color outline_color;
        float thickness;
        sf::Text::Style style;
};

inline GlyphInformation GLYPH_DESIGN1 = {
        .text_color = sf::Color(sf::Color(0xa2, 0x93, 0xbd)),
        .outline_color = sf::Color(0, 0, 0, 255),
        .thickness = 2,
        .style = sf::Text::Bold
};

inline GlyphInformation GLYPH_DESIGN2 = {
        .text_color = sf::Color(sf::Color(0xff, 0xff, 0xff)),
        .outline_color = sf::Color(0x4c, 0x6c, 0xb3, 255),
        .thickness = 2,
        .style = sf::Text::Bold
};

class Label : public DrawableComponent {
protected:
        sf::Text text;
        sf::Vector2f place;
        sf::Color color;
        sf::Font font;
        u8 font_size;
        
public:
        Label(const wchar_t *str, sf::Font *f);
        Label(const char *str);

        void set_place(i16 x, i16 y);
        void set_color(sf::Color color);
        void set_font_size(u8 size);
        void set_text(const char *text);
        void set_text(const wchar_t *text);
        std::string get_text(void);
        sf::Vector2f get_place(void);
        void set_alpha(u8 alpha);

        void change_status(GlyphInformation &info);
        
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

template <typename T>
class DrawableScoreCounter : public DrawableComponent {
private:
        ScoreCounter<T> score_counter;
        Label label;

public:
        DrawableScoreCounter(T initial, sf::Font *f, T rate = 1)
                : score_counter(initial, rate), label(L"0", f)
        {}
        
        void draw(sf::RenderWindow &window) override
        {
                label.set_text(std::to_string(score_counter.get_score()).c_str());
                label.draw(window);
        }
        
        ScoreCounter<T> &counter_method(void)
        {
                return this->score_counter;
        }
        
        void set_place(i16 x, i16 y)
        {
                label.set_place(x, y);
        }
};

template <>
inline void DrawableScoreCounter<double>::draw(sf::RenderWindow &window)
{
        label.set_text(std::to_string(score_counter.get_score()).c_str());
        label.draw(window);
}

class ElapsedCounter : public DrawableScoreCounter<u64> {
private:
        u64 last_set;

public:
        ElapsedCounter(u64 initial, sf::Font *f, float rate = 1);
        u64 get_elapsed(void);
        u64 get_last_set(void);
        void reset_counter(float value);
};

class DrawableObject : public DrawableComponent {
protected:
        sf::Texture *texture;
        sf::Sprite sprite;
        sf::Vector2f place;

public:
        DrawableObject(sf::Texture *t, sf::Vector2f p, sf::Vector2f texture_scale = sf::Vector2f(1.0, 1.0));
        void draw(sf::RenderWindow &window) override;
        sf::Vector2f get_origin(void);
        sf::Vector2f get_place(void);
        void set_scale(sf::Vector2f scale);
        void set_scale(float x, float y);
        sf::Vector2f get_scale(void);
        sf::Vector2u get_texture_size(void);
        void set_color(sf::Color color);
        virtual void set_alpha(u8 alpha);
        sf::Vector2f displaying_size();
        void rotate(float rad);
        void set_default_origin(void);
        void set_repeat_flag(bool flag);
        void move_sprite(sf::Vector2f offset);
        void set_place(sf::Vector2f np);
        void change_texture(sf::Texture *t, sf::Vector2f scale);
        void change_texture(sf::Texture *t);
};

class BackgroundTile : public DrawableObject {
private:
        sf::Vector2f init_position;
        i32 scroll_speed;
        
public:
        BackgroundTile(sf::Texture *t, sf::Vector2f p, sf::IntRect sprite_rect, sf::Vector2f texture_scale);
        void draw(sf::RenderWindow &window) override;
        void scroll(i32 speed);
        void scroll(void);
	void scroll(std::function<
		    sf::Vector2f(sf::Vector2f init, sf::Vector2f current)>
			    f);
        void set_scroll_speed(i32 speed);
};

class ManualBackground : public DrawableComponent {
private:
        sf::Vector2f init_position;
        
public:
        ManualBackground(sf::Vector2f pos);
        void draw(sf::RenderWindow &window) override;
};

class Rotatable {
protected:
        float angle;
        std::function<float(Rotatable *, u64, u64)> rotate_func;
        
public:
        Rotatable(void);
        Rotatable(std::function<float(Rotatable *, u64, u64)> fn);
        virtual void rotate(float a) = 0;
        virtual void call_rotate_func(u64 now, u64 begin) = 0;
        virtual void rotate_with_func(u64 now) = 0;
        float get_angle(void);
};

class MoveObject : public DrawableObject, public Rotatable {
protected:
        u64 begin_count;
        sf::Vector2f init_pos;
        std::function<sf::Vector2f(MoveObject *, u64, u64)> move_func;
	std::vector<std::function<void(MoveObject *, u64, u64)> > effects;
        
public:
        MoveObject(sf::Texture *t, sf::Vector2f p,
                   std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                   std::function<float(Rotatable *, u64, u64)> r_fn,
                   u64 begin_count);
	void add_effect(std::vector<std::function<void(MoveObject *, u64, u64)>> fn);
        void clear_effect_queue(void);
	virtual void move(u64 count);
        void draw(sf::RenderWindow &window) override;
	void effect(u64 count);
        void override_move_func(std::function<sf::Vector2f(MoveObject *, u64, u64)> fn);

        void rotate(float a) override;
        void call_rotate_func(u64 now, u64 begin) override;
        void rotate_with_func(u64 now) override;

        void move_diff(sf::Vector2f diff);
        
        sf::Vector2f get_initial_position(void);
};

class Conflictable {
protected:
        sf::Vector2f center;
        sf::Vector2f offset;
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
        float get_radius(void);

        sf::Vector2f *get_homing_point(void);

        float distance(Conflictable *c);
	float outer_distance(Conflictable *c);
        void set_conflict_offset(sf::Vector2f offset);
};

using SHOT_MASTER_ID = u64;
constexpr SHOT_MASTER_ID NO_SHOT_MASTER = -1;
constexpr SHOT_MASTER_ID SHOT_MASTER_UNDEFINED = -2;
constexpr SHOT_MASTER_ID RUNNING_CHARACTER_SHOT = -3;
constexpr SHOT_MASTER_ID MAIN_ENEMY_SHOT = -4;
constexpr SHOT_MASTER_ID SPECIAL_ITEM = -5;

class Bullet : public MoveObject, public Conflictable {
private:
        bool grazable;
        SHOT_MASTER_ID id;
        
public:
        Bullet(sf::Texture *t, sf::Vector2f p,
               std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
               u64 begin_count, sf::Vector2f scale, float radius,
               bool Conflictable, bool grazable, float init_rotate = 0);
        bool is_finish(sf::IntRect window_rect);
        bool is_grazable(void);
        void disable_graze(void);
        void enable_graze(void);
        void move(u64 count) override;
        void rotate(float a) override;
        void call_rotate_func(u64 now, u64 begin) override;
        void rotate_with_func(u64 now) override;
        void draw(sf::RenderWindow &window) override;
        SHOT_MASTER_ID get_shot_master_id(void);
        void set_shot_master_id(SHOT_MASTER_ID id);
};

class SpecialBulletAttribute {
public:
        float power;
        i64 score;

        SpecialBulletAttribute(float power, i64 score);
};

class SpecialBullet : public Bullet {
private:
        SpecialBulletAttribute attribute;

public:
        SpecialBullet(sf::Texture *t, sf::Vector2f p,
               std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
               u64 begin_count, sf::Vector2f scale, float radius,
               bool conflictable, bool grazable, float init_rotate,
               SpecialBulletAttribute _attribute);
        
        SpecialBulletAttribute get_attribute(void);
        
};

class ItemOrder {
public:
        u16 power;
        u16 score;

        ItemOrder(u16 power, u16 score); 
};

class ScoreInformation {
public:
        util::FixedCounter<double> power;
        util::FixedCounter<u64> score;
        util::FixedCounter<u64> graze;
        util::FixedCounter<u64> hit;
        
        ScoreInformation(double power, u64 score, u64 graze, u64 _hit);
};

class SaveData {
private:
        std::string name;
        ScoreInformation score_info;
        util::Date date;

public:
        SaveData(std::string _name, ScoreInformation _si, util::Date date);
        ScoreInformation get_score_information(void);
        std::string get_name(void);
        void reset_name(std::string _name);
        util::Date get_date(void);
};

class RaceStatus {
private:
        u64 hit_count;

public:
        void hit(void);
        u64 get_hit_count(void);
        RaceStatus(void);
};
