#include "gm.hpp"

SceneAnimation::SceneAnimation()
{}

void SceneAnimation::add_animation_object(MoveObject *obj)
{
        move_objects.push_back(obj);
}

void SceneAnimation::flush_effect_buffer(u64 count)
{
	for (MoveObject *obj : move_objects) {
		obj->effect(count);
	}
}

void SceneAnimation::draw_animation(sf::RenderWindow &window)
{
        for(MoveObject *obj : move_objects){
                obj->draw(window);
        }
}

NovelText::NovelText(std::vector<wchar_t *> list, sf::Font *font, sf::Vector2f pos,
		     float offset, u8 font_size)
{
        sf::Vector2f text_pos = pos;
        for(wchar_t *line : list){
                text_lines.push_back(new DynamicText(line, font, text_pos));
                text_lines.back()->set_font_size(font_size);
                text_pos.y += (offset + font_size);
        }
}

void NovelText::draw(sf::RenderWindow &window)
{
        for(DynamicText *p : text_lines){
                p->draw(window);
        }
}
