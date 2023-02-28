#pragma once
#include <Windows.h> //Required for OpenGL on Windows 
#include <gl/GL.h> //OpenGL 
#include <gl/GLU.h> //OpenGL Utilities 
#include "GL\freeglut.h" //freeglut library

#define REFRESHRATE 16

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Camera
{
	Vector3 eye;
	Vector3 center;
	Vector3 up;
};

class HelloGL
{
private:
	float rotation;

public:
	Camera* camera;

	HelloGL(int argc, char* argv[]);
	~HelloGL(void);

	void Update();

	void Display();
	void DrawPolygon();

	void Keyboard(unsigned char key, int x, int y);
};