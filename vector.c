#include <math.h>

struct Vec2
{
    double x;
    double y;
};

//const struct Vec2 up = {};
//const struct Vec2 left = {};

/*
void TranslateVector(struct Vec2 *vec, const double angle, const double amount)
{
        vec->x += cos(angle) * amount;
        vec->y += sin(angle) * amount;
}
*/

void TranslateVector(struct Vec2 *vec, const struct Vec2 *amount)
{
    vec->x += amount->x;
    vec->y += amount->y;
}

enum VEC_OPERATIONS {
    VEC_ADD,
    VEC_SUB,
    VEC_CPY,
    VEC_CROSS,
};

void VecOperate(struct Vec2 *vec, const struct Vec2 *amount, int flag)
{
    switch (flag)
    {
        case VEC_ADD:
            vec->x += amount->x;
            vec->y += amount->y;
            break;

        case VEC_SUB:
            vec->x -= amount->x;
            vec->y -= amount->y;
            break;

        case VEC_CPY:
            vec->x = amount->x;
            vec->y = amount->y;
            break;
        case VEC_CROSS:
            break;
    }
}

double Dot( const struct Vec2 *a, const struct Vec2 *b)
{
    return a->x * b->x + a->y * b->y;
}

double LengthSquared(const struct Vec2 *vec)
{
    return vec->x * vec->x + vec->y * vec->y;
}

double Length(const struct Vec2 *vec)
{
    return sqrt(LengthSquared(vec));
}

void Scale(struct Vec2 *vec, double scalar)
{
    vec->x *= scalar;
    vec->y *= scalar;
}

void SetLength(struct Vec2 *vec, double length)
{
    Scale(vec, length / Length(vec));
}

void Normalize(struct Vec2 *vec)
{
    double length = Length(vec);
    Scale(vec, 1 / length);
}

void Invert(struct Vec2 *vec)
{
    vec->x = -(vec->x);
    vec->y = -(vec->y);
}

double AngleOf(const struct Vec2 *vec)
{
    return atan2(vec->y,vec->x);
}

