#pragma once

#include "game_component.hpp"
#include "textures.hpp"

class FlipEffect : public DrawableComponent {
    private:
	std::vector<DrawableObject *> frames;

    public:
	FlipEffect(std::vector<TextureID> textures);
        void draw(sf::RenderWindow &window);
        
};

namespace effect {
        std::function<void(MoveObject *, u64, u64)> fade_in(u64 distance);
        std::function<void(MoveObject *, u64, u64)> fade_out(u64 distance);
        std::function<void(MoveObject *, u64, u64)> fade_in(u64 distance, u64 called);
        std::function<void(MoveObject *, u64, u64)> fade_out(u64 distance, u64 called);
        std::function<void(MoveObject *, u64, u64)> kill_at(u64 time);
} // namespace effect
