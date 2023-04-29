#include "ModelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

ModelLoader::ModelLoader(const std::string& modelName)
    : textureID(0), Name(modelName) {  // Initialize textureID to 0

    Transform.Scale.x = 1.0;
    Transform.Scale.y = 1.0;
    Transform.Scale.z = 1.0;

    if (!loadObj(GetModelPath(Name))) {
        std::cerr << "Failed to load OBJ file: " << GetModelPath(Name) << std::endl;
    }

    if (!loadTexture(GetTexturePath(Name))) {
        std::cerr << "Failed to load texture file: " << GetTexturePath(Name) << std::endl;
    }
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
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f },
        shininess = 128.0f;

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_TRIANGLES);
    for (const Face& face : faces) {
        for (int i = 0; i < 3; ++i) {
            const TexCoord& texCoord = texCoords[face.texCoordIndex[i]];
            const Vertex& vertex = vertices[face.vertexIndex[i]];
            glTexCoord2f(texCoord.u, texCoord.v);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

bool ModelLoader::loadObj(const std::string& filename) {
    std::ifstream objFile(filename);

    if (!objFile.is_open()) {
        return false;
    }

    std::string line;
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
        else if (type == "f") {
            Face face;
            char slash;
            int normalIndex;  // Add a variable to store the normal index
            for (int i = 0; i < 3; ++i) {
                // Add another slash to the input stream extraction to handle the normal index
                lineStream >> face.vertexIndex[i] >> slash >> face.texCoordIndex[i] >> slash >> normalIndex;
                // Adjust indices to be zero-based
                --face.vertexIndex[i];
                --face.texCoordIndex[i];

                // ... (the rest of the function remains unchanged)
            }
            faces.push_back(face);
        }
    }

    objFile.close();
    return true;
}

bool ModelLoader::loadTexture(const std::string& filename) {
    std::ifstream textureFile(filename, std::ios::binary);

    if (!textureFile.is_open()) {
        std::cerr << "Error: Could not open texture file." << std::endl;
        return false;
    }

    BITMAPFILEHEADER fileHeader;
    textureFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    BITMAPINFOHEADER infoHeader;
    textureFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    // Print some debugging information about the texture
    std::cout << "Texture width: " << infoHeader.biWidth << std::endl;
    std::cout << "Texture height: " << infoHeader.biHeight << std::endl;
    std::cout << "Texture bit count: " << infoHeader.biBitCount << std::endl;

    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;
    int imageSize = width * height * (infoHeader.biBitCount / 8);
    unsigned char* data = new unsigned char[imageSize];

    textureFile.seekg(fileHeader.bfOffBits);
    textureFile.read(reinterpret_cast<char*>(data), imageSize);
    textureFile.close();

    textureID = createTexture(data, width, height);

    delete[] data;

    return textureID != 0;
}

GLuint ModelLoader::createTexture(const unsigned char* data, int width, int height) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 0x80E0, GL_UNSIGNED_BYTE, data);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}