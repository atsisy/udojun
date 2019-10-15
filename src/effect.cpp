#include "effect.hpp"
#include "gm.hpp"
#include <iostream>

std::function<void(MoveObject *, u64, u64)> effect::none(void)
{
        return [=](MoveObject *obj, u64 now, u64 begin){
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_in(u64 distance)
{
	return [=](MoveObject *obj, u64 now, u64 begin){
                       float r;
		       if (now - begin > distance) {
			       r = 255;
		       }else{
			       r = 255.0 * (float)((float)(now - begin) /
						   (float)distance);
		       }
		       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_in(u64 distance, u64 called)
{
	return [=](MoveObject *obj, u64 now, u64 begin){
                       float r;
                       begin = called;
		       if (now - begin > distance) {
			       r = 255;
		       }else{
			       r = 255.0 * (float)((float)(now - begin) /
						   (float)distance);
		       }
		       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_out(u64 distance)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       float r;
                       if (now - begin > distance) {
                               r = 0;
                       } else {
                               r = 255.0 *
                                       (1 - (float)((float)(now - begin) / (float)distance));
                       }
                       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_out_later(u64 distance, u64 later)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - begin;
                       u64 after_enabled = 0;
                       float r;

                       if(past < later){
                               return;
                       }

                       after_enabled = past - later;
                       
                       if (after_enabled > distance) {
                               r = 0;
                       } else {
                               r = 255.0 *
                                       (1 - (float)((float)(after_enabled) / (float)distance));
                       }
                       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_in_later(u64 distance, u64 later, float init_alpha)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - begin;
                       u64 after_enabled = 0;
                       float r;

                       if(past < later){
                               return;
                       }

                       after_enabled = past - later;
                       
                       if (after_enabled > distance) {
                               r = 255;
                       } else {
                               r = init_alpha * (float)((float)(after_enabled) /
						   (float)distance);
                       }
                       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::fade_out(u64 distance, u64 called, float init_alpha)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       float r;
                       begin = called;
                       if (now - begin > distance) {
                               r = 0;
                       } else {
                               r = init_alpha *
                                       (1 - (float)((float)(now - begin) / (float)distance));
                       }
                       obj->set_alpha(r);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::flashing(u64 hz)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - begin;
                       if((past / hz) % 2 == 0){
                               obj->set_alpha(0);
                       }else{
                               obj->set_alpha(255);
                       }
               };
}


std::function<void(MoveObject *, u64, u64)> effect::bullet_conflict_on_at(u64 calling, u64 offset)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       if (now == (calling + offset)) {
                               dynamic_cast<Bullet *>(obj)->conflict_on();
                       }
               };
}


std::function<void(MoveObject *, u64, u64)> effect::kill_at(u64 time)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       if (now == (time + begin)) {
                               obj->hide();
                       }
	};
}

std::function<void(MoveObject *, u64, u64)> effect::kill_at(u64 time, u64 called)
{
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       begin = called;
                       if (now == (time + begin)) {
                               obj->hide();
                       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::scale_effect(sf::Vector2f init_scale, sf::Vector2f end_scale, u64 time)
{
        float unit_x = (end_scale.x - init_scale.x) / (float)(time);
        float unit_y = (end_scale.y - init_scale.y) / (float)(time);
        
	return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - begin;
                       if(past <= time){
                               sf::Vector2f next_scale = init_scale + sf::Vector2f(unit_x * past, unit_y * past);
                               obj->set_scale(next_scale);
                       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::animation_effect(std::vector<TextureID> frames, u64 frame_time)
{
        return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - begin;
		       if ((past % frame_time) == 0) {
			       obj->change_texture(
                                       GameMaster::texture_table[frames[(past / frame_time) % (frames.size())]]
                                       );
		       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::keep_origin(sf::Vector2f origin)
{
        return [=](MoveObject *obj, u64 now, u64 begin) {
                       sf::Vector2u texture_size = obj->get_texture_size();
                       sf::Vector2f scale = obj->get_scale();

                       sf::Vector2f diff(
                               (float)texture_size.x * scale.x / 2.f,
                               (float)texture_size.y * scale.y / 2.f);
                       
                       obj->set_place(origin - diff);
               };
}

std::function<void(MoveObject *, u64, u64)> effect::rotate_animation(u64 distance, u64 call, float init_angle, float goal_angle)
{
        float frame_angle = (goal_angle - init_angle) / distance;
        return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - call;
                       
                       if (past < distance) {
                               float now_angle = obj->get_angle();
                               obj->rotate(now_angle + frame_angle);
                       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::alpha_animation(u64 distance, u64 call, float init_alpha, float goal_alpha)
{
        float frame_alpha = (goal_alpha - init_alpha) / distance;
        return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - call;
                       
                       if (past < distance) {
                               obj->set_alpha(obj->get_alpha() + frame_alpha);
                       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::override_move_func(
        std::function<sf::Vector2f(MoveObject *, u64, u64)> fn,
        u64 effect_begin, u64 call_offset)
{
        return [=](MoveObject *obj, u64 now, u64 begin) {
                       u64 past = now - effect_begin;
                       if(past == call_offset){
                               obj->override_move_func(fn);
                       }
               };
}

std::function<void(MoveObject *, u64, u64)> effect::call_interface(EffectID id, picojson::object &obj)
{
        switch(id){
        case EID_FADE_IN:
                return effect::fade_in(obj["distance"].get<double>());
        case EID_FADE_IN_REL:
                return effect::fade_in(
                        obj["distance"].get<double>(), obj["called"].get<double>());
        case EID_FADE_OUT:
                return effect::fade_out(obj["distance"].get<double>());
        case EID_FADE_OUT_REL:
                return effect::fade_out(
                        obj["distance"].get<double>(), obj["called"].get<double>());
        case EID_KILL_AT:
                return effect::kill_at(obj["time"].get<double>());
        case EID_KILL_AT_REL:
                return effect::kill_at(obj["time"].get<double>(), obj["called"].get<double>());
        case EID_SCALE_EFFECT:
        {
                auto &init_scale = obj["init_scale"].get<picojson::object>();
                auto &end_scale = obj["end_scale"].get<picojson::object>();
                return effect::scale_effect(
                        sf::Vector2f(init_scale["x"].get<double>(), init_scale["y"].get<double>()),
                        sf::Vector2f(end_scale["x"].get<double>(), end_scale["y"].get<double>()),
                        obj["time"].get<double>());
        }
        case EID_ANIMATION_EFFECT:
        {
                std::vector<TextureID> txid_data;
                auto &frames = obj["frames"].get<picojson::array>();
                for(auto &frame_element : frames){
                        std::string &txid = frame_element.get<std::string>();
                        txid_data.push_back(str_to_txid(txid.data()));
                }
                return effect::animation_effect(
                        txid_data,
                        obj["frame_time"].get<double>());
        }
        case UNKNOWN_EFFECTID:
        default:
                return effect::none();
        }
}
