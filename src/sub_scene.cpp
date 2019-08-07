#include "gm.hpp"

SceneSubEvent::SceneSubEvent(sf::Vector2f pos)
        : position(pos)
{}

void SceneSubEvent::set_status(GameState status)
{
        this->current_status = status;
}


GameState SceneSubEvent::get_status(void)
{
        return this->current_status;
}

void SceneSubEvent::pre_process(sf::RenderWindow &window)
{}

void SceneSubEvent::drawing_process(sf::RenderWindow &window)
{}

GameState SceneSubEvent::post_process(sf::RenderWindow &window)
{
        return get_status();
}
