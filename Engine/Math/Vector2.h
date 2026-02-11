#pragma once

#include <cmath>

class Vector2 {
public:
    float x, y;

    Vector2();
    Vector2(float x, float y);

    // Operators
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);

    bool operator==(const Vector2& other) const;

    // Utility functions
    float magnitude() const;
    float magnitudeSquared() const;
    Vector2 normalized() const;
    float distance(const Vector2& other) const;
    float dot(const Vector2& other) const;

    // Static functions
    static Vector2 zero();
    static Vector2 one();
    static Vector2 up();
    static Vector2 down();
    static Vector2 left();
    static Vector2 right();

    // For grid-based pathfinding (Manhattan distance)
    int manhattanDistance(const Vector2& other) const;
};
