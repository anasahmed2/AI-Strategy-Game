#include "Vector2.h"

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

bool Vector2::operator==(const Vector2& other) const {
    return x == other.x && y == other.y;
}

float Vector2::magnitude() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::magnitudeSquared() const {
    return x * x + y * y;
}

Vector2 Vector2::normalized() const {
    float mag = magnitude();
    if (mag == 0.0f) return Vector2::zero();
    return *this / mag;
}

float Vector2::distance(const Vector2& other) const {
    return (*this - other).magnitude();
}

float Vector2::dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

Vector2 Vector2::zero() {
    return Vector2(0.0f, 0.0f);
}

Vector2 Vector2::one() {
    return Vector2(1.0f, 1.0f);
}

Vector2 Vector2::up() {
    return Vector2(0.0f, -1.0f);
}

Vector2 Vector2::down() {
    return Vector2(0.0f, 1.0f);
}

Vector2 Vector2::left() {
    return Vector2(-1.0f, 0.0f);
}

Vector2 Vector2::right() {
    return Vector2(1.0f, 0.0f);
}

int Vector2::manhattanDistance(const Vector2& other) const {
    return static_cast<int>(std::abs(x - other.x) + std::abs(y - other.y));
}
