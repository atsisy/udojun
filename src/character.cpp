#include "character.hpp"
#include "gm.hpp"
#include "value.hpp"
#include <iostream>
#include <fstream>
#include "enemy_character.hpp"

#include "move_func.hpp"
#include "rotate_func.hpp"
#include "geometry.hpp"

DanmakuScheduler danmaku_empty_sched =
        DanmakuScheduler({});

CharacterAttribute::CharacterAttribute(std::string name)
{
        this->name = name;
}

DrawableCharacter::DrawableCharacter(CharacterAttribute attribute,
				     sf::Texture *t, sf::Vector2f p,
				     sf::Vector2f scale,
                                     std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                                     std::function<float(Rotatable *, u64, u64)> r_fn,
                                     u64 begin_count)
	: MoveObject(t, p, f, r_fn, begin_count), Conflictable(true), char_info(attribute)
{
        set_radius(6);
        sprite.setScale(scale.x, scale.y);
        update_center(sf::Vector2f(
                              place.x + ((texture->getSize().x * scale.x) / 2),
                              place.y + ((texture->getSize().y * scale.y) / 2)));
        set_conflict_offset(sf::Vector2f(
                                    ((texture->getSize().x * scale.x) / 2),
                                    ((texture->getSize().y * scale.y) / 2)));
}

void DrawableCharacter::move_diff(sf::Vector2f diff)
{
        this->place.x += diff.x;
        this->place.y += diff.y;
        
        sprite.setPosition(place);
        move_center(diff);
}

void DrawableCharacter::jump_to(sf::Vector2f diff)
{
        this->place.x = diff.x;
        this->place.y = diff.y;
        sprite.setPosition(place);
        update_center(sf::Vector2f(place.x + (texture->getSize().x / 2), place.y + (texture->getSize().y / 2)));
}

void DrawableCharacter::draw(sf::RenderWindow &window)
{
        sf::CircleShape shape(r);
        shape.setPosition(sf::Vector2f(center.x - r, center.y - r));
        shape.setFillColor(sf::Color(255, 0, 0));
        window.draw(shape);
        window.draw(sprite);
}

void DrawableCharacter::rotate(float a)
{
        MoveObject::rotate(a);
        
        sf::Vector2f relative_center = sf::Vector2f(
                ((texture->getSize().x / 2) * get_scale().x),
                ((texture->getSize().y / 2) * get_scale().y)
                );

        relative_center = geometry::rotate_point(get_angle(), relative_center);
        
        set_conflict_offset(relative_center);
        update_center(get_place());
}

void DrawableCharacter::call_rotate_func(u64 now, u64 begin)
{
        MoveObject::call_rotate_func(now, begin);
        
        sf::Vector2f relative_center = sf::Vector2f(
                ((texture->getSize().x / 2) * get_scale().x),
                ((texture->getSize().y / 2) * get_scale().y)
                );

        relative_center = geometry::rotate_point(get_angle(), relative_center);
        
        set_conflict_offset(relative_center);
        update_center(get_place());
}

void DrawableCharacter::rotate_with_func(u64 now)
{
        MoveObject::rotate_with_func(now);
}

void DrawableCharacter::change_textures(sf::Texture *t)
{
        this->texture = t;
        sprite.setTexture(*texture);
        sprite.setPosition(place);
}

PlayerCharacter::PlayerCharacter(CharacterAttribute attribute,
                                 sf::Texture *character, sf::Texture *core, sf::Vector2f p)
        : DrawableCharacter(attribute, character, p,
                            sf::Vector2f(
                                    TextureSize::PLAYER_CHARACTER_SIZE_X / character->getSize().x,
                                    TextureSize::PLAYER_CHARACTER_SIZE_Y / character->getSize().y),
                            mf::stop, rotate::stop, 0)
{
        this->core_texture = *core;
        core_sprite.setTexture(core_texture);
        set_core_place();
        core_sprite.setScale(0.11, 0.11);
        core_sprite.setColor(sf::Color(255, 230, 230));
        set_radius(6);
        core_off();
        this->enable_shot = true;
}

void PlayerCharacter::shot_on(void)
{
        this->enable_shot = true;
}

void PlayerCharacter::shot_off(void)
{
        this->enable_shot = false;
}

bool PlayerCharacter::shot_is_enable(void)
{
        return enable_shot;
}

void PlayerCharacter::core_on()
{ 
        enable_core = true;
}

void PlayerCharacter::core_off()
{
        enable_core = false;
}

void PlayerCharacter::move_diff(sf::Vector2f diff)
{
        if(center.x + diff.x < 32){
                return;
        }
        if(center.x + diff.x > 992){
                return;
        }
        if(center.y + diff.y < 32){
                return;
        }
        if(center.y + diff.y > 736){
                return;
        }

        this->place.x += diff.x;
        this->place.y += diff.y;
        
        sprite.setPosition(place);
        move_center(diff);
        set_core_place();
}

