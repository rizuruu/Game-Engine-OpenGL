#pragma once
#include <GL/freeglut.h>
#include "ModelLoader.h"

enum SkyType
{
    SixSided,
    Spherical
};

class Skybox {
public:

    Skybox(SkyType type, const char* filenames[6]);
    Skybox(SkyType type);
    ~Skybox();
    void init();
    void draw();

    unsigned char* allocateBmp(char* filename, int& width, int& height);
private:
    GLuint texNames[6];
    unsigned char* imageData[6];
    GLsizei width[6];
    GLsizei height[6];

    SkyType skyType = SkyType::SixSided;
    ModelLoader* SkySphere;
};