#include "HelloGL.h"
#include "GLUTCallbacks.h"
#include <iostream>

HelloGL::HelloGL(int argc, char* argv[])
{
	rotation = 0.0f;

	GLUTCallbacks::Init(this); 
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple OpenGL Program");
	glutDisplayFunc(GLUTCallbacks::Display);
	glutKeyboardFunc(GLUTCallbacks::Keyboard);
	glutTimerFunc(REFRESHRATE, GLUTCallbacks::Timer, REFRESHRATE);
	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
}

void HelloGL::Update()
{
	glutPostRedisplay();
	//rotation += 1.5f;

	if (rotation >= 360.0f)
		rotation = 0.0f;
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT); //this clears the scene

	glPushMatrix();
	glRotatef(rotation, 0.5f, 0.5f, 0.5f);

	glBegin(GL_POLYGON); //starts to draw a polygon
	glColor3f(1, 1, 1);
	glVertex2f(-0.75, 0.5); //define the first point of the polygon,top left
	glColor3f(1, 0, 0);
	glVertex3f(-0.75, 0.5, 0.75);
	glColor3f(0, 1, 0);
	glVertex3f(0.75, 0.5, 0.75);

	glColor3f(0.5, 0.3, 0.3);
	glVertex2f(0.75, 0.5); //next point, top right
	glVertex2f(0.75, -0.5); //bottom right
	glVertex2f(-0.75, -0.5); //last point of the polygon, bottom left

	glColor3f(0, 0, 1);
	glVertex3f(-0.75, -0.5, 0.75);
	glVertex3f(-0.75, 0.5, 0.75);

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