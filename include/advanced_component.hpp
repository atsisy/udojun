#pragma once

#include "game_component.hpp"

class Tachie : public MoveObject {
private:
        std::string name;
public:
	Tachie(sf::Texture *t, sf::Vector2f p,
	       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
               std::function<float(Rotatable *, u64, u64)> r_fn,
               u64 begin_count, std::string name);
        void move(u64 count);
        bool are_you(std::string name);
};
