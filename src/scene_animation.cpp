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

DynamicText::DynamicText(const wchar_t *str, sf::Font *font, GlyphInformation &g_info,
                         sf::Vector2f init, std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                         std::function<float(Rotatable *, u64, u64)> r_fn,
                         u64 begin_count, u8 font_size)
        : MoveObject(GameMaster::texture_table[EMPTY_TEXTURE], init, f, r_fn, begin_count), label(str, font)
{
        label.set_place(init.x, init.y);
        label.set_font_size(font_size);
        label.change_status(g_info);
}

void DynamicText::move(u64 current_count)
{
        MoveObject::move(current_count);
        label.set_place(place.x, place.y);
}
        
void DynamicText::draw(sf::RenderWindow &window)
{
        label.draw(window);
}

void DynamicText::set_alpha(u8 alpha)
{
        label.set_alpha(alpha);
}

void DynamicText::set_font_size(u8 size)
{
        label.set_font_size(size);
}

std::string DynamicText::get_text(void)
{
        return label.get_text();
}

void DynamicText::change_text_status(GlyphInformation &info)
{
        this->label.change_status(info);
}

NovelText::NovelText(std::vector<wchar_t *> list, sf::Font *font, sf::Vector2f pos,
		     float offset, u8 font_size)
{
        sf::Vector2f text_pos = pos;
        for(wchar_t *line : list){
                text_lines.push_back(
                        new DynamicText(line, font, GLYPH_DESIGN1,
                                        text_pos, mf::stop, rotate::stop, 0, font_size));
                text_pos.y += (offset + font_size);
        }
}

void NovelText::draw(sf::RenderWindow &window)
{
        for(DynamicText *p : text_lines){
                p->draw(window);
        }
}
