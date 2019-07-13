#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class ViewDrawer {
private:
        sf::View *view;
        std::function<void(sf::View *, sf::RenderWindow)> drawing_func;

public:
        ViewDrawer(sf::FloatRect area,
	       std::function<void(sf::View *, sf::RenderWindow)> fn);

        void draw(sf::RenderWindow &window);
        sf::View *get_view(void);
        
};
