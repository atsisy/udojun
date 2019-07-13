#include "game_system.hpp"

ViewDrawer::ViewDrawer(sf::FloatRect area,
		       std::function<void(sf::View *, sf::RenderWindow)> fn)
{
        view = new sf::View(area);
        drawing_func = fn;
}

void ViewDrawer::draw(sf::RenderWindow &window)
{
        drawing_func(view, window);
}

sf::View *ViewDrawer::get_view(void)
{
        return view;
}
