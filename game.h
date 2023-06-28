#include "raylib.h"

typedef struct {
    Vector2 pose;
    Vector2 vel;
    float angle; // in radians
} Boid;