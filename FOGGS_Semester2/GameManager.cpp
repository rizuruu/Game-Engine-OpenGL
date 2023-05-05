#include "GameManager.h"
#include "GLUTCallbacks.h"
#include "Constants.h"
#include <fstream>
#include <imgui/imgui_internal.h>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace ImGui;
ModelLoader* model;

GameManager::GameManager(int argc, char** argv)
{
	Init(argc, argv);
	// Load the model and texture
}

GameManager::~GameManager()
{
	/*for (ModelLoader* model : Models) {
		delete model;
	}
	Models.clear();*/

	delete SkyboxRenderer;
	delete customFont;
}

// Initialize Freeglut
void GameManager::Init(int argc, char** argv) {
	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(1200, 600);
	glutCreateWindow(Constants::GameTitle);
	glutFullScreen();

	glutTimerFunc(1000 / 60, GLUTCallbacks::Timer, 0);
	glutDisplayFunc(GLUTCallbacks::Display);
	// Setup ImGui binding
	ImGui::CreateContext();
	ImGui_ImplFreeGLUT_Init();
	ImGui_ImplFreeGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("..\\Assets\\Fonts\\Comme-Medium.ttf", 21.0f);	// Load the model and texture

	//try {
	//	model = new ModelLoader("model");
	//	Models.push_back(model);
	//}
	//catch (const std::exception& e) {
	//	std::cerr << "Error: " << e.what() << std::endl;
	//}
	if (Constants::EditorMode)
	{

	}
	else
	{
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	

	glutKeyboardFunc((void(*)(unsigned char, int, int))GLUTCallbacks::Keyboard);
	glutPassiveMotionFunc(GLUTCallbacks::Motion);
	glutMouseWheelFunc(GLUTCallbacks::MouseWheel);

	//glutMouseFunc(mouse);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//
	RoboPlayer = new Player();
	EditorGUIRenderer = new EditorGUI(gContext);
	SkyboxRenderer = new Skybox(SkyType::Spherical);
	gContext.pointlight.enable();
	gContext.spotlight.enable();

	//gContext.art.init();
	// Setup style
	ImGui::StyleColorsClassic();
	//loadModels();
	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplFreeGLUT_Shutdown();
	ImGui::DestroyContext();
}

//display handling, rendering all objects
void GameManager::Display() {
	//imgui new frame
	Constants::ModernDarkTheme();

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplFreeGLUT_NewFrame();

	//update interaction
	if (Constants::EditorMode)
		guiInteraction();

	ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();

	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(Constants::FOV, io.DisplaySize.x / io.DisplaySize.y, Constants::NearClippingPlane, Constants::FarClippingPlane);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//change viewing mode if in Doggy view setup
	if (!Constants::EditorMode) {
		float radius = 10.0f; // Distance from camera to target
		float pitchRadians = -pitch * 3.14159f / 180.0f;
		float yawRadians = yaw * 3.14159f / 180.0f;
		// Check if yaw is less than 0, set it to 0
		if (pitchRadians < 0) {
			pitchRadians = 0;
		}

		// Calculate the camera's position in spherical coordinates
		float cameraX = radius * cos(pitchRadians) * sin(yawRadians);
		float cameraY = radius * sin(pitchRadians);
		float cameraZ = radius * cos(pitchRadians) * cos(yawRadians);

		// Calculate the camera's target position
		float targetX = RoboPlayer->Head->Transform.Position.x;
		float targetY = RoboPlayer->Head->Transform.Position.y;
		float targetZ = RoboPlayer->Head->Transform.Position.z;

		// Add the position of the dog to the camera position
		cameraX += targetX;
		cameraY += targetY;
		cameraZ += targetZ;

		// Set up the camera using gluLookAt
		gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0, 1, 0);
	}
	else if (Constants::EditorMode && freeLook)
	{
		// Lock cursor for freelook in editor mode
		glutSetCursor(GLUT_CURSOR_NONE);

		// Calculate the direction vector from the yaw and pitch angles
		float dirX = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
		float dirY = sin(pitch * M_PI / 180.0f);
		float dirZ = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);

		// Calculate the target position by adding the direction vector to the camera's position
		float targetX = gContext.camera.position[0] + dirX;
		float targetY = gContext.camera.position[1] + dirY;
		float targetZ = gContext.camera.position[2] + dirZ;

		// Set up the camera using gluLookAt
		gluLookAt(gContext.camera.position[0], gContext.camera.position[1], gContext.camera.position[2],
			targetX, targetY, targetZ,
			0, 1, 0);

		// Update the last target position
		lastTargetX = targetX;
		lastTargetY = targetY;
		lastTargetZ = targetZ;
	}
	else
	{
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

		// Set up the camera using gluLookAt with the last target position
		gluLookAt(gContext.camera.position[0], gContext.camera.position[1], gContext.camera.position[2],
			lastTargetX, lastTargetY, lastTargetZ,
			0, 1, 0);
	}

	GLfloat globalAmbientVec[4] = { gContext.globalAmbient, gContext.globalAmbient, gContext.globalAmbient, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientVec);

	drawScene();

	// --- MIRROR------//
	// 
	////create mirror effect
	//glEnable(GL_STENCIL_TEST);
	//glColorMask(0, 0, 0, 0); //Disable drawing colors to the screen
	//glDisable(GL_DEPTH_TEST); //Disable depth testing
	//glStencilFunc(GL_ALWAYS, 1, 1); //Make the stencil test always pass
	//								//Make pixels in the stencil buffer be set to 1 when the stencil test passes
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	////Set all of the pixels covered by the mirror to be 1 in the stencil buffer
	//glPushMatrix();
	//glRotatef(90, 0, 1, 0);
	//glTranslatef(-1, 0.2f, 4.99f);
	//gContext.mirror.draw();
	//glPopMatrix();

	//glColorMask(1, 1, 1, 1); //Enable drawing colors to the screen
	//glEnable(GL_DEPTH_TEST); //Enable depth testing
	//						 //Make the stencil test pass only when the pixel is 1 in the stencil buffer
	//glStencilFunc(GL_EQUAL, 1, 1);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Make the stencil buffer not change

	////Draw the scene, reflected in the mirror
	//glPushMatrix();
	//glTranslatef(10.001f, 0, 0);
	//glScalef(-1, 1, 1);
	//drawScene();
	//glPopMatrix();

	//glDisable(GL_STENCIL_TEST); //Disable using the stencil buffer

	////Blend the mirror onto the screen
	//glEnable(GL_BLEND);
	//glPushMatrix();
	//glRotatef(90, 0, 1, 0);
	//glTranslatef(-1, 0.2f, 4.99f);
	//gContext.mirror.draw();
	//glPopMatrix();
	//glDisable(GL_BLEND);
	// 
	// --- MIRROR------// 

	//add the wall next to the mirror
	//gContext.walls.draw({ 2 });

	//imgui does not handle light well
	glDisable(GL_LIGHTING);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	glEnable(GL_LIGHTING);
	glColor3f(1, 1, 1);

	glFlush();
	glutSwapBuffers();

	//if (isEditor)
	//{
	//	glutIgnoreKeyRepeat(true);
	//	glutKeyboardFunc(nullptr);
	//}
	//else
	//	glutKeyboardFunc(keyboard);
	// Draw the HUD text
	glutPostRedisplay();
}

