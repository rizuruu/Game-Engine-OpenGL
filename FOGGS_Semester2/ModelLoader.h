#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <GL/freeglut.h>
#include "Transform.h"
#include <map>
#include "Structs.h"

class ModelLoader {
public:
    ModelLoader(const std::string& modelName);
    ~ModelLoader();

    void render();
    Transform Transform;
    std::string Name;
    Material Material;
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
        std::string materialName;  
        int normalIndex[3];
    };

    struct Normal {
        float x, y, z;
    };

    std::vector<Vertex> vertices;
    std::vector<TexCoord> texCoords;
    std::vector<Face> faces;
    std::vector<Normal> normals;
    std::map<std::string, GLuint> materialTextures;
    GLuint textureID;

    bool loadObj(const std::string& filename);
    bool loadTexture(const std::string& filename);
    GLuint createTexture(const unsigned char* data, int width, int height, GLenum format);

    bool loadMtl(const std::string& filename);

    // Paths
    const std::string GetModelPath(std::string Name)
    {
        return "..\\Assets\\Models\\" + Name + ".obj";
    }
    const std::string GetTexturePath(std::string Name)
    {
        return "..\\Assets\\Textures\\" + extractFileName(Name) + ".png";
    }
    const std::string GetMaterialPath(std::string Name)
    {
        return "..\\Assets\\Models\\" + Name;
    }

    std::string extractFileName(const std::string& filePath) {
        size_t lastSlash = filePath.find_last_of("/\\");
        size_t lastDot = filePath.find_last_of('.');

        if (lastSlash == std::string::npos) {
            lastSlash = 0; // No directory path found, start from the beginning
        }
        else {
            lastSlash += 1; // Move past the last slash
        }

        if (lastDot == std::string::npos || lastDot < lastSlash) {
            lastDot = filePath.size(); // No file extension found or before the last slash, use the whole string
        }

        return filePath.substr(lastSlash, lastDot - lastSlash);
    }
};

#endif // MODEL_LOADER_H