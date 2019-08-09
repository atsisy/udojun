#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

namespace geometry {
        inline sf::Vector2f rotate_point(float angle, sf::Vector2f p, sf::Vector2f center = sf::Vector2f(0, 0))
        {
                p -= center;
                return sf::Vector2f(
                        (p.x * std::cos(angle)) - (p.y * std::sin(angle)) + center.x,
                        ((p.x * std::sin(angle)) + (p.y * std::cos(angle)) + center.y)
                        );
	}

        inline sf::Vector2f rotate_point2(float angle, sf::Vector2f p, sf::Vector2f center = sf::Vector2f(0, 0))
        {
                p -= center;
                return sf::Vector2f(
                        (p.x * std::cos(angle)) - (p.y * std::sin(angle)) + center.x,
                        -((p.x * std::sin(angle)) + (p.y * std::cos(angle))) + center.y
                        );
	}
}
