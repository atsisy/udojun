#include "advanced_component.hpp"
#include "gm.hpp"
#include "effect.hpp"
#include "geometry.hpp"

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
        : position(pos)
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

        p = new MoveObject(GameMaster::texture_table[CIRCLE_WHITE_STROKE1],
                                pos, mf::stop, rotate::stop, count);
        p->add_effect({
                        effect::scale_effect(sf::Vector2f(0.005, 0.005), sf::Vector2f(0.295, 0.295), 40),
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

sf::Vector2f RainWave::get_position(void)
{
        return this->position;
}

FallingLeaf::FallingLeaf(sf::Vector2f pos, u64 count)
{
        move_obj = new MoveObject(
                GameMaster::texture_table[((util::generate_random() & 1) == 0) ? LEAF1 : LEAF2],
                pos,
                mf::vector_linear(sf::Vector2f(1, -1)),
                rotate::pendulum(util::generate_random.floating(0.38, 0.42), 70, util::generate_random.floating(-0.1, 0.1)),
                count
                );
        move_obj->set_scale(util::generate_random.floating((float)0.15, (float)0.2), util::generate_random.floating(0.15, 0.2));
}

void FallingLeaf::draw(sf::RenderWindow &window)
{
        move_obj->draw(window);
}

void FallingLeaf::effect(u64 count)
{
        move_obj->move(count);
        move_obj->effect(count);
        move_obj->rotate_with_func(count);
}

sf::Vector2f FallingLeaf::get_position(void)
{
        return move_obj->get_place();
}

ConflictableObject::ConflictableObject(sf::Texture *t, sf::Vector2f p,
                                       std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                                       std::function<float(Rotatable *, u64, u64)> r_fn,
                                       u64 begin_count, sf::Vector2f scale, float radius)
        : MoveObject(t, p, f, r_fn, begin_count),
          Conflictable(true)
{
        set_radius(radius);
        set_scale(scale);
}

void ConflictableObject::move(u64 count)
{
        MoveObject::move(count);
        update_center(get_place());
}

ScreenSaverElement::ScreenSaverElement(sf::Texture *t, sf::Vector2f p,
                                       std::function<float(Rotatable *, u64, u64)> r_fn,
                                       u64 begin_count, sf::Vector2f scale, float radius,
                                       sf::Vector2f init_move_speed, float init_rotate_speed)
        : ConflictableObject(t, p, mf::vector_linear_with_noise(init_move_speed), rotate::constant(init_rotate_speed),
                             begin_count, scale, radius),
          move_speed(init_move_speed), next_move_speed(init_move_speed), rotate_speed(init_rotate_speed)
{}

sf::Vector2f ScreenSaverElement::get_move_speed(void)
{
        return move_speed;
}

void ScreenSaverElement::set_next_move_speed(sf::Vector2f speed)
{
        this->next_move_speed = speed;
}

void ScreenSaverElement::apply_next_speed(void)
{
        this->set_move_speed(next_move_speed);
}

void ScreenSaverElement::set_move_speed(sf::Vector2f speed)
{
        this->move_speed = speed;
        this->override_move_func(mf::vector_linear_with_noise(move_speed));
}

float ScreenSaverElement::get_rotate_speed(void)
{
        return rotate_speed;
}

void ScreenSaverElement::set_rotate_speed(float speed)
{
        this->rotate_speed = speed;
}

ScreenSaver::ScreenSaver(std::vector<ScreenSaverElement *> objs)
        : obj_group(objs)
{}

void ScreenSaver::judge_conflict_window_edge(ScreenSaverElement *p)
{
        sf::Vector2f origin = p->get_origin();
        sf::Vector2f move_speed = p->get_move_speed();
        
        if(origin.x > 1366 || origin.x < 0){
                move_speed.x *= -1;
                p->set_next_move_speed(move_speed);
        }

        if(origin.y > 768 || origin.y < 0){
                move_speed.y *= -1;
                p->set_next_move_speed(move_speed);
        }
}

void ScreenSaver::judge_conflict_each_object(void)
{
        for(size_t i = 0;i < obj_group.size();i++){
                for(size_t j = 0;j < obj_group.size();j++){
                        if(i == j)
                                break;
                        if(obj_group[i]->check_conflict(*obj_group[j])){
                                judge_conflict_each_object_sub(obj_group[i], obj_group[j]);
                        }
                }
        }
}

void ScreenSaver::judge_conflict_each_object_sub(ScreenSaverElement *p1, ScreenSaverElement *p2)
{
        sf::Vector2f p1_origin = p1->get_origin();
        sf::Vector2f p2_origin = p2->get_origin();
        
        sf::Vector2f p1_speed = p1->get_move_speed();
        sf::Vector2f p2_speed = p2->get_move_speed();

        sf::Vector2f p1_next_speed = geometry::calc_conflict_speed(p1_speed, p2_speed, 0.9);
        sf::Vector2f p2_next_speed = geometry::calc_conflict_speed(p2_speed, p1_speed, 0.9);

        if(geometry::distance(p1_origin + p1_next_speed, p2_origin + p2_next_speed) > geometry::distance(p1_origin, p2_origin)){
                p1->set_next_move_speed(p1_next_speed);
                p2->set_next_move_speed(p2_next_speed);       
        }
}

void ScreenSaver::effect(u64 count)
{       
        for(auto p : obj_group){
                p->move(count);
        }

        judge_conflict_each_object();
        
        for(auto p : obj_group){
                judge_conflict_window_edge(p);
                p->apply_next_speed();
        }
}

void ScreenSaver::draw(sf::RenderWindow &window)
{
        for(auto p : obj_group){
                p->draw(window);
        }
}

sf::Vector2f ScreenSaver::get_position(void)
{
        return sf::Vector2f(0, 0);
}

void DrawableStackCounter::shrink_fit(u64 count, i16 offset)
{
        while(offset--){
                delete stack_objects.back();
                stack_objects.pop_back();
        }
}

void DrawableStackCounter::grow_fit(u64 count, i16 offset)
{
        while(offset--){
                auto p = new MoveObject(
                        texture,
                        position + sf::Vector2f(stack_objects.size() * 32 ,0),
                        mf::stop,
                        rotate::stop,
                        count
                        );
                p->set_scale(scale);
                stack_objects.push_back(p);
        }
}

void DrawableStackCounter::add(u64 count, i16 value)
{
        counter.add(value);
        if(value < 0){
                shrink_fit(count, -value);
        }else{
                grow_fit(count, value);
        }
}

void DrawableStackCounter::draw(sf::RenderWindow &window)
{
        for(MoveObject *p : stack_objects){
                p->draw(window);
        }
}

void DrawableStackCounter::effect(u64 count)
{
        for(MoveObject *p : stack_objects){
                p->move(count);
                p->effect(count);
        }
}

sf::Vector2f DrawableStackCounter::get_position(void)
{
        return position;
}

DrawableStackCounter::DrawableStackCounter(sf::Vector2f pos, sf::Vector2f _scale, sf::Texture *t, i16 init, u64 count)
        : counter(init), position(pos), texture(t), scale(_scale)
{
        grow_fit(count, init);
}
