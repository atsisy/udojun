#include "gm.hpp"

SceneSubEvent::SceneSubEvent(sf::Vector2f pos, std::string n)
        : position(pos), name(n), flags(0)
{}

void SceneSubEvent::set_status(GameState status)
{
        this->current_status = status;
}


GameState SceneSubEvent::get_status(void)
{
        return this->current_status;
}

std::string SceneSubEvent::get_name(void)
{
        return name;
}

void SceneSubEvent::pre_process(sf::RenderWindow &window)
{}

void SceneSubEvent::drawing_process(sf::RenderWindow &window)
{}

GameState SceneSubEvent::post_process(sf::RenderWindow &window)
{
        return get_status();
}

bool SceneSubEvent::check_flags(u64 _flags)
{
        return this->flags & _flags;
}

void SceneSubEvent::up_flags(u64 flags)
{
        this->flags |= flags;
}

void SceneSubEvent::down_flags(u64 flags)
{
        this->flags &= ~flags;
}