void GameManager::Update()
{
	glutTimerFunc(1000 / 60, GLUTCallbacks::Timer, 0);
}

//
void GameManager::drawScene() {
	SkyboxRenderer->draw();

	glPushMatrix();
	glTranslatef(gContext.pointlight.position[0], gContext.pointlight.position[1], gContext.pointlight.position[2]);
	gContext.pointlight.addLight();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(gContext.spotlight.position[0], gContext.spotlight.position[1], gContext.spotlight.position[2]);
	gContext.spotlight.addlight();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(gContext.spotlight.position[0], gContext.spotlight.position[1], gContext.spotlight.position[2]);
	gContext.spotlight.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(gContext.pointlight.position[0], gContext.pointlight.position[1], gContext.pointlight.position[2]);
	gContext.pointlight.draw();
	glPopMatrix();

	//gContext.floor.draw();

	if (!Constants::EditorMode)
		RoboPlayer->Render();

	for (int i = 0; i < gContext.GameObjects.size(); i++)
	{
		glPushMatrix();
		gContext.GameObjects[i]->render();
		glPopMatrix();
	}

	//if (Constants::EditorMode)
	//	drawHUD(Constants::Combine("Freelook Speed:", freeLookSpeed), 0, 0);
	//drawHUD(Constants::Combine("Editor Mode:", Constants::EditorMode), 0, 20);
}

