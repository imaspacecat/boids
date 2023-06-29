#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1200;
const int NUM_BOIDS = 400;
const int BOID_RADIUS = 10;
const int TRIANGLE_RADIUS = 5;

const int CLOSE_RANGE = 8;
const int FAR_RANGE = 40;
const float SEPARATION_FACTOR = 0.0215;
const float ALIGN_FACTOR = 0.0262;
const float COHESION_FACTOR = 0.0011;

const float MAX_SPEED = 10;
const float MIN_SPEED = 5;

const int LEFT_MARGIN = 100;
const int RIGHT_MARGIN = SCREEN_WIDTH - 100;
const int BOTTOM_MARGIN = SCREEN_HEIGHT - 100;
const int TOP_MARGIN = 100;
const float TURN_FACTOR = 1;

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
    Vector2 a = {origin.x - TRIANGLE_RADIUS * 0.5, origin.y - TRIANGLE_RADIUS * sqrt(3) / 2};
    Vector2 b = {origin.x - (TRIANGLE_RADIUS)*0.5, origin.y + (TRIANGLE_RADIUS)*sqrt(3) / 2};
    Vector2 c = {origin.x + TRIANGLE_RADIUS, origin.y};
    // subtract point of rotation from each triangle vertex, apply rotation, and then add point back
    DrawTriangle(Vector2Add(Vector2Rotate(Vector2Subtract(a, origin), angle), origin),
                 Vector2Add(Vector2Rotate(Vector2Subtract(b, origin), angle), origin),
                 Vector2Add(Vector2Rotate(Vector2Subtract(c, origin), angle), origin), PURPLE);

    DrawCircleV(Vector2Add(Vector2Rotate(Vector2Subtract(c, origin), angle), origin), 2, BLACK);
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

    int dX = 0, dY = 0;
    int xVelAvg = 0, yVelAvg = 0, xPoseAvg = 0, yPoseAvg = 0;
    int neighborBoidCount = 0;
    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(RAYWHITE);
        dX = dY = 0;
        xVelAvg = yVelAvg = xPoseAvg = yPoseAvg = 0;
        neighborBoidCount = 0;

        for (int i = 0; i < NUM_BOIDS; i++)
        {
            for (int j = 0; j < NUM_BOIDS; j++)
            {
                if (Vector2Distance(boids[i].pose, boids[j].pose) <= CLOSE_RANGE)
                {
                    dX += boids[i].pose.x - boids[j].pose.x;
                    dY += boids[i].pose.y - boids[j].pose.y;

                    xPoseAvg += boids[j].pose.x;
                    yPoseAvg += boids[j].pose.y;

                    xVelAvg += boids[j].vel.x;
                    yVelAvg += boids[j].vel.y;
                    neighborBoidCount++;
                }

                if (Vector2Distance(boids[i].pose, boids[j].pose) <= FAR_RANGE)
                {
                    xPoseAvg += boids[j].pose.x;
                    yPoseAvg += boids[j].pose.y;

                    xVelAvg += boids[j].vel.x;
                    yVelAvg += boids[j].vel.y;
                    neighborBoidCount++;
                }
                // teleport to other end of screen
                if (boids[i].pose.x < 0)
                    boids[i].pose.x = SCREEN_WIDTH;
                if (boids[i].pose.x > SCREEN_WIDTH)
                    boids[i].pose.x = 0;
                if (boids[i].pose.y < 0)
                    boids[i].pose.y = SCREEN_HEIGHT;
                if (boids[i].pose.y > SCREEN_HEIGHT)
                    boids[i].pose.y = 0;

                // avoid screen edges
                // if (boids[i].pose.x < LEFT_MARGIN)
                //     boids[i].vel.x += TURN_FACTOR;
                // if (boids[i].pose.x > RIGHT_MARGIN)
                //     boids[i].vel.x -= TURN_FACTOR;
                // if (boids[i].pose.y > BOTTOM_MARGIN)
                //     boids[i].vel.y -= TURN_FACTOR;
                // if (boids[i].pose.y < TOP_MARGIN)
                //     boids[i].vel.y += TURN_FACTOR;
            }

            boids[i].vel.x += dX * SEPARATION_FACTOR;
            boids[i].vel.y += dY * SEPARATION_FACTOR;

            if (neighborBoidCount > 0)
            {
                xVelAvg = xVelAvg / neighborBoidCount;
                yVelAvg = yVelAvg / neighborBoidCount;
                xPoseAvg /= neighborBoidCount;
                yPoseAvg /= neighborBoidCount;
            }

            boids[i].vel.x += (xVelAvg - boids[i].vel.x) * ALIGN_FACTOR + (xPoseAvg - boids[i].pose.x) * COHESION_FACTOR;
            boids[i].vel.y += (yVelAvg - boids[i].vel.y) * ALIGN_FACTOR + (yPoseAvg - boids[i].pose.y) * COHESION_FACTOR;
            printf("vx: %f, vy: %f\n", boids[i].vel.x, boids[i].vel.y);

            float speed = sqrt(boids[i].vel.x * boids[i].vel.x + boids[i].vel.y * boids[i].vel.y);
            // printf("speed: %f\n", speed);
            if (speed > MAX_SPEED)
            {
                boids[i].vel.x = (boids[i].vel.x / speed) * MAX_SPEED;
                boids[i].vel.y = (boids[i].vel.y / speed) * MAX_SPEED;
            }
            if (speed < MAX_SPEED)
            {
                boids[i].vel.x = (boids[i].vel.x / speed) * MIN_SPEED;
                boids[i].vel.y = (boids[i].vel.y / speed) * MIN_SPEED;
            }

            update_boid_pose(&boids[i]);

            draw_rotated_triangle(boids[i].pose, atan2(boids[i].vel.y, boids[i].vel.x));
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}