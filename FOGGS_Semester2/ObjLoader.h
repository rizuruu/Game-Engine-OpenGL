#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <string>
#include <vector>
#include <GL\freeglut.h>
#include "Transform.h"

class ObjLoader {
public:
    ObjLoader();
    ~ObjLoader();
    void render() const;
    bool loadObj(const std::string& fileName);

    unsigned char* allocateBmp(const char* filename);
    Transform Transform;
    std::string Name;
private:
    float* m_vertices;
    float* m_normals;
    float* m_texCoords;
    unsigned int* m_indices;
    int m_numVertices;
    int m_numNormals;
    int m_numTexCoords;
    int m_numIndices;
    GLuint m_textureID;
    GLsizei width;
    GLsizei height;
    void loadTexture(const std::string& textureFile);
    std::string GetName(std::string n);

    // Paths
    std::string ModelsPath()
    {
        return "..\\Assets\\Models\\" + Name + ".obj";
    }
    std::string TexturesPath()
    {
        return "..\\Assets\\Textures\\" + Name + ".bmp";
    }
};

#endif // OBJLOADER_H