#pragma once
#include <cmath>
struct Vector3
{
    Vector3() {};
    Vector3(float xVal, float yVal, float zVal)
        : x(xVal), y(yVal), z(zVal)
    {
    }
    float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

    static Vector3 Zero() {
        Vector3 v;
        return v;
    }

    static Vector3 One() {
        Vector3 v;
        v.x = 1.0f;
        v.y = 1.0f;
        v.z = 1.0f;
        return v;
    }
    void Vector3::rotateY(float angle) {
        float radians = angle * (3.14159265f / 180.0f);
        float newX = x * cos(radians) - z * sin(radians);
        float newZ = x * sin(radians) + z * cos(radians);
        x = newX;
        z = newZ;
    }
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    Vector3 normalize() const {
        float length = std::sqrt(x * x + y * y + z * z);
        return Vector3(x / length, y / length, z / length);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
};