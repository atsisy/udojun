#pragma once

#include "game_component.hpp"

class Tachie : public MoveObject {
private:
        std::string name;
public:
	Tachie(sf::Texture *t, sf::Vector2f p,
	       std::function<sf::Vector2f(sf::Vector2f &, sf::Vector2f &, u64, u64)> f,
               u64 begin_count, std::string name);
        void move(u64 count);
        bool are_you(std::string name);
        
};
