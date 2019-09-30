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

        float calc_angle(sf::Vector2f a, sf::Vector2f b);
        float convert_to_radian(float angle);
        sf::Vector2f spline_curve(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f v1, sf::Vector2f v2, float t);
        sf::Vector2f vector_translate(sf::Vector3f position, float distance);
        sf::Vector2f random_screen_vertex(void);
        sf::Vector2f random_screen_vertex(sf::FloatRect rect);
        sf::Vector2f calc_conflict_speed(sf::Vector2f speed1, sf::Vector2f spped2, float elast);
        float distance(sf::Vector2f p1, sf::Vector2f p2);
}
