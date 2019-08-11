#include "advanced_component.hpp"

Tachie::Tachie(sf::Texture *t, sf::Vector2f p,
	       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
               std::function<float(Rotatable *, u64, u64)> r_fn,
	       u64 begin_count, std::string _name)
	: MoveObject(t, p, f, r_fn, begin_count), name(_name)
{}

void Tachie::move(u64 count)
{
	MoveObject::move(count);
}

bool Tachie::are_you(std::string name)
{
        return this->name == name;
}