//keyboard events handling
void GameManager::Keyboard(unsigned char key, int, int) 
{
	if (Constants::EditorMode)
		EditorInput(key);
	else
		GameInput(key);

	glutPostRedisplay();
}

void GameManager::GameInput(unsigned char key)
{
	RoboPlayer->Update(tolower(key));
}

void GameManager::EditorInput(unsigned char key)
{
	switch (tolower(key)) {
	case 'w':
		gContext.camera.position[0] += sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f) * freeLookSpeed;
		gContext.camera.position[1] += sin(pitch * M_PI / 180.0f) * freeLookSpeed;
		gContext.camera.position[2] += cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f) * freeLookSpeed;
		break;
	case 'a':
		gContext.camera.position[0] += cos(yaw * M_PI / 180.0f) * freeLookSpeed;
		gContext.camera.position[2] -= sin(yaw * M_PI / 180.0f) * freeLookSpeed;
		break;
	case 's':
		gContext.camera.position[0] -= sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f) * freeLookSpeed;
		gContext.camera.position[1] -= sin(pitch * M_PI / 180.0f) * freeLookSpeed;
		gContext.camera.position[2] -= cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f) * freeLookSpeed;
		break;
	case 'd':
		gContext.camera.position[0] -= cos(yaw * M_PI / 180.0f) * freeLookSpeed;
		gContext.camera.position[2] += sin(yaw * M_PI / 180.0f) * freeLookSpeed;
		break;
	case 'p':
		freeLook = !freeLook;
		break;
	case 'l':
		break;
	default:
		break;
	}
}

//
void GameManager::mouse(int button, int state, int x, int y)
{
	lastX = x;
	lastY = y;
}

//
void GameManager::Motion(int x, int y) {
	// Re-center the cursor
	if (freeLook) {

		// Center of the screen
		int center_x = glutGet(GLUT_WINDOW_WIDTH) / 2;
		int center_y = glutGet(GLUT_WINDOW_HEIGHT) / 2;

		// If the cursor is already at the center, do nothing
		if (x == center_x && y == center_y) {
			return;
		}

		int delta_x = x - center_x;
		int delta_y = y - center_y;

		// Invert delta values to reverse mouse movement
		delta_x *= -1;
		delta_y *= -1;

		// Update camera yaw and pitch based on mouse movement
		yaw += delta_x * 0.2f;
		pitch += delta_y * 0.2f;

		// Make sure pitch stays within [-90, 90] degrees
		pitch = fmaxf(-90.0f, fminf(90.0f, pitch));

		glutWarpPointer(center_x, center_y);
	}

	glutPostRedisplay();
}

void GameManager::MouseWheel(int wheel, int direction, int x, int y) {
	if (freeLook)
	{
		if (direction > 0) {
			// Scroll up
			if (freeLookSpeed < 5.0f)
				freeLookSpeed += 0.1f;
		}
		else {
			// Scroll down
			if (freeLookSpeed > 0.1f)
				freeLookSpeed -= 0.1f;
			else
				freeLookSpeed = 0.1f;
		}
	}
	glutPostRedisplay();
}

//gui interaction handling via imgui
void GameManager::guiInteraction()
{
	EditorGUIRenderer->Render();
}

void GameManager::drawHUD(const char* text, float x, float y)
{
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	int textWidth = 0;
	for (const char* c = text; *c != '\0'; c++) {
		textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
	}

	float offsetX = x;
	float offsetY = y + 18; // Add the font height to the Y offset

	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 1.0f);
	glRasterPos2f(offsetX, windowHeight - offsetY);
	for (const char* c = text; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
}
