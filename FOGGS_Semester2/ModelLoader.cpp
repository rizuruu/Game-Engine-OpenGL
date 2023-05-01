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

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

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
}

bool ModelLoader::loadObj(const std::string& filename) {
    std::ifstream objFile(filename);

    if (!objFile.is_open()) {
        return false;
    }

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
            std::cout << currentMaterial << std::endl;
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
    for (const auto& materialTexture : materialTextures) {
        std::cout << "Material: " << materialTexture.first << ", Texture ID: " << materialTexture.second << std::endl;
    }

    mtlFile.close();
    return true;
}