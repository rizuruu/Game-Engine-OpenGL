#pragma once
#include <GL/glut.h>

/*
DirectionalLight is a light source with parallel light rays and uniform light distribution.
*/
class DirectionalLight
{
public:
	GLfloat color[4];
	GLfloat direction[3];

	DirectionalLight();
	void addLight();
	void draw();
	void disable();
	void enable();
	void setIntensity(GLfloat intensity);
	~DirectionalLight() = default;

private:
	GLfloat ambient[4];
	GLfloat specular[4];
	GLfloat intensity;
};
