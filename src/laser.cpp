#include "laser.hpp"
#include "gm.hpp"

Laser::Laser(sf::Vector2f begin, float length, float rad, float speed, u64 count)
{
        float angle_rad = rad;
        float angle_kodo = -(angle_rad * 180 / M_PI) - 90;

        auto fn = mf::linear(std::tan(angle_rad), speed, 0);
        
        head = new Bullet(GameMaster::texture_table[LASER_HEAD1], begin,
                          fn, count, sf::Vector2f(0.15, 1.0), 12, true, true);
        head->set_color(sf::Color(0xa2, 0x93, 0xbd));
        head->rotate(angle_kodo);

        u32 body_count = (length - (GameMaster::texture_table[LASER_HEAD1]->getSize().y * 2)) / (float)GameMaster::texture_table[LASER_BODY1]->getSize().y;
        float dx = (float)GameMaster::texture_table[LASER_BODY1]->getSize().y * std::cos(angle_rad);
        float x_pp = dx * (body_count);

        do{
		body.push_back(new Bullet(
			GameMaster::texture_table[LASER_BODY1],
			sf::Vector2f(begin.x + x_pp +
					     (head->displaying_size().y *
					      std::cos(angle_rad)),
				     (begin.y) -
					     (head->displaying_size().y *
					      std::sin(angle_rad)) -
					     ((body_count) *
					      ((float)GameMaster::texture_table
						       [LASER_BODY1]
							       ->getSize()
							       .y *
					       std::sin(angle_rad)))),
			fn, count, sf::Vector2f(0.15, 1.0), 12, true, true));
		x_pp -= dx;
                body.back()->set_color(sf::Color(0xa2, 0x93, 0xbd));
                body.back()->rotate(angle_kodo);
        }while(body_count--);

	tail = new Bullet(
		GameMaster::texture_table[LASER_TAIL1],
		sf::Vector2f((begin.x) + (length * std::cos(angle_rad)) -
				     (GameMaster::texture_table[LASER_TAIL1]
					      ->getSize()
					      .y *
				      std::cos(angle_rad)),
			     (begin.y) - (length * std::sin(angle_rad)) +
				     (GameMaster::texture_table[LASER_TAIL1]
					      ->getSize()
					      .y *
				      std::sin(angle_rad))),
		fn, count, sf::Vector2f(0.15, 1.0), 12, true, true);
        tail->set_color(sf::Color(0xa2, 0x93, 0xbd));
        tail->rotate(angle_kodo);
}

Laser::~Laser()
{
        delete head;
        for(Bullet *b : body){
                delete b;
        }
        delete tail;
}

void Laser::draw(sf::RenderWindow &window)
{
        head->draw(window);
        for(Bullet *b : body){
                b->draw(window);
        }
        tail->draw(window);
}

void Laser::move(u64 count)
{       head->move(count);
        for(Bullet *b : body){
                b->move(count);
        }
        tail->move(count);
}

std::vector<Bullet *> Laser::get_bullet_stream()
{
        std::vector<Bullet *> ret;
        ret.push_back(head);
        std::copy(std::begin(body), std::end(body), std::back_inserter(ret));
        ret.push_back(tail);
        return ret;
}
