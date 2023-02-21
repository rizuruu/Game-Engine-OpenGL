#pragma once
#include <Windows.h> //Required for OpenGL on Windows 
#include <gl/GL.h> //OpenGL 
#include <gl/GLU.h> //OpenGL Utilities 
#include "GL\freeglut.h" //freeglut library

#define REFRESHRATE 16

class HelloGL
{
private:
	float rotation;

public:
	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Update();

	void Display();
	void DrawPolygon();

	void Keyboard(unsigned char key, int x, int y);
};

