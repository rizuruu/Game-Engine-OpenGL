#pragma once
struct Vector3
{
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
};