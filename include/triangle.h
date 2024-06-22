#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <math.h>

#define square_root_of_3_4ths 0.8660254037844386467637231707529361834714026269051903140279034897
#define sine_2pi_over_6 square_root_of_3_4ths
#define cosine_2pi_over_6 0.5
#define equilateral_y sine_2pi_over_6
#define equilateral_x cosine_2pi_over_6
// circumscribed radius = equilateral_y / (1 + 1/sin(pi/6)) => radius = equilateral_y/3
#define equilateral_circumscribe_radius (equilateral_y / 3)

typedef double Distance;

// 0 angle = +x axis (y = 0)
// pi/2    = +y axis (x = 0)
typedef double Angle;
constexpr Angle pi = M_PI;

typedef struct {
    Distance x;
    Distance y;
} Point;

typedef Point Vector2;

typedef struct {
    Point point[3];
} Triangle;

bool is_point_within_equilateral_triangle(Point point, Triangle triangle);

#endif /* TRIANGLE_HPP */
