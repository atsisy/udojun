#pragma once

#include "game_component.hpp"
#include "textures.hpp"
#include "picojson.h"

class FlipEffect : public DrawableComponent {
    private:
	std::vector<DrawableObject *> frames;

    public:
	FlipEffect(std::vector<TextureID> textures);
        void draw(sf::RenderWindow &window);
        
};

enum EffectID {
	EID_FADE_IN = 0,
	EID_FADE_OUT,
	EID_FADE_IN_REL,
	EID_FADE_OUT_REL,
	EID_KILL_AT,
	EID_KILL_AT_REL,
	EID_SCALE_EFFECT,
	EID_ANIMATION_EFFECT,
        UNKNOWN_EFFECTID,
};

inline EffectID str_to_effectid(const char *str)
{
        str_to_idx_sub(str, EID_FADE_IN);
        str_to_idx_sub(str, EID_FADE_OUT);
        str_to_idx_sub(str, EID_FADE_IN_REL);
        str_to_idx_sub(str, EID_FADE_OUT_REL);
        str_to_idx_sub(str, EID_KILL_AT);
        str_to_idx_sub(str, EID_KILL_AT_REL);
        str_to_idx_sub(str, EID_SCALE_EFFECT);
        str_to_idx_sub(str, EID_ANIMATION_EFFECT);

        return UNKNOWN_EFFECTID;
}

namespace effect {
        std::function<void(MoveObject *, u64, u64)> none(void);
        std::function<void(MoveObject *, u64, u64)> fade_in(u64 distance);
        std::function<void(MoveObject *, u64, u64)> fade_out(u64 distance);
        std::function<void(MoveObject *, u64, u64)> fade_in(u64 distance, u64 called);
        std::function<void(MoveObject *, u64, u64)> fade_out(u64 distance, u64 called, float init_alpha = 255.f);

        std::function<void(MoveObject *, u64, u64)> fade_out_later(u64 distance, u64 later);
        std::function<void(MoveObject *, u64, u64)> fade_out_later(u64 distance, u64 later, u64 called);
        std::function<void(MoveObject *, u64, u64)> fade_in_later(u64 distance, u64 later, float init_alpha = 255.f);
        
        std::function<void(MoveObject *, u64, u64)> kill_at(u64 time);
        std::function<void(MoveObject *, u64, u64)> kill_at(u64 time, u64 called);
        std::function<void(MoveObject *, u64, u64)> scale_effect(sf::Vector2f begin, sf::Vector2f end, u64 time);
        std::function<void(MoveObject *, u64, u64)> animation_effect(std::vector<TextureID> frames, u64 frame_time);
        std::function<void(MoveObject *, u64, u64)> keep_origin(sf::Vector2f origin);
        std::function<void(MoveObject *, u64, u64)> override_move_func(
                std::function<sf::Vector2f(MoveObject *, u64, u64)> fn,
                u64 effect_begin, u64 call_offset);
        std::function<void(MoveObject *, u64, u64)> bullet_conflict_on_at(u64 calling, u64 offset);

        std::function<void(MoveObject *, u64, u64)> call_interface(EffectID id, picojson::object &obj);
        std::function<void(MoveObject *, u64, u64)> flashing(u64 hz);
        std::function<void(MoveObject *, u64, u64)> rotate_animation(u64 distance, u64 call,
                                                                             float init_angle, float goal_angle);
        std::function<void(MoveObject *, u64, u64)> alpha_animation(u64 distance, u64 call,
                                                                    float init_alpha, float goal_alpha);
} // namespace effect
