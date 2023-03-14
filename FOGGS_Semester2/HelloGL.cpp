#include "HelloGL.h"
#include "GLUTCallbacks.h"

HelloGL::HelloGL(int argc, char* argv[])
{
	cube = new Cube();
	//camera initilisation
	camera = new Camera();
	//camera->eye.x = 0.0f; camera->eye.y = 0.0f; camera->eye.z = 1.0f; 
	camera->eye.x = 5.0f; camera->eye.y = 5.0f; camera->eye.z = -5.0f;
	camera->center.x = 0.0f; camera->center.y = 0.0f; camera->center.z = 0.0f;
	camera->up.x = 0.0f; camera->up.y = 1.0f; camera->up.z = 0.0f;

	rotation = 0.0f;

	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE); //enables double buffering
	glutInitWindowSize(800, 800);

	glutInitWindowPosition(500, 100);
	glutCreateWindow("Simple OpenGL Program");
	glutTimerFunc(REFRESHRATE, GLUTCallbacks::Timer, REFRESHRATE);
	glutDisplayFunc(GLUTCallbacks::Display);
	glutKeyboardFunc(GLUTCallbacks::Keyboard);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, 800, 800);
	gluPerspective(45, 1, 0, 1000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glEnable(GL_BACK);


	glutMainLoop();
}

HelloGL::~HelloGL(void)
{
	delete camera;
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	cube->Draw();
	glFlush();
	glutSwapBuffers();
}

void HelloGL::Update()
{
	glLoadIdentity();
	//camera control
	gluLookAt(camera->eye.x, camera->eye.y, camera->eye.z, camera->center.x, camera->center.y, camera->center.z, camera->up.x, camera->up.y, camera->up.z);
	glutPostRedisplay();
	if (rotation >= 360.0f)
		rotation = 0.0f;
}

void HelloGL::Keyboard(unsigned char key, int x, int y)
{
	//if (key == 'd')
		//rotate += 0.5f; 
	//if (key == 'a')
		//rotate += -0.5f; 
	// shifts perspective
	if (key == 'a')
		camera->center.x += 0.1f; camera->eye.x += 0.1f;
	if (key == 'd')
		camera->center.x += -0.1f; camera->eye.x += -0.1f;
}