#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include "boid.c"

const int SCREEN_WIDTH = 480 * 2;
const int SCREEN_HEIGHT = 360 * 2;
const double SPRITE_SIZE = 8;
const int OBSTACLE_AMOUNT = 0;

const int AMOUNT = 1600;
const double FLOCK_RADIUS = 60;
const double BOID_SPEED = .15;

const double SEPERATION_STRENGTH    = 2.02;
const double ALIGNMENT_STRENGTH     = 1;
const double COHESION_STRENGTH      = 2;
const double AVOID                  = 18;

const double TURN_RESISTANCE        = 60;

void Tick(struct Boid *boids, struct Vec2 *obstacles, double delta);
void RefreshWindow(SDL_Renderer *renderer, const struct Boid *boids, const struct Vec2 *obstacles);
struct Boid* CreateBoids();
struct Vec2* CreateObstacles(int amount);

int main(void)
{
    printf("initializing boids\n");
    struct Boid *boids = CreateBoids();
    printf("boids initialized\n");

    struct Vec2 *obstacles = CreateObstacles(OBSTACLE_AMOUNT);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    printf( "SDL initialized\n" );

    if(SDL_CreateWindowAndRenderer( SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer ) < 0)
    {
        printf( "Window/Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    printf( "Window & renderer created\n" );

    SDL_Event e;
    int running = 0;
    int paused = -1;
    double timeOfLastFrame = 0;
    while(running == 0){

        if(paused == -1)
        {
            RefreshWindow(renderer, boids, obstacles);
            double deltaTime = (SDL_GetTicks() - timeOfLastFrame);
            timeOfLastFrame = SDL_GetTicks();
            Tick(boids, obstacles, deltaTime);
            SDL_Delay(1);
        }

        while( SDL_PollEvent(&e) > 0 )
        {

           //dont work
            if(e.type == SDLK_SPACE)
            {
                printf("Pause registered. pause/unpausing");
                paused = -paused;
            }
            if( e.type == SDL_QUIT )
            {
                printf("QUIT registered. exiting now\n");
                running = 1;
            }
        }
    }

    free(boids);
    free(obstacles);

    return 0;
}

struct Boid* CreateBoids()
{
    struct Boid *boids = malloc(sizeof(struct Boid) * AMOUNT);
    if(boids == NULL)
        printf("ERROR: Failed to allocate memory for boids");

    srand(653);
    printf("USING SEED: %i\n", 2);

    for(int i = 0; i < AMOUNT; i++)
    {
        boids[i].pos.x = rand() % SCREEN_WIDTH;
        boids[i].pos.y = rand() % SCREEN_HEIGHT;

        boids[i].dir.x = -1 + ((float)rand() / (float)RAND_MAX) * 2;
        boids[i].dir.y = -1 + ((float)rand() / (float)RAND_MAX) * 2;

        //boids[i].depth = 0.5 + (float)rand() / (float)RAND_MAX;
        boids[i].depth = 0.5 + (double)(i + 1) / (double)(AMOUNT);
    }

    return boids;
}

struct Vec2* CreateObstacles(int amount)
{
    struct Vec2* obstacles = malloc(amount * sizeof(struct Vec2));

    for(int i = 0; i < amount; i++)
    {
        if(i % 2 == 0)
            obstacles[i].x = 2.0;
        else
            obstacles[i].x = SCREEN_WIDTH - 2.0;

        obstacles[i].y = (i) * SCREEN_HEIGHT / (double)(amount-1);
    }

    return obstacles;
}

void RefreshWindow(SDL_Renderer *renderer, const struct Boid *boids, const struct Vec2 *obstacles)
{
    SDL_SetRenderDrawColor(renderer, 26, 26, 26, 255);
    SDL_RenderClear(renderer);

    for(int i = 0; i < AMOUNT; i++)
    {
        const double p = boids[i].depth * 70;
        //SDL_SetRenderDrawColor(renderer, 130 + p, 186, 38, 25);
        SDL_SetRenderDrawColor(renderer,  p+20, p+50, p, 255);

        const double BOID_SIZE = (SPRITE_SIZE * boids[i].depth + BOID_SIZE) / 2.0;

        SDL_Rect rect;
        rect.x = boids[i].pos.x - BOID_SIZE / 2.0;
        rect.y = /*SCREEN_HEIGHT - */(boids[i].pos.y - BOID_SIZE / 2.0);
        rect.h = BOID_SIZE;
        rect.w = BOID_SIZE;

        SDL_RenderFillRect(renderer, &rect);

        const double FACE_SIZE = BOID_SIZE / 2.0;
        const double FACE_DISTANCE = 6;
        SDL_Rect face;
        face.x = boids[i].pos.x + FACE_DISTANCE * boids[i].dir.x - FACE_SIZE / 2.0;
        face.y = /*SCREEN_HEIGHT - */(boids[i].pos.y + FACE_DISTANCE * boids[i].dir.y - FACE_SIZE / 2.0);
        face.h = FACE_SIZE;
        face.w = FACE_SIZE;

        SDL_RenderFillRect(renderer, &face);
    }

    SDL_SetRenderDrawColor(renderer, 200, 86, 38, 255);
    for(int i = 0; i < OBSTACLE_AMOUNT; i++)
    {
        const double FACE_SIZE = SPRITE_SIZE * 2.0;
        SDL_Rect rect;
        rect.x = obstacles[i].x - FACE_SIZE / 2.0;
        rect.y = /*SCREEN_HEIGHT - */(obstacles[i].y - FACE_SIZE / 2.0);
        rect.h = FACE_SIZE;
        rect.w = FACE_SIZE;

        SDL_RenderFillRect(renderer, &rect);
    }


    SDL_RenderPresent(renderer);
}

double Loop(double x, const double min, const double max)
{
    // shit gross
    double y = x;
    const double range = max - min;

    if(y > max)
    {
        while (y>max){
            y -= range;
        }
    }

    if(y < min)
    {
        while ( y < min )
        {
            y += range;
        }
    }

    return y;
}

double Lerp(const double a, const double b, const double i)
{
    return a * (1 - i) + b * i;
}

void GetLocalBoids(struct Boid *boids, const int index, struct Flock *flock)
{
    struct Boid *locals[AMOUNT];
    int j = 0;

    for (int i = 0; i < AMOUNT; i++)
    {
        if(i == index || fabs(boids[i].depth - boids[index].depth) >= .2)
            continue;
        
        struct Vec2 distance;
        distance.x = boids[i].pos.x - boids[index].pos.x;
        distance.y = boids[i].pos.y - boids[index].pos.y;

        double distanceSqr = LengthSquared(&distance);


        if(distanceSqr < FLOCK_RADIUS * FLOCK_RADIUS)
        {
            Normalize(&distance);
            Normalize(&boids[i].dir);

            if( Dot( &boids[i].dir, &distance ) > -.5 )
            {
                locals[j] = &boids[i];
                j++;
            }
        }
    }
    
    flock->boids = malloc(j * sizeof(struct Boid**));

    if(flock->boids == NULL)
        printf("failed to allocate memory for local flock processing.");

    for(int i = 0; i < j; i++)
    {
        flock->boids[i] = locals[i];
    }

    flock->size= j;
}

void GetNearbyObstacles(struct Vec2 *obstacles, const struct Vec2 pos, struct Vec2 **nearby, int *count)
{
    struct Vec2 *temp = malloc(sizeof(struct Vec2) * OBSTACLE_AMOUNT);
    int j = 0;
    for (int i = 0; i< OBSTACLE_AMOUNT; i++)
    {
        struct Vec2 obsDis;
        VecOperate(&obsDis, &obstacles[i], VEC_CPY);
        VecOperate(&obsDis, &pos, VEC_SUB);

        if(LengthSquared(&obsDis) < FLOCK_RADIUS * FLOCK_RADIUS)
        {
            temp[j] = obstacles[i];
            j++;
        }
    }

    *nearby = malloc(sizeof(struct Vec2*) * j);
    for(int i = 0; i < j; i++)
    {
        *nearby[i] = temp[i];
    }
    *count = j;
    free(temp);
}

void Tick(struct Boid *boids, struct Vec2 *obstacles, double delta)
{
    for(int i = 0; i < AMOUNT; i++)
    {
        struct Boid *boid = &(boids[i]);
        struct Vec2 steerDir;
        struct Flock flock;
        GetLocalBoids(boids, i, &flock);

        if(flock.size > 0)
        {
            // average pos of all flockmates
            struct Vec2 cohesionDir;
                AvgFlockPos(flock, &cohesionDir);
                VecOperate(&cohesionDir, &boid->pos, VEC_SUB);
                SetLength(&cohesionDir, COHESION_STRENGTH);

            //average directeion from current to flockmate
            struct Vec2 seperationDir;
                AvgFlockDir(flock, boid, &seperationDir);
                Invert(&seperationDir);
                SetLength(&seperationDir, SEPERATION_STRENGTH);

            // avergae angle of all flockmates
            struct Vec2 alignmentDir;
                AvgFlockAngle(flock, &alignmentDir);
                SetLength(&alignmentDir, ALIGNMENT_STRENGTH);

            struct Vec2 newDir = { .x = 0, .y = 0 };
                VecOperate(&newDir, &cohesionDir, VEC_ADD);
                VecOperate(&newDir, &seperationDir, VEC_ADD);
                VecOperate(&newDir, &alignmentDir, VEC_ADD);
                Normalize(&newDir);

            if(!isnan(newDir.x) && !isnan(newDir.y))
                steerDir = newDir;
        }

        struct Vec2* nearbyObstacles;
        int obstaclesCount = 0;
        GetNearbyObstacles(obstacles, boid->pos, &nearbyObstacles, &obstaclesCount);

        if(obstaclesCount > 0)
        {
            struct Vec2 dirAway = { .x=0, .y=0 };

            for(int i = 0; i < obstaclesCount; i++)
            {
                dirAway.x += boid->pos.x - nearbyObstacles[i].x;
                dirAway.y += boid->pos.y - nearbyObstacles[i].y;
            }

            double a = AVOID / Length(&steerDir);
            steerDir.x = (a * steerDir.x + dirAway.x) / (a + 1);
            steerDir.y = (a * steerDir.y + dirAway.y) / (a + 1);
        }

        free(flock.boids);
        free(nearbyObstacles);

        boid->dir.x = Lerp(boid->dir.x, steerDir.x, fmin(delta / TURN_RESISTANCE,1));
        boid->dir.y = Lerp(boid->dir.y, steerDir.y, fmin(delta / TURN_RESISTANCE,1));

        SetLength(&boid->dir, BOID_SPEED * boid->depth * delta);
        TranslateVector(&boid->pos, &boid->dir);
        boid->pos.x = Loop(boid->pos.x, 0, SCREEN_WIDTH );
        boid->pos.y = Loop(boid->pos.y, 0, SCREEN_HEIGHT);
        Normalize(&boid->dir);
    }
}

