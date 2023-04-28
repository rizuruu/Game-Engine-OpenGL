#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <GL/freeglut.h>

class ModelLoader {
public:
    ModelLoader(const std::string& objFilename, const std::string& textureFilename);
    ~ModelLoader();

    void render();

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
};

#endif // MODEL_LOADER_H