void PlayerCharacter::draw(sf::RenderWindow &window)
{
        DrawableCharacter::draw(window);
        if(enable_core){
                window.draw(core_sprite);
        }
}

void PlayerCharacter::set_core_place()
{
        auto scale = core_sprite.getScale();
        core_sprite.setPosition(
                center.x - ((core_sprite.getTextureRect().width * scale.x) / 2),
                center.y - ((core_sprite.getTextureRect().height * scale.y) / 2));
}

EnemyCharacterSchedule::EnemyCharacterSchedule(GameData *game_data, const char *path)
{
        std::ifstream ifs(path, std::ios::in);
                
        std::cout << "Loading Enemy Schedule listed in " << path << "..." << std::endl;

        if (ifs.fail()) {
                DEBUG_PRINT_HERE();
                std::cerr << "failed to read json file" << std::endl;
                exit(1);
        }

        const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();

        picojson::value v;
        const std::string err = picojson::parse(v, json);
        if (err.empty() == false) {
                DEBUG_PRINT_HERE();
                std::cerr << err << std::endl;
                exit(1);
        }
        
        picojson::object &obj = v.get<picojson::object>();
        picojson::array &array = obj["enemy_list"].get<picojson::array>();

        for(auto &array_element : array){
                picojson::object &elem = array_element.get<picojson::object>();
                picojson::object &point = elem["point"].get<picojson::object>();
                
                sf::Vector2f relative_point(point["x"].get<double>(), point["y"].get<double>());

                EnemyCharacterMaterial material(*game_data->get_enemy_material(elem["name"].get<std::string>()));
                material.time = elem["time"].get<double>();
                material.point += relative_point;
                data_list.emplace_back(material);
        }

        sort();
}

EnemyCharacterMaterial EnemyCharacterSchedule::get_front(void)
{
        return data_list.back();
}

void EnemyCharacterSchedule::pop_front(void)
{
        return data_list.pop_back();
}

size_t EnemyCharacterSchedule::size(void)
{
        return data_list.size();       
}

EnemyCharacterMaterial EnemyCharacterSchedule::at(int index)
{
        return data_list[index];
}

void EnemyCharacterSchedule::sort(void)
{
        std::sort(std::begin(data_list), std::end(data_list),
                  [](EnemyCharacterMaterial e1, EnemyCharacterMaterial e2){
                          return e1.time > e2.time;
                  });
}

EnemyCharacter::EnemyCharacter(CharacterAttribute attribute, sf::Texture *t,
			       sf::Vector2f p, sf::Vector2f scale, u64 begin_count,
                               std::function<sf::Vector2f(MoveObject *, u64, u64)> f,
                               std::function<float(Rotatable *, u64, u64)> r_fn,
                               float hp_max, float hp_init, bool damage_flag)
	: DrawableCharacter(attribute, t, p, scale, f, r_fn, begin_count)
{
        this->hp_actual = hp_init;
        this->hp_max = hp_max;
        set_radius(12);
        if(damage_flag)
                damage_on();
        else
                damage_off();
}

EnemyCharacter::EnemyCharacter(EnemyCharacterMaterial material, u64 time)
        : DrawableCharacter(CharacterAttribute(material.name), GameMaster::texture_table[material.txid],
                            material.point, material.scale, material.move_func, material.rot_func, time),
          shot_data(material.shot_data)
{
        this->hp_actual = material.init_hp;
        this->hp_max = material.max_hp;
        set_radius(material.radius);
        damage_on();
        for(FunctionCallEssential &f : shot_data){
                f.time += time;
        }
        std::sort(std::begin(shot_data), std::end(shot_data),
                  [](FunctionCallEssential &e1, FunctionCallEssential &e2){ return e1.time > e2.time; });
}

float EnemyCharacter::get_hp(void)
{
        return hp_actual;
}

void EnemyCharacter::set_hp(float val)
{
        this->hp_actual = val;
}

void EnemyCharacter::set_hp_max(void)
{
        this->hp_actual = this->hp_max;
}

void EnemyCharacter::damage(float value)
{
        if(damage_enable)
                this->hp_actual -= value;
}

bool EnemyCharacter::dead(void)
{
        return hp_actual <= 0;
}

void EnemyCharacter::move(u64 count)
{
	MoveObject::move(count);
	update_center(get_place());
}

void EnemyCharacter::damage_on(void)
{
        this->damage_enable = true;
}

void EnemyCharacter::damage_off(void)
{
        this->damage_enable = false;
}


std::optional<FunctionCallEssential> EnemyCharacter::shot(u64 now)
{
        if(shot_data.size() && shot_data.back().time == now){
                FunctionCallEssential e = shot_data.back();
                shot_data.pop_back();
                return e;
        }

        return std::nullopt;
}
