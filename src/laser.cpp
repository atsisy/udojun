#include "laser.hpp"
#include "gm.hpp"
#include "geometry.hpp"

Laser::Laser(TextureID head_tx, TextureID body_tx, TextureID tail_tx,
             sf::Vector2f begin, std::function<sf::Vector2f(MoveObject *, u64, u64)> fn,
             u64 begin_count, sf::Vector2f scale, float radius, u64 length)
{
        this->head_texture = GameMaster::texture_table[head_tx];
        this->body_texture = GameMaster::texture_table[body_tx];
        this->tail_texture = GameMaster::texture_table[tail_tx];

        this->move_func = fn;
        this->begin = begin;
        this->length = length;
        this->scale = scale;
        this->radius = radius;
}

Laser::~Laser()
{
        for(Bullet *b : body){
                delete b;
        }
}

void Laser::draw(sf::RenderWindow &window)
{
        for(Bullet *b : body){
                if(b->visible())
                        b->draw(window);
        }

        body.back()->appear();
}

void Laser::move(u64 count)
{
        float last_rotation;
        
        for(auto it = std::begin(body);it != std::end(body);it++){
                Bullet *b = *it;
                sf::Vector2f &&before = b->get_place();
                b->move(count);
                sf::Vector2f diff = b->get_place() - before;
                last_rotation = -diff.x / diff.y;
                b->rotate(std::atan(last_rotation));
        }

        if(body.size() == 0){
                body.push_back(new Bullet(head_texture, begin, move_func, count,
                                          sf::Vector2f(0.3, 0.3), radius, true, true));
                body.back()->hide();
        }else if(length > body.size()){
                body.push_back(new Bullet(body_texture, begin, move_func, count,
                                          sf::Vector2f(0.3, 0.3), radius, true, true));
                body.back()->hide();
        }else if(length == body.size()){
                body.push_back(new Bullet(tail_texture, begin, move_func, count,
                                          sf::Vector2f(0.3, 0.3), radius, true, true));
                body.back()->hide();
        }
}

std::list<Bullet *> Laser::get_bullet_stream()
{
        return body;
}

StraightLaser::StraightLaser(sf::Texture *t, sf::Vector2f begin, sf::Vector2f end,
                             std::function<sf::Vector2f(MoveObject *, u64, u64)> fn, u64 begin_count)
{
        texture = new sf::Texture(*t);

        this->scale_func = fn;
        this->begin = begin;
        this->end = end;
        
        const float distance = util::distance<float>(begin.x, begin.y, end.x, end.y);
        const int count = distance / texture->getSize().y;

        sf::Vector2f diff = end - begin;
        float angle = geometry::calc_angle(begin, end);

        if (diff.y < 0) {
                angle -= M_PI_2;
        }else if(diff.x < 0){
                angle -= M_PI_2;
        }

        sf::Vector2f point = begin;
        for(int i = 0;i < count;i++){
                 Bullet *b = new Bullet(this->texture, point, scale_func, begin_count,
                                       sf::Vector2f(0.3, 1.0), 7, true, true);
                 
                 b->rotate(angle);
                 
                 laser.push_back(b);
                 point += sf::Vector2f(
                         texture->getSize().y * std::sin(angle),
                         -(texture->getSize().y * std::cos(angle))
                         );
        }
}

void StraightLaser::draw(sf::RenderWindow &window)
{
        for(Bullet *b : laser){
                if(b->visible())
                        b->draw(window);
        }
}

void StraightLaser::update_scale(u64 count)
{
        for(Bullet *b : laser){
                b->move(count);
        }
}

std::list<Bullet *> StraightLaser::get_bullet_stream(void)
{
        return laser;
}

