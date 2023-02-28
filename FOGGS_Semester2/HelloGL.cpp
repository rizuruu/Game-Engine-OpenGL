#pragma once
#include "HelloGL.h"
#include "GLUTCallbacks.h"
#include <iostream>

HelloGL::HelloGL(int argc, char* argv[])
{
	rotation = 0.0f;
	camera = new Camera();
	//camera->eye.x = 0.0f; camera->eye.y = 0.0f; camera->eye.z = 1.0f;
	camera->eye.x = 0.0f; camera->eye.y = 0.0f; camera->eye.z = -1.0f;
	camera->center.x = 0.0f; camera->center.y = 0.0f; camera->center.z = 0.0f;
	camera->up.x = 0.0f; camera->up.y = 1.0f; camera->up.z = 0.0f;

	GLUTCallbacks::Init(this); 
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	// Set the viewport to be the entire window
	glViewport(0, 0, 800, 800);

	// Set the correct perspective.
	gluPerspective(45, 1, 0, 1000);

	glutCreateWindow("Simple OpenGL Program");
	glutDisplayFunc(GLUTCallbacks::Display);
	glutKeyboardFunc(GLUTCallbacks::Keyboard);
	glutTimerFunc(REFRESHRATE, GLUTCallbacks::Timer, REFRESHRATE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
	delete camera;
}

void HelloGL::Update()
{
	glLoadIdentity();
	gluLookAt(camera->eye.x, camera->eye.y, camera->eye.z, camera->center.x, camera->center.y, camera->center.z, camera->up.x, camera->up.y, camera->up.z);

	glutPostRedisplay();
	//rotation += 1.5f;

	if (rotation >= 360.0f)
		rotation = 0.0f;
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT); //this clears the scene

	glPushMatrix();
	glRotatef(rotation, 1.5f, 1.5f, -5.0f);
	glutSolidTeapot(0.3f);

	//glBegin(GL_TRIANGLES);
	//// face v0-v1-v2
	//glColor3f(1, 1, 1);
	//glVertex3f(1, 1, 1);
	//glColor3f(1, 1, 0);
	//glVertex3f(-1, 1, 1);
	//glColor3f(1, 0, 0);
	//glVertex3f(-1, -1, 1);
	//// face v2-v3-v0
	//glColor3f(1, 0, 0);
	//glVertex3f(-1, -1, 1);
	//glColor3f(1, 0, 1);
	//glVertex3f(1, -1, 1);
	//glColor3f(1, 1, 1);
	//glVertex3f(1, 1, 1);

	//// face v0-v3-v4
	//glColor3f(1, 1, 1);
	//glVertex3f(1, 1, 1);
	//glColor3f(1, 0, 1);
	//glVertex3f(1, -1, 1);
	//glColor3f(0, 0, 1);
	//glVertex3f(1, -1, -1);
	//// face v4-v5-v0
	//glColor3f(0, 0, 1);
	//glVertex3f(1, -1, -1);
	//glColor3f(0, 1, 1);
	//glVertex3f(1, 1, -1);
	//glColor3f(1, 1, 1);
	//glVertex3f(1, 1, 1);

	//// face v0-v5-v6
	//glColor3f(1, 1, 1);
	//glVertex3f(1, 1, 1);
	//glColor3f(0, 1, 1);
	//glVertex3f(1, 1, -1);
	//glColor3f(0, 1, 0);
	//glVertex3f(-1, 1, -1);
	//// face v6-v1-v0
	//glColor3f(0, 1, 0);
	//glVertex3f(-1, 1, -1);
	//glColor3f(1, 1, 0);
	//glVertex3f(-1, 1, 1);
	//glColor3f(1, 1, 1);
	//glVertex3f(1, 1, 1);

	//// face  v1-v6-v7
	//glColor3f(1, 1, 0);
	//glVertex3f(-1, 1, 1);
	//glColor3f(0, 1, 0);
	//glVertex3f(-1, 1, -1);
	//glColor3f(0, 0, 0);
	//glVertex3f(-1, -1, -1);
	//// face v7-v2-v1
	//glColor3f(0, 0, 0);
	//glVertex3f(-1, -1, -1);
	//glColor3f(1, 0, 0);
	//glVertex3f(-1, -1, 1);
	//glColor3f(1, 1, 0);
	//glVertex3f(-1, 1, 1);

	//// face v7-v4-v3
	//glColor3f(0, 0, 0);
	//glVertex3f(-1, -1, -1);
	//glColor3f(0, 0, 1);
	//glVertex3f(1, -1, -1);
	//glColor3f(1, 0, 1);
	//glVertex3f(1, -1, 1);
	//// face v3-v2-v7
	//glColor3f(1, 0, 1);
	//glVertex3f(1, -1, 1);
	//glColor3f(1, 0, 0);
	//glVertex3f(-1, -1, 1);
	//glColor3f(0, 0, 0);
	//glVertex3f(-1, -1, -1);

	//// face v4-v7-v6
	//glColor3f(0, 0, 1);
	//glVertex3f(1, -1, -1);
	//glColor3f(0, 0, 0);
	//glVertex3f(-1, -1, -1);
	//glColor3f(0, 1, 0);
	//glVertex3f(-1, 1, -1);
	//// face v6-v5-v4
	//glColor3f(0, 1, 0);
	//glVertex3f(-1, 1, -1);
	//glColor3f(0, 1, 1);
	//glVertex3f(1, 1, -1);
	//glColor3f(0, 0, 1);
	//glVertex3f(1, -1, -1);

	glEnd(); // defines the end of the draw

	glPopMatrix();

	glFlush(); //flushes the scene drawn to the graphics card
}

void HelloGL::DrawPolygon()
{
	glBegin(GL_POLYGON); //starts to draw a polygon
	{
		glVertex2f(-0.75, 0.5); //define the first point of the polygon,top left
		glVertex2f(0.75, 0.5); //next point, top right
		glVertex2f(0.75, -0.5); //bottom right
		glVertex2f(-0.75, -0.5); //last point of the polygon, bottom left
		glEnd(); // defines the end of the draw
	}
}

void HelloGL::Keyboard(unsigned char key, int x, int y)
{
	if (key == 'd')
		rotation += 1.7f;
	if (key == 'a')
		rotation -= 1.7f;
}