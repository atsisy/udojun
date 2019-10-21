#pragma once

#include <list>
#include "move_func.hpp"

class Laser : public DrawableComponent {
private:
        std::list<Bullet *> body;
        std::function<sf::Vector2f(MoveObject *, u64, u64)> move_func;
        sf::Texture *head_texture;
        sf::Texture *body_texture;
        sf::Texture *tail_texture;
        sf::Vector2f begin;
        sf::Vector2f scale;
        u64 length;
        float radius;
        
public:
        Laser(TextureID head_tx, TextureID body_tx, TextureID tail_tx,
              sf::Vector2f begin, std::function<sf::Vector2f(MoveObject *, u64, u64)> fn,
              u64 begin_count, sf::Vector2f scale, float radius, u64 length);
        ~Laser();

        std::list<Bullet *> get_bullet_stream();
        
        void draw(sf::RenderWindow &window) override;
        void move(u64 count);
        bool finish_shot(void);
        sf::Vector2f get_tail_position(void);
};

class StraightLaser : public DrawableComponent {
private:
        std::list<Bullet *> laser;
        sf::Texture *texture;
        sf::Vector2f begin;
        sf::Vector2f end;
        std::function<sf::Vector2f(MoveObject *, u64, u64)> scale_func;

public:
        StraightLaser(sf::Texture *t, sf::Vector2f begin, sf::Vector2f end,
                      std::function<sf::Vector2f(MoveObject *, u64, u64)> fn, u64 begin_count);
        void draw(sf::RenderWindow &window) override;
        void update_scale(u64 count);
        std::list<Bullet *> get_bullet_stream(void);
};
