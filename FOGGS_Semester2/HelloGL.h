#pragma once
#include <Windows.h> //Required for OpenGL on Windows 
#include <gl/GL.h> //OpenGL 
#include <gl/GLU.h> //OpenGL Utilities 
#include "GL\freeglut.h" //freeglut library
#include "Structures.h"
#include "Cube.h"

#define REFRESHRATE 16

class HelloGL
{
private:
	float rotation;
public:
	Camera* camera;
	Cube* cube;

	HelloGL(int argc, char* argv[]);
	~HelloGL(void);
	void InitObjects();
	void InitGL(int argc, char* argv[]);

	void Update();

	void Display();
	void Keyboard(unsigned char key, int x, int y);
};