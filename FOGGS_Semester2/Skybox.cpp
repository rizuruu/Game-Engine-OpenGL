#include "Skybox.h"
#include <iostream>

Skybox::Skybox(SkyType type, const char* filenames[6]) : skyType(type) {

    if (type == SkyType::SixSided)
    {
        for (int i = 0; i < 6; i++) {
            imageData[i] = allocateBmp(const_cast<char*>(filenames[i]), width[i], height[i]);
        }
        init();
        //delete SkySphere;
    }
    else
    {
        SkySphere = new ModelLoader("SkySphere");
        SkySphere->Transform.Scale = Vector3(2250, 2250, 2250);
    }
}

Skybox::Skybox(SkyType type) : skyType(type)
{
    SkySphere = new ModelLoader("SkySphere");
    SkySphere->Transform.Scale = Vector3(2250, 2250, 2250);
}

Skybox::~Skybox() {
    for (int i = 0; i < 6; i++) {
        delete[] imageData[i];
    }
}

void Skybox::init() {
    glGenTextures(6, texNames);
    for (int i = 0; i < 6; i++) {
        glBindTexture(GL_TEXTURE_2D, texNames[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812f);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812f);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[i], height[i], 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[i]);
    }
}

void Skybox::draw() {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    const float size = 1500.0f;

    switch (skyType)
    {
    case SkyType::SixSided:
        std::cout << "sex" << std::endl;
        // Render the six faces of the skybox
        for (int i = 0; i < 6; i++) {
            glBindTexture(GL_TEXTURE_2D, texNames[i]);
            glBegin(GL_QUADS);

            switch (i) {
            case 0: // Left
                glTexCoord2f(1, 0); glVertex3f(-size, -size, -size);
                glTexCoord2f(1, 1); glVertex3f(-size, size, -size);
                glTexCoord2f(0, 1); glVertex3f(-size, size, size);
                glTexCoord2f(0, 0); glVertex3f(-size, -size, size);
                break;
            case 1: // Right
                glTexCoord2f(0, 0); glVertex3f(size, -size, -size);
                glTexCoord2f(1, 0); glVertex3f(size, -size, size);
                glTexCoord2f(1, 1); glVertex3f(size, size, size);
                glTexCoord2f(0, 1); glVertex3f(size, size, -size);
                break;
            case 2: // Bottom
                glTexCoord2f(1, 1); glVertex3f(-size, -size, -size);
                glTexCoord2f(0, 1); glVertex3f(size, -size, -size);
                glTexCoord2f(0, 0); glVertex3f(size, -size, size);
                glTexCoord2f(1, 0); glVertex3f(-size, -size, size);
                break;
            case 3: // Top
                glTexCoord2f(1, 1); glVertex3f(-size, size, -size);
                glTexCoord2f(1, 0); glVertex3f(-size, size, size);
                glTexCoord2f(0, 0); glVertex3f(size, size, size);
                glTexCoord2f(0, 1); glVertex3f(size, size, -size);
                break;
            case 4: // Front
                glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
                glTexCoord2f(1, 0); glVertex3f(size, -size, -size);
                glTexCoord2f(1, 1); glVertex3f(size, size, -size);
                glTexCoord2f(0, 1); glVertex3f(-size, size, -size);
                break;
            case 5: // Back
                glTexCoord2f(1, 0); glVertex3f(-size, -size, size);
                glTexCoord2f(1, 1); glVertex3f(-size, size, size);
                glTexCoord2f(0, 1); glVertex3f(size, size, size);
                glTexCoord2f(0, 0); glVertex3f(size, -size, size);
                break;
            }

            glEnd();
        }
        break;

    case SkyType::Spherical:
        SkySphere->render();
        break;
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

unsigned char* Skybox::allocateBmp(char* filename, int& width, int& height) {
    int i;
    FILE* f = nullptr;
    errno_t status = fopen_s(&f, filename, "rb");
    if (status != 0) {
        return nullptr;
    }
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
                                       // extract image height and width from header
    width = *(int*)&info[18];
    height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for (i = 0; i < size; i += 3)
    {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    return data;
}