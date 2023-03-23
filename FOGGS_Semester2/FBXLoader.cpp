#include <Windows.h>
#include <gl/GL.h>   
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "fbxsdk.h"
#include <iostream>
#include <vector>

// Structure for holding vertex data
struct Vertex {
    double x, y, z;
};

// Structure for holding face data
struct Face {
    int v1, v2, v3;
};

// Container for storing vertex and face data
std::vector<Vertex> vertices;
std::vector<Face> faces;

// Function for loading FBX file
void loadFbxFile(std::string filename) {
    // Initialize the FBX SDK
    FbxManager* manager = FbxManager::Create();
    FbxIOSettings* iosettings = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(iosettings);

    // Create an FBX importer
    FbxImporter* importer = FbxImporter::Create(manager, "");

    // Initialize the importer with the FBX file
    if (!importer->Initialize(filename.c_str(), -1, manager->GetIOSettings())) {
        std::cout << "Error loading FBX file: " << importer->GetStatus().GetErrorString() << std::endl;
        return;
    }

    // Create a new FBX scene
    FbxScene* scene = FbxScene::Create(manager, "Scene");

    // Import the FBX file into the scene
    importer->Import(scene);

    // Traverse the scene and extract the mesh data
    FbxNode* root = scene->GetRootNode();
    if (root) {
        for (int i = 0; i < root->GetChildCount(); i++) {
            FbxNode* child = root->GetChild(i);
            FbxMesh* mesh = child->GetMesh();

            if (mesh) {
                // Extract vertex data
                for (int j = 0; j < mesh->GetControlPointsCount(); j++) {
                    FbxVector4 vertex = mesh->GetControlPointAt(j);
                    Vertex v = { vertex[0], vertex[1], vertex[2] };
                    vertices.push_back(v);
                }

                // Extract face data
                for (int j = 0; j < mesh->GetPolygonCount(); j++) {
                    int v1 = mesh->GetPolygonVertex(j, 0);
                    int v2 = mesh->GetPolygonVertex(j, 1);
                    int v3 = mesh->GetPolygonVertex(j, 2);
                    Face f = { v1, v2, v3 };
                    faces.push_back(f);
                }
            }
        }
    }

    // Destroy the importer, scene, and manager
    importer->Destroy();
    scene->Destroy();
    iosettings->Destroy();
    manager->Destroy();
}

// Function for rendering mesh
void renderMesh() {
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < faces.size(); i++) {
        // Render each face as a triangle
        glVertex3d(vertices[faces[i].v1].x, vertices[faces[i].v1].y, vertices[faces[i].v1].z);
        glVertex3d(vertices[faces[i].v2].x, vertices[faces[i].v2].y, vertices[faces[i].v2].z);
        glVertex3d(vertices[faces[i].v3].x, vertices[faces[i].v3].y, vertices[faces[i].v3].z);
    }

    glEnd();
}

// GLUT