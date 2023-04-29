#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <GL/freeglut.h>
#include "Transform.h"

class ModelLoader {
public:
    ModelLoader(const std::string& modelName);
    ~ModelLoader();

    void render();
    Transform Transform;
    std::string Name;

private:
    struct Vertex {
        GLfloat x, y, z;
    };

    struct TexCoord {
        GLfloat u, v;
    };

    struct Face {
        GLuint vertexIndex[3];
        GLuint texCoordIndex[3];
    };

    std::vector<Vertex> vertices;
    std::vector<TexCoord> texCoords;
    std::vector<Face> faces;
    GLuint textureID;

    bool loadObj(const std::string& filename);
    bool loadTexture(const std::string& filename);
    GLuint createTexture(const unsigned char* data, int width, int height);

    // Paths
    const std::string GetModelPath(std::string Name)
    {
        return "..\\Assets\\Models\\" + Name + ".obj";
    }
    const std::string GetTexturePath(std::string Name)
    {
        return "..\\Assets\\Textures\\" + Name + ".bmp";
    }
};

#endif // MODEL_LOADER_H