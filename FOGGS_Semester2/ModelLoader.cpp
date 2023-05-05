#include "ModelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ModelLoader::ModelLoader(const std::string& modelName)
    : textureID(0), Name(modelName) {  // Initialize textureID to 0

    Transform.Scale.x = 1.0;
    Transform.Scale.y = 1.0;
    Transform.Scale.z = 1.0;
    std::cout << modelName << std::endl;
    if (!loadObj(GetModelPath(Name))) {
        std::cerr << "Failed to load OBJ file: " << GetModelPath(Name) << std::endl;
    }

    //if (!loadTexture(GetTexturePath(Name))) {
    //    std::cerr << "Failed to load texture file: " << GetTexturePath(Name) << std::endl;
    //}
}

ModelLoader::~ModelLoader() {
    glDeleteTextures(1, &textureID);
}

void ModelLoader::render() {
    if (textureID == 0) {
        return;
    }

    // Translation
    glTranslatef(Transform.Position.x, Transform.Position.y, Transform.Position.z);

    //Rotation
    glRotatef(Transform.Rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(Transform.Rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(Transform.Rotation.z, 0.0f, 0.0f, 1.0f);

    //Scaling
    glScalef(Transform.Scale.x, Transform.Scale.y, Transform.Scale.z);

    GLfloat color[4] = { 1.0, 1.0, 1.0 , 1.0 };
    glColor4fv(color);
    GLfloat specular[] = { 0.1f, 0.1f, 0.1f },
        shininess = 0.1f;

    glMaterialfv(GL_FRONT, GL_SPECULAR, Material.Specular);
    glMaterialf(GL_FRONT, GL_SHININESS, Material.Shine);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::string lastMaterial = ""; // Add a variable to keep track of the last material
    for (const Face& face : faces) {
        if (face.materialName != lastMaterial) { // Check if the material has changed
            lastMaterial = face.materialName;
            glBindTexture(GL_TEXTURE_2D, materialTextures[lastMaterial]);
        }
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; ++i) {
            const TexCoord& texCoord = texCoords[face.texCoordIndex[i]];
            const Vertex& vertex = vertices[face.vertexIndex[i]];
            const Normal& normal = normals[face.normalIndex[i]];  // Get the vertex normal
            glTexCoord2f(texCoord.u, texCoord.v);
            glNormal3f(normal.x, normal.y, normal.z);  // Specify the vertex normal
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
        glEnd();

    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    // Disable alpha blending
    glDisable(GL_BLEND);
}

bool ModelLoader::loadObj(const std::string& filename) {

    std::ifstream objFile(filename);

    if (!objFile.is_open()) {
        std::cerr << "Error opening file: " << std::strerror(errno) << std::endl;
        return false;
    }
    std::cout << "going ahead" << std::endl;
    std::string line;
    std::string currentMaterial;
    while (std::getline(objFile, line)) {
        std::istringstream lineStream(line);
        std::string type;
        lineStream >> type;

        if (type == "v") {
            Vertex vertex;
            lineStream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (type == "vt") {
            TexCoord texCoord;
            lineStream >> texCoord.u >> texCoord.v;
            texCoords.push_back(texCoord);
        }
        else if (type == "mtllib") {
            std::string mtlFilename;
            lineStream >> mtlFilename;
            if (!loadMtl(GetMaterialPath(mtlFilename))) {
                std::cerr << "Failed to load MTL file: " << GetMaterialPath(mtlFilename) << std::endl;

                // Load single texture if MTL file loading failed
                if (!loadTexture(GetTexturePath(Name))) {
                    std::cerr << "Failed to load texture file: " << GetTexturePath(Name) << "Fallback to default material." << std::endl;
                }
                else {
                    // Set the same texture for the default material
                    materialTextures["default"] = textureID;
                    currentMaterial = "default";
                }
            }
        }
        else if (type == "usemtl") {
            lineStream >> currentMaterial;
        }
        else if (type == "f") {
            Face face;
            char slash;
            int normalIndex;  // Add a variable to store the normal index
            for (int i = 0; i < 3; ++i) {
                // Add another slash to the input stream extraction to handle the normal index
                lineStream >> face.vertexIndex[i] >> slash >> face.texCoordIndex[i] >> slash >> face.normalIndex[i];
                --face.vertexIndex[i];
                --face.texCoordIndex[i];
                --face.normalIndex[i];  // Adjust indices to be zero-based
            }
            face.materialName = currentMaterial;
            faces.push_back(face);
        }
        else if (type == "vn") {
            Normal normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
    }

    objFile.close();
    return true;
}

bool ModelLoader::loadTexture(const std::string& filename) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Error loading image: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // If the image has an alpha channel, use GL_RGBA, otherwise use GL_RGB
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    textureID = createTexture(data, width, height, format);

    stbi_image_free(data);

    return textureID != 0;
}

GLuint ModelLoader::createTexture(const unsigned char* data, int width, int height, GLenum format) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812f);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

bool ModelLoader::loadMtl(const std::string& filename) {
    std::ifstream mtlFile(filename);

    if (!mtlFile.is_open()) {
        return false;
    }

    std::string line;
    std::string currentMaterial;
    while (std::getline(mtlFile, line)) {
        std::istringstream lineStream(line);
        std::string type;
        lineStream >> type;

        if (type == "newmtl") {
            lineStream >> currentMaterial;
        }
        else if (type == "map_Kd") {
            std::string textureName;
            lineStream >> textureName;
            std::string texturePath = GetTexturePath(textureName);
            if (!loadTexture(texturePath)) {
                std::cerr << "Failed to load texture file: " << texturePath << std::endl;
            }
            else {
                materialTextures[currentMaterial] = textureID;
            }
        }
    }

    mtlFile.close();
    return true;
}