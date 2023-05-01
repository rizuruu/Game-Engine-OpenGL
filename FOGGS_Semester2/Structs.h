#pragma once
struct Color
{
    Color(float r, float g, float b, float a) : r(r), g(g), b(b) {}

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

struct Material
{
    GLfloat Shine = 0.1f;

    GLfloat Specular[3] = { 0.1f, 0.1f, 0.1f };
    GLfloat Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};