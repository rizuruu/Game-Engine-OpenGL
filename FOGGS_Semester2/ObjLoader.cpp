#include "ObjLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <GL/freeglut.h> // Assuming freeglut is installed

ObjLoader::ObjLoader()
{
    m_vertices = nullptr;
    m_normals = nullptr;
    m_texCoords = nullptr;
    m_indices = nullptr;
    m_numVertices = 0;
    m_numNormals = 0;
    m_numTexCoords = 0;
    m_numIndices = 0;

    Transform.Scale.x = 1.0;
    Transform.Scale.y = 1.0;
    Transform.Scale.z = 1.0;
}

ObjLoader::~ObjLoader()
{
    if (m_vertices != nullptr) delete[] m_vertices;
    if (m_normals != nullptr) delete[] m_normals;
    if (m_texCoords != nullptr) delete[] m_texCoords;
    if (m_indices != nullptr) delete[] m_indices;
}

bool ObjLoader::loadObj(const std::string& fileName)
{
    Name = fileName;

    std::ifstream file(ModelsPath());
    if (!file.is_open())
    {
        std::cerr << "Unable to open file " << ModelsPath() << std::endl;
        return false;
    }

    std::vector<float> vertices, normals, texCoords;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if (token == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        }
        else if (token == "vt")
        {
            float u, v;
            iss >> u >> v;
            texCoords.push_back(u);
            texCoords.push_back(v);
        }
        else if (token == "f")
        {
            unsigned int v1, v2, v3, t1, t2, t3, n1, n2, n3;
            char slash;
            iss >> v1 >> slash >> t1 >> slash >> n1
                >> v2 >> slash >> t2 >> slash >> n2
                >> v3 >> slash >> t3 >> slash >> n3;
            // OBJ files are 1-indexed
            indices.push_back(v1 - 1);
            indices.push_back(v2 - 1);
            indices.push_back(v3 - 1);
        }
    }

    file.close();

    m_numVertices = vertices.size();
    m_vertices = new float[m_numVertices];
    std::memcpy(m_vertices, vertices.data(), m_numVertices * sizeof(float));

    m_numNormals = normals.size();
    m_normals = new float[m_numNormals];
    std::memcpy(m_normals, normals.data(), m_numNormals * sizeof(float));

    m_numTexCoords = texCoords.size();
    m_texCoords = new float[m_numTexCoords];
    std::memcpy(m_texCoords, texCoords.data(), m_numTexCoords * sizeof(float));

    m_numIndices = indices.size();
    m_indices = new unsigned int[m_numIndices];
    std::memcpy(m_indices, indices.data(), m_numIndices * sizeof(unsigned int));

    loadTexture(TexturesPath());
    return true;
}

void ObjLoader::loadTexture(const std::string& textureFile)
{
    std::cout << "tex" << textureFile << std::endl;
    unsigned char* textureData = allocateBmp(textureFile.c_str());

    if (!textureData) {
        std::cerr << "Unable to open texture file " << textureFile << std::endl;
        return;
    }
    GLfloat color[4] = { 1.0, 1.0, 1.0 , 1.0 };
    glColor4fv(color);
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f },
        shininess = 128.0f;

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Check if anisotropic filtering is supported
    GLfloat maxAnisotropy;
    glGetFloatv(0x84FF, &maxAnisotropy);

    // Enable anisotropic filtering if supported, with a desired level (e.g., 4)
    if (maxAnisotropy > 1.0f)
    {
        GLfloat anisotropyLevel = std::min(7.0f, maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, 0x84FE, anisotropyLevel);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] textureData;
}

void ObjLoader::render() const
{
    // Translation
    glTranslatef(Transform.Position.x, Transform.Position.y, Transform.Position.z);

    //Rotation
    glRotatef(Transform.Rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(Transform.Rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(Transform.Rotation.z, 0.0f, 0.0f, 1.0f);

    //Scaling
    glScalef(Transform.Scale.x, Transform.Scale.y, Transform.Scale.z);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, m_vertices);
    if (m_normals != nullptr)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, m_normals);
    }

    if (m_texCoords != nullptr)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);

        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glEnable(GL_TEXTURE_2D);
    }

    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, m_indices);

    if (m_texCoords != nullptr)
    {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (m_normals != nullptr)
    {
        //glDisableClientState(GL_NORMAL_ARRAY);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
}

unsigned char* ObjLoader::allocateBmp(const char* filename)
{
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

std::string ObjLoader::GetName(std::string n)
{
    std::string filename = n;

    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) 
    {
        std::string filenameWithoutExtension = filename.substr(0, pos);
        return filenameWithoutExtension;
    }
    return n;
}