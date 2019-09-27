#include "advanced_component.hpp"
#include "gm.hpp"
#include "effect.hpp"

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

DrawableKeyboard::DrawableKeyboard(sf::Vector2f pos, sf::Font *font, u64 count)
        : position(pos), cursor(0, 0)
{
        std::vector<std::vector<const wchar_t *>> raw_map = {
                { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M" },
                { L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" },
                { L"a", L"b", L"c", L"d", L"e", L"f", L"g", L"h", L"i", L"j", L"k", L"l", L"m" },
                { L"n", L"o", L"p", L"q", L"r", L"s", L"t", L"u", L"v", L"w", L"x", L"y", L"z" },
                { L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"+", L"=", L"-" },
                { L".", L",", L"!", L"?", L"@", L":", L";", L"[", L"]", L"(", L")", L"_", L"/" },
                { L"{", L"}", L"|", L"~", L"^", L"#", L"$", L"%", L"&", L"*", L"SPC", L"BS", L"OK" },
        };

        map_size.x = raw_map.at(0).size();
        map_size.y = raw_map.size();

        sf::Vector2f char_pos = pos;
        for(auto &char_vec : raw_map){
                std::vector<DynamicText *> elem;
                for(const wchar_t *p : char_vec){
                        elem.push_back(
                                new DynamicText(
                                        p,
                                        font,
                                        GLYPH_DESIGN2,
                                        char_pos,
                                        mf::random_vibration(char_pos, 4),
                                        rotate::stop,
                                        count,
                                        32
                                        ));
                        char_pos.x += 54;
                }
                char_pos.x = pos.x;
                char_pos.y += 42;
                
                keymap.push_back(elem);
        }

        cursor_object = new MoveObject(
                GameMaster::texture_table[GHOST_ENEMY_TX1],
                pos,
                mf::stop,
                rotate::stop,
                count);
        cursor_object->set_scale(0.03, 0.03);
        cursor_object->add_effect({ effect::animation_effect({ GHOST_ENEMY_TX1,
                                                               GHOST_ENEMY_TX2,
                                                               GHOST_ENEMY_TX3,
                                                               GHOST_ENEMY_TX2,
                                                               GHOST_ENEMY_TX1,
                                                               GHOST_ENEMY_TX4,
                                                               GHOST_ENEMY_TX5,
                                                               GHOST_ENEMY_TX4 }, 4) });

        listener.add_key_event(key::ARROW_KEY_RIGHT,
                               [this](key::KeyStatus status){
                                       this->arrow_right_handler(status);
                               });
        listener.add_key_event(key::ARROW_KEY_LEFT,
                               [this](key::KeyStatus status){
                                       this->arrow_left_handler(status);
                               });
        listener.add_key_event(key::ARROW_KEY_DOWN,
                               [this](key::KeyStatus status){
                                       this->arrow_down_handler(status);
                               });
        listener.add_key_event(key::ARROW_KEY_UP,
                               [this](key::KeyStatus status){
                                       this->arrow_up_handler(status);
                               });

        listener.add_key_event(key::VKEY_1,
                               [this](key::KeyStatus status){
                                       this->typed_handler(status);
                               });
}

void DrawableKeyboard::draw(sf::RenderWindow &window)
{
        cursor_object->draw(window);
        
        for(auto &text_vec : keymap){
                for(DynamicText *p : text_vec){
                        p->draw(window);
                }
        }
}

void DrawableKeyboard::move(u64 count)
{
        cursor_object->move(count);
        cursor_object->effect(count);
        
        for(auto &text_vec : keymap){
                for(DynamicText *p : text_vec){
                        p->move(count);
                }
        }
}

void DrawableKeyboard::realloc_cursor(void)
{
        cursor_object->set_place(sf::Vector2f(
                                         position.x + (cursor.x * 54),
                                         position.y + (cursor.y * 42)
                                         ));
}

void DrawableKeyboard::check(void)
{
        listener.key_update();
}

void DrawableKeyboard::arrow_right_handler(key::KeyStatus status)
{
        if(status & key::KEY_FIRST_PRESSED){
                cursor.x++;
                cursor.x %= map_size.x;
        }

        realloc_cursor();
}

void DrawableKeyboard::arrow_left_handler(key::KeyStatus status)
{
        if(status & key::KEY_FIRST_PRESSED){
                cursor.x--;
                if(cursor.x < 0)
                        cursor.x += map_size.x;
                else
                        cursor.x %= map_size.x;
        }

        realloc_cursor();
}

void DrawableKeyboard::arrow_down_handler(key::KeyStatus status)
{
        if(status & key::KEY_FIRST_PRESSED){
                cursor.y++;
                cursor.y %= map_size.y;
        }

        realloc_cursor();
}

void DrawableKeyboard::arrow_up_handler(key::KeyStatus status)
{
        if(status & key::KEY_FIRST_PRESSED){
                cursor.y--;
                if(cursor.y < 0)
                        cursor.y += map_size.y;
                else
                        cursor.y %= map_size.y;
        }

        realloc_cursor();
}

void DrawableKeyboard::typed_handler(key::KeyStatus status)
{
        if(status & key::KEY_FIRST_PRESSED){
                DynamicText *p = keymap[cursor.y][cursor.x];
                std::string &&key = p->get_text();

                try{
                        /*
                         * 登録されていないとatメソッドで例外を吐く
                         * そこで、デフォルトのハンドラを呼び出す
                         * 登録されていればそれを全て実行し、デフォルトのハンドラは実行しない
                         */
                        auto &&fn_vec = user_handler_func.at(key);
                        for(auto &fn : fn_vec){
                                fn(status);
                        }
                }catch(std::out_of_range &oor){
                        /*
                         * 登録されていなかった
                         */
                        if(key == "BS"){
                                if(buffer.size())
                                        buffer.pop_back();
                        }else if(key == "SPC"){
                                buffer.append(" ");
                        }else{
                                buffer.append(p->get_text());
                        }
                }
        }
}

void DrawableKeyboard::register_handler_function(std::string key, std::function<void(key::KeyStatus)> fn)
{
        try {
                auto &&vec = user_handler_func.at(key);
                vec.push_back(fn);
        }catch(std::out_of_range &oor){
                user_handler_func.emplace(key, std::vector<std::function<void(key::KeyStatus)>>{ fn });
        }
}

std::string DrawableKeyboard::get_buffer(void)
{
        return buffer;
}

void DrawableKeyboard::clear_buffer(void)
{
        buffer.clear();
}

RainWave::RainWave(sf::Vector2f pos, u64 count)
{
        auto p = new MoveObject(GameMaster::texture_table[CIRCLE_WHITE_STROKE1],
                                pos, mf::stop, rotate::stop, count);
        p->add_effect({
                        effect::scale_effect(sf::Vector2f(0.01, 0.01), sf::Vector2f(0.3, 0.3), 40),
                        effect::fade_out_later(20, 20),
                        effect::kill_at(40),
                        effect::keep_origin(pos)
                });
	waves.push_back(p);
}

void RainWave::draw(sf::RenderWindow &window)
{
        for(auto p : waves){
                p->draw(window);
        }
}

void RainWave::effect(u64 count)
{
        for(auto p : waves){
                p->move(count);
                p->effect(count);
        }
}
