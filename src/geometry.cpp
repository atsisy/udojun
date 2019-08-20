#include "geometry.hpp"
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

namespace geometry {

        sf::Vector2f spline_curve(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f v1, sf::Vector2f v2, float t)
        {
                matrix<double> T(4, 4);
                matrix<double> H(4, 4);
                matrix<double> G(4, 4);
        
                T(0, 0) = t*t*t; T(0,1) = t*t; T(0,2) = t; T(0,3) = 1;

                H(0,0) = 2; H(0,1) = -2; H(0,2) = 1; H(0,3) = 1;
                H(1,0) =-3; H(1,1) =  3; H(1,2) =-2; H(1,3) =-1;
                H(2,0) = 0; H(2,1) =  0; H(2,2) = 1; H(2,3) = 0;
                H(3,0) = 1; H(3,1) =  0; H(3,2) = 0; H(3,3) = 0;

                G(0,0) = p1.x; G(0,1) = p1.y; G(0,2) = 0; G(0,3) = 1;
                G(1,0) = p2.x; G(1,1) = p2.y; G(1,2) = 0; G(1,3) = 1;
                G(2,0) = v1.x; G(2,1) = v1.y; G(2,2) = 0; G(2,3) = 1;
                G(3,0) = v2.x; G(3,1) = v2.y; G(3,2) = 0; G(3,3) = 1;

                matrix<double> ret1 = prod(T, H);
                matrix<double> ret_mat = prod(ret1, G);
                return sf::Vector2f(ret_mat(0,0), ret_mat(0,1));
        }

        float calc_angle(sf::Vector2f a, sf::Vector2f b)
        {
                sf::Vector2f diff = a - b;
                float angle = std::atan(diff.y / diff.x);
                
                if(diff.x < 0){
                        if(diff.y >= 0){
                                angle += M_PI;
                        }else{
                                angle += (M_PI / 2);
                        }
                }

                return angle;
        }

        float convert_to_radian(float angle)
        {
                return angle / 180.0 * M_PI;
        }

        sf::Vector2f vector_translate(sf::Vector3f position, float distance)
        {
                float x = distance * position.x / position.z;
                float y = distance * position.y / position.z;

                return sf::Vector2f(x, y);
        }


}
