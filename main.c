#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const int NUM_BOIDS = 100;
const int BOID_RADIUS = 10;
const int SEPARATION_RANGE = 20;
const int AVOID_FACTOR = 1;
const int TRIANGLE_RADIUS = 20;

void update_boid_pose(Boid *b)
{
    b->pose.x += b->vel.x;
    b->pose.y += b->vel.y;
}

void draw_rotated_triangle(Vector2 origin, float angle)
{
    //  c
    // a b
    // cos(30) = sqrt(3)/2
    // sin(30) = 1/2
    Vector2 a = {origin.x - TRIANGLE_RADIUS * sqrt(3) / 2, origin.y + TRIANGLE_RADIUS * 0.5};
    Vector2 b = {origin.x + (TRIANGLE_RADIUS) * sqrt(3) / 2, origin.y + (TRIANGLE_RADIUS) * 0.5};
    Vector2 c = {origin.x, origin.y - TRIANGLE_RADIUS};
    // subtract point of rotation from each triangle vertex, apply rotation, and then add point back
    DrawTriangle(Vector2Add(Vector2Rotate(Vector2Subtract(a, origin), angle), origin),
                 Vector2Add(Vector2Rotate(Vector2Subtract(b, origin), angle), origin),
                 Vector2Add(Vector2Rotate(Vector2Subtract(c, origin), angle), origin), PURPLE);

    // DrawCircleV(Vector2Add(Vector2Rotate(Vector2Subtract(c, origin), angle), origin), 5, BLACK);
}

int main(void)
{
    printf("start\n");
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "boids");

    // Rendering flags and fps
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);

    // Initialize boids
    Boid boids[NUM_BOIDS];
    memset(boids, 0, sizeof(Boid) * NUM_BOIDS); // set all values in boid to 0
    for (int i = 0; i < NUM_BOIDS; i++)
    {
        Boid *b = &boids[i];
        b->pose.x = GetRandomValue(BOID_RADIUS, SCREEN_WIDTH - BOID_RADIUS);
        b->pose.y = GetRandomValue(BOID_RADIUS, SCREEN_HEIGHT - BOID_RADIUS);
    }

    int dX = 0;
    int dY = 0;

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(RAYWHITE);
        dX = 0;
        dY = 0;

        for (int i = 0; i < NUM_BOIDS; i++)
        {
            for (int j = 1; j < NUM_BOIDS; j++)
            {
                if (Vector2Distance(boids[i].pose, boids[j].pose) <= SEPARATION_RANGE)
                {
                    printf("distance: %f\n", Vector2Distance(boids[i].pose, boids[j].pose));
                    dX += boids[i].pose.x - boids[j].pose.x;
                    printf("%f\n", boids[i].pose.x - boids[j].pose.x);
                    dY += boids[i].pose.y - boids[j].pose.y;
                }
            }
            boids[i].vel.x += dX * AVOID_FACTOR;
            boids[i].vel.y += dY * AVOID_FACTOR;
            update_boid_pose(&boids[i]);

            // DrawCircleV(boids[i].pose, BOID_RADIUS, DARKGRAY);
            
            // TODO fix triangle rotation to match direction of movement
            draw_rotated_triangle(boids[i].pose, atan2(boids[i].vel.y, boids[i].vel.x));
        }

        EndDrawing();
        // usleep(500000);
    }

    CloseWindow();

    return 0;
}