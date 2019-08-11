#include "laser.hpp"
#include "gm.hpp"

Laser::Laser(sf::Texture *head_tx, sf::Texture *body_tx, sf::Texture *tail_tx,
             sf::Vector2f begin, std::function<sf::Vector2f(MoveObject *, u64, u64)> fn,
             u64 begin_count, sf::Vector2f scale, float radius, u64 length)
{
        this->head_texture = head_tx;
        this->body_texture = body_tx;
        this->tail_texture = tail_tx;
        
        body.push_back(new Bullet(head_texture, begin, fn, begin_count, sf::Vector2f(0.3, 0.3), radius,
                                  true, true));

        this->move_func = fn;
        this->begin = begin;
        this->length = length;
        this->scale = scale;
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

        if(body.front()->visible())
                body.front()->draw(window);
        if(body.back()->visible())
                body.back()->draw(window);
}

void Laser::move(u64 count)
{
        for(auto it = std::begin(body);it != std::end(body);it++){
                Bullet *b = *it;
                sf::Vector2f &&before = b->get_place();
                b->move(count);
                sf::Vector2f diff = b->get_place() - before;
                b->rotate(std::atan(-diff.x / diff.y));
        }

        if(length > body.size()){
                body.push_back(new Bullet(body_texture, begin, move_func, count,
                                          sf::Vector2f(0.3, 0.3), 7, true, true));
        }else if(length == body.size()){
                body.push_back(new Bullet(tail_texture, begin, move_func, count,
                                          sf::Vector2f(0.3, 0.3), 7, true, true));
        }
}

std::list<Bullet *> Laser::get_bullet_stream()
{
        return body;
}
