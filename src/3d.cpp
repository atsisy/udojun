#include "3d.hpp"
#include "move_func.hpp"
#include "rotate_func.hpp"
#include "geometry.hpp"

DrawableObject3D::DrawableObject3D(sf::Texture *t, sf::Vector3f p,
                                   std::function<sf::Vector3f(DrawableObject3D *, u64, u64)> f,
                                   std::function<float(Rotatable *, u64, u64)> r_fn,
                                   u64 begin_count)
        : MoveObject(t, sf::Vector2f(0, 0), mf::stop, rotate::stop, begin_count),
          actual_3d_position(p)
{
        move_func_3d = f;
        set_place(geometry::vector_translate(actual_3d_position, 50));
}

void DrawableObject3D::draw(sf::RenderWindow &window)
{
        MoveObject::draw(window);
}

void DrawableObject3D::move(u64 count)
{
        actual_3d_position = move_func_3d(this, count, this->begin_count);
        sf::Vector2f translated = geometry::vector_translate(actual_3d_position, 5);
        translated.x += 400;
        set_place(translated);
        float scale = 5 * (5 / actual_3d_position.z);
        this->set_scale(scale, scale);
}

sf::Vector3f DrawableObject3D::get_3d_position(void)
{
        return actual_3d_position;
}
