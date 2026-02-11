#pragma once

#include <cmath>

class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);

    // Operators
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    bool operator==(const Vector3& other) const;

    // Utility functions
    float magnitude() const;
    float magnitudeSquared() const;
    Vector3 normalized() const;
    float distance(const Vector3& other) const;
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;

    // Static functions
    static Vector3 zero();
    static Vector3 one();
};
