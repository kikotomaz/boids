#include "vector.c"

struct Boid
{
    struct Vec2 pos;
    struct Vec2 dir;
    double depth;
};

struct Flock
{
    struct Boid **boids;
    int size;
};

void AvgFlockDir(struct Flock flock, struct Boid *boid, struct Vec2 *out)
{
    out->x= 0;
    out->y= 0;

    for(int i = 0; i < flock.size; i++)
    {
        struct Vec2 dir = { .x = 0, .y = 0 };
        dir.x += flock.boids[i]->pos.x - boid->pos.x;
        dir.y += flock.boids[i]->pos.y - boid->pos.y;

        Normalize(&dir);
        VecOperate(out, &dir, VEC_ADD);
    }
    
    out->x /= flock.size;
    out->y /= flock.size;
}

void AvgFlockPos(struct Flock flock, struct Vec2 *out)
{
    out->x= 0;
    out->y= 0;

    for(int i = 0; i < flock.size; i++)
    {
        out->x += flock.boids[i]->pos.x;
        out->y += flock.boids[i]->pos.y;
    }
    
    out->x /= flock.size;
    out->y /= flock.size;
}

void AvgFlockAngle(struct Flock flock, struct Vec2 *out)
{
    out->x = 0;
    out->y = 0;

    for (int i=0; i < flock.size; i++)
    {
        out->x += flock.boids[i]->dir.x;
        out->y += flock.boids[i]->dir.y;
    }

    out->x /= flock.size;
    out->y /= flock.size;

}

