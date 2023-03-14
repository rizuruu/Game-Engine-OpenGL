#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL\freeglut.h"
#include "Structures.h"
#include <iostream>

class Cube
{
private:

	Mesh* _mesh;
	GLfloat _rotation;
	GLfloat _translation;

public:
	Cube(Mesh* mesh, float x, float y, float z);
	~Cube();

	void Draw();
	void Update();
	void SetRotation(float rotation);
	void Translate(unsigned char key, float speed);

	//private:
//	static Vertex indexedVertices[];
//	static Color indexedColors[];
//	static GLushort indices[];
//	GLfloat _rotation;
//	GLfloat _translation;
//
//public:
//	Cube();
};