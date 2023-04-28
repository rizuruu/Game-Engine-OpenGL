#pragma once
#include <GL/freeglut.h>

class Skybox {
public:

    Skybox(const char* filenames[6]);
    ~Skybox();
    void init();
    void draw();

    unsigned char* allocateBmp(char* filename, int& width, int& height);

private:
    GLuint texNames[6];
    unsigned char* imageData[6];
    GLsizei width[6];
    GLsizei height[6];
};
