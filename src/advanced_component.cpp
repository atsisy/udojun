#include "advanced_component.hpp"

Tachie::Tachie(sf::Texture *t, sf::Vector2f p,
	       std::function<sf::Vector2f(sf::Vector2f &, u64, u64)> f,
	       u64 begin_count)
	: MoveObject(t, p, f, begin_count)
{}

void Tachie::move(u64 count)
{
	MoveObject::move(count);
}
