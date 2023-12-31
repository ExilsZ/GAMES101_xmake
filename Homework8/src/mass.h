#pragma once

#include "CGL/CGL.h"
#include "CGL/vector2D.h"

using namespace CGL;

struct Mass {
    Mass(Vector2D position, float mass, bool pinned)
        : start_position(position), position(position), last_position(position), mass(mass),
          pinned(pinned) {}

    float mass;
    bool  pinned;

    Vector2D start_position;
    Vector2D position; // x(t-1)

    // explicit Verlet integration

    Vector2D last_position; // x(t-1)

    // explicit Euler integration

    Vector2D velocity;
    Vector2D forces;
};
