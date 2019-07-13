#pragma once

#include <SFML/Graphics.hpp>

namespace geometry {
        sf::Vector2f rotate_point(float angle, sf::Vector2f p, sf::Vector2f center = sf::Vector2f(0, 0))
        {
		sf::Transform t;
                return t.rotate(angle).translate(center).transformPoint(p);
	}
}
