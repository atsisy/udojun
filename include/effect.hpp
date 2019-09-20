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
        std::function<void(MoveObject *, u64, u64)> kill_at(u64 time, u64 called);
        std::function<void(MoveObject *, u64, u64)> scale_effect(sf::Vector2f begin, sf::Vector2f end, u64 time);
        std::function<void(MoveObject *, u64, u64)> animation_effect(std::vector<TextureID> frames, u64 frame_time);
} // namespace effect
