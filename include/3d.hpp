#pragma once

#include "game_component.hpp"

class DrawableObject3D : public MoveObject {
private:
        sf::Vector3f actual_3d_position;
        std::function<sf::Vector3f(DrawableObject3D *, u64, u64)> move_func_3d;

public:
        DrawableObject3D(sf::Texture *t, sf::Vector3f p,
                         std::function<sf::Vector3f(DrawableObject3D *, u64, u64)> f,
                         std::function<float(Rotatable *, u64, u64)> r_fn,
                         u64 begin_count);
        void draw(sf::RenderWindow &window) override;
        void move(u64 count) override;
        sf::Vector3f get_3d_position(void);
};
