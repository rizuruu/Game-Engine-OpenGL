#pragma once
#include <GL\freeglut.h>

/*
An Art piece, example of texture mapping of the mona lisa
*/
class Art {
public:
	Art();
	void init();
	void drawCubeFace(float x1, float y1, float z1, float x2, float y2, float z2, int axis);
	void draw();
	~Art();
private:
	unsigned char * imageData;
	GLuint texName;
	GLsizei width;
	GLsizei height;

	unsigned char* allocateBmp(char* filename);
};