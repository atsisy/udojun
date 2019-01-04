#pragma once

#include "move_func.hpp"

class Laser : public DrawableComponent {
private:
        Bullet *head;
        std::vector<Bullet *> body;
        Bullet *tail;
        
public:
        Laser(sf::Vector2f begin, float length, float rad, float speed, u64 count);
        ~Laser();

        std::vector<Bullet *> get_bullet_stream();
        
        void draw(sf::RenderWindow &window) override;
        void move(u64 count);
};

