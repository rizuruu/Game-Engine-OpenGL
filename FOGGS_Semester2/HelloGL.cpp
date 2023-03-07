#pragma once
#include "HelloGL.h"
#include "GLUTCallbacks.h"
#include <iostream>

HelloGL::HelloGL(int argc, char* argv[])
{
	rotation = 0.0f;
	cube = new Cube();
	camera = new Camera();
	//camera->eye.x = 5.0f; camera->eye.y = 5.0f; camera->eye.z = -5.0f;
	camera->eye.x = 0.0f; camera->eye.y = 0.0f; camera->eye.z = -2.0f;
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
	glEnable(GL_DEPTH_TEST | GL_CULL_FACE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glCullFace(GL_BACK);


	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
	delete camera;
}

void HelloGL::Update()
{
	cube->Update();

	glLoadIdentity();
	gluLookAt(camera->eye.x, camera->eye.y, camera->eye.z, camera->center.x, camera->center.y, camera->center.z, camera->up.x, camera->up.y, camera->up.z);

	glutPostRedisplay();
	//rotation += 1.5f;

	if (rotation >= 360.0f)
		rotation = 0.0f;
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cube->Draw();

	glFlush(); //flushes the scene drawn to the graphics card
	//glutPostRedisplay();
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
		cube->Translate(key, 1.0f);
	if (key == 'a')
		rotation -= 1.7f;
}