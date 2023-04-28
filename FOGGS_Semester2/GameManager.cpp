#include "GameManager.h"
#include "GLUTCallbacks.h"
#include "Constants.h"
#include <fstream>
#include <imgui/imgui_internal.h>

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

	glutDisplayFunc(GLUTCallbacks::Display);
	// Setup ImGui binding
	ImGui::CreateContext();

	ImGui_ImplFreeGLUT_Init();
	ImGui_ImplFreeGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();
	// Load the model and texture
	try {
		model = new ModelLoader("..\\Assets\\Models\\model.obj", "..\\Assets\\Textures\\model.bmp");
		Models.push_back(model);
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	if (!Constants::EditorMode)
	{
		glutKeyboardFunc((void(*)(unsigned char, int, int))GLUTCallbacks::Keyboard);
		glutSetCursor(GLUT_CURSOR_NONE);
		glutPassiveMotionFunc(GLUTCallbacks::Motion);
	}
	else
	{
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	//glutMouseFunc(mouse);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SkyboxRenderer = new Skybox(skyboxFilenames);
	SkyboxRenderer->init();
	gContext.pointlight.enable();
	gContext.spotlight.enable();
	gContext.dog.init();
	//gContext.art.init();

	// Setup style
	ImGui::StyleColorsClassic();

	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplFreeGLUT_Shutdown();
	ImGui::DestroyContext();
}

//display handling, rendering all objects
void GameManager::Display() {
	//imgui new frame
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
	gluPerspective(40.0, io.DisplaySize.x / io.DisplaySize.y, 1.0, 1500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//update the dog's transformation matrix
	if (gContext.dog.nextMove) {
		gContext.dog.isMoving = true;
		GLfloat viewModelMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, viewModelMatrix);
		glLoadMatrixf(gContext.dog.local);
		gContext.dog.nextMove();
		gContext.dog.nextMove = nullptr;
		glGetFloatv(GL_MODELVIEW_MATRIX, gContext.dog.local);
		glLoadMatrixf(viewModelMatrix);
	}

	//change viewing mode if in Doggy view setup
	if (gContext.isDogView) {
		GLfloat viewModelMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, viewModelMatrix);
		glLoadMatrixf(gContext.dog.local);

		glRotatef(gContext.dog.headVerticalAngle, 1, 0, 0);
		glRotatef(gContext.dog.headHorizontalAngle, 0, 1, 0);
		glTranslated(0, 0.75, 0.9);

		GLfloat cameraPoseInDogView[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, cameraPoseInDogView);
		glLoadMatrixf(viewModelMatrix);

		//Hack...
		GLfloat zAngle = atan2(-cameraPoseInDogView[2], cameraPoseInDogView[0]);
		GLfloat yAngle = atan2(-cameraPoseInDogView[9], cameraPoseInDogView[5]);

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
		float targetX = gContext.dog.local[12];
		float targetY = gContext.dog.local[13];
		float targetZ = gContext.dog.local[14];

		// Add the position of the dog to the camera position
		cameraX += targetX;
		cameraY += targetY;
		cameraZ += targetZ;

		// Set up the camera using gluLookAt
		gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0, 1, 0);
	}
	else
	{
		//view mode of camera view setup
		gluLookAt(gContext.camera.position[0], gContext.camera.position[1], gContext.camera.position[2],
			gContext.camera.position[0] + sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f),
			gContext.camera.position[1] + sin(pitch * M_PI / 180.0f),
			gContext.camera.position[2] + cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f),
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
	drawHUD("Score: 100");
	glutPostRedisplay();
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

	gContext.floor.draw();

	glPushMatrix();
	glMultMatrixf(gContext.dog.local);
	gContext.dog.draw();
	glPopMatrix();

	//glPushMatrix();
	//glTranslated(-3, 1.05, -3);
	//gContext.table.draw();
	//glPopMatrix();

	//glPushMatrix();
	//glTranslated(-2.2, 1.35, -3);
	//gContext.teapot.draw();
	//glPopMatrix();

	//glPushMatrix();
	//glTranslatef(-3.0f, 0, 3);
	//glRotatef(90, 0, 1, 0);
	//gContext.snowman.draw();
	//glPopMatrix();

	//glPushMatrix();
	//glTranslated(1.0f, 1.5f, -4.99f);
	//gContext.art.draw();
	//glPopMatrix();
	//glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < Models.size(); i++)
	{
		glPushMatrix();
		Models[i]->render();
		glPopMatrix();
	}

	glPushMatrix();
	model->render();
	glPopMatrix();

	//g_objLoader.render();

	//gContext.walls.draw({ 0, 1 });
}

//keyboard events handling
void GameManager::keyboard(unsigned char key, int, int) {
	switch (tolower(key)) {
	case 'w':
		gContext.dog.nextMove = []() { glTranslated(0, 0, 0.2); };
		break;
	case 'a':
		gContext.dog.nextMove = []() { glRotatef(7, 0, 1, 0); };
		break;
	case 's':
		gContext.dog.nextMove = []() { glTranslated(0, 0, -0.2); };
		break;
	case 'd':
		gContext.dog.nextMove = []() { glRotatef(-7, 0, 1, 0); };
		break;
	case 'p':
		isEditor = !isEditor;
		break;
		// Handle other key presses
	default:
		break;
	}
	glutPostRedisplay();
}

//
void GameManager::mouse(int button, int state, int x, int y)
{
	lastX = x;
	lastY = y;
}

//
void GameManager::Motion(int x, int y) {
	static int last_x = -1;
	static int last_y = -1;

	if (last_x == -1 && last_y == -1) {
		last_x = x;
		last_y = y;
	}

	int delta_x = x - last_x;
	int delta_y = y - last_y;

	// Invert delta values to reverse mouse movement
	delta_x *= -1;
	delta_y *= -1;

	// Update camera yaw and pitch based on mouse movement
	yaw += delta_x * 0.2f;
	pitch += delta_y * 0.2f;

	// Make sure pitch stays within [-90, 90] degrees
	pitch = fmaxf(-90.0f, fminf(90.0f, pitch));

	last_x = x;
	last_y = y;

	// Center of the screen
	int center_x = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int center_y = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	glutPostRedisplay();
}

//gui interaction handling via imgui
void GameManager::guiInteraction()
{
	//ImGui::PushStyleColor(ImGuiCol_TitleBg , ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
	//ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	ImGuiWindowFlags window_flags = 0;
	if (ImGui::Begin("My Scene Properties", false, window_flags))
	{
		//ImGui::RadioButton("external view", &gContext.isDogView, 0); ImGui::SameLine();
		//ImGui::RadioButton("doggy view", &gContext.isDogView, 1);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100); // set width of first column
		ImGui::Text("Model name:");
		ImGui::NextColumn();
		ImGui::InputText("without extension", inputText, 256);
		ImGui::Columns(1); // reset to a single column

		float fullWidth = ImGui::GetContentRegionAvailWidth();
		ImVec2 fullSizeButton(fullWidth, 20); // You can adjust the height value according to your needs
		if (ImGui::Button("IMPORT", fullSizeButton))
		{
			SpawnModel(inputText);
		}

		ImVec2 verticalSpacing(0.0f, 10.0f); // Adjust the height value according to your needs
		ImGui::Dummy(verticalSpacing);

		if (ImGui::CollapsingHeader("Dog"))
		{
			ImGui::SliderFloat("head horizontal", &gContext.dog.headHorizontalAngle, -30.0f, 30.0f);
			ImGui::SliderFloat("head vertical", &gContext.dog.headVerticalAngle, -5.0f, 50.0f);
			ImGui::SliderFloat("tail horizontal", &gContext.dog.tailHorizontalAngle, -25.0f, 25.0f);
			ImGui::SliderFloat("tail vertical", &gContext.dog.tailVerticalAngle, -14.0f, 50.0f);
		}
		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::SliderFloat("camera source x", &gContext.camera.position[0], -10.0f, 10.0f);
			ImGui::SliderFloat("camera source y", &gContext.camera.position[1], -10.0f, 10.0f);
			ImGui::SliderFloat("camera source z", &gContext.camera.position[2], -10.0f, 10.0f);
			ImGui::SliderFloat("camera target x", &gContext.camera.target[0], -10.0f, 10.0f);
			ImGui::SliderFloat("camera target y", &gContext.camera.target[1], -10.0f, 10.0f);
			ImGui::SliderFloat("camera target z", &gContext.camera.target[2], -10.0f, 10.0f);
		}
		static bool pointlight = true;
		static bool spotlight = true;
		if (ImGui::CollapsingHeader("Lights"))
		{
			ImGui::SliderFloat("ambient light adjust", &gContext.globalAmbient, 0.0f, 1.0f);
			ImGui::Checkbox("enable pointlight", &pointlight);

			ImGui::ColorEdit3("point light color", reinterpret_cast<float*>(&gContext.pointlight.color));
			ImGui::SliderFloat("pointlight source x", &gContext.pointlight.position[0], -10.0f, 10.0f);
			ImGui::SliderFloat("pointlight source y", &gContext.pointlight.position[1], -10.0f, 10.0f);
			ImGui::SliderFloat("pointlight source z", &gContext.pointlight.position[2], -10.0f, 10.0f);

			ImGui::Checkbox("enable spotlight", &spotlight);
			ImGui::ColorEdit3("spotlight color", reinterpret_cast<float*>(&gContext.spotlight.color));
			ImGui::SliderFloat("spotlight source x", &gContext.spotlight.position[0], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight source y", &gContext.spotlight.position[1], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight source z", &gContext.spotlight.position[2], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight target x", &gContext.spotlight.target[0], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight target y", &gContext.spotlight.target[1], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight target z", &gContext.spotlight.target[2], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight cutoff", &gContext.spotlight.cutoff, 0.0f, 90.0f);
			ImGui::SliderFloat("spotlight exponent", &gContext.spotlight.exponent, 0.0f, 90.0f);

			pointlight ? gContext.pointlight.enable() : gContext.pointlight.disable();
			spotlight ? gContext.spotlight.enable() : gContext.spotlight.disable();
		}
		if (ImGui::CollapsingHeader("Walls"))
		{
			ImGui::SliderFloat("alpha channel", &gContext.walls.alpha, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Imports")) {
			ImGui::Text("Imported models");
			ImGui::Indent();
			for (int i = 0; i < Models.size(); i++)
			{
				char label[64];
				if (ImGui::CollapsingHeader(Models[i]->Name.c_str()))
				{
					ImGui::Text("Transform");
					ImGui::Indent();
					if (ImGui::CollapsingHeader(i + " Position"))
					{
						ImGui::SliderFloat("X Position", &Models[i]->Transform.Position.x, -10.0f, 10.0f);
						ImGui::SliderFloat("Y Position", &Models[i]->Transform.Position.y, -10.0f, 10.0f);
						ImGui::SliderFloat("Z Position", &Models[i]->Transform.Position.z, -10.0f, 10.0f);
					}
					if (ImGui::CollapsingHeader(i + " Rotation"))
					{
						ImGui::SliderFloat("X Rotation", &Models[i]->Transform.Rotation.x, 0.0f, 360.0f);
						ImGui::SliderFloat("Y Rotation", &Models[i]->Transform.Rotation.y, 0.0f, 360.0f);
						ImGui::SliderFloat("Z Rotation", &Models[i]->Transform.Rotation.z, 0.0f, 360.0f);
					}
					if (ImGui::CollapsingHeader(i + " Scale"))
					{
						float scale = Models[i]->Transform.Scale.x;
						if (ImGui::SliderFloat("All Scale", &scale, 0.0f, 100.0f))
						{
							Models[i]->Transform.Scale.x = scale;
							Models[i]->Transform.Scale.y = scale;
							Models[i]->Transform.Scale.z = scale;
						}
						ImGui::SliderFloat("X Scale", &Models[i]->Transform.Scale.x, 0.0f, 100.0f);
						ImGui::SliderFloat("Y Scale", &Models[i]->Transform.Scale.y, 0.0f, 100.0f);
						ImGui::SliderFloat("Z Scale", &Models[i]->Transform.Scale.z, 0.0f, 100.0f);
					}
					ImGui::Unindent();
					if (ImGui::Button("Reset"))
					{
						Models[i]->Transform.Position = Vector3::Zero();
						Models[i]->Transform.Rotation = Vector3::Zero();
						Models[i]->Transform.Scale = Vector3::One();
					}
					ImGui::SameLine();
					if (ImGui::Button("Delete"))
					{
						DeleteModel(i);
					}
				}
			}

			ImGui::Unindent();
		}

		if (ImGui::Button("Quit"))
		{
			exit(0);
		}

		verticalSpacing = ImVec2(0.0f, 10.0f); 
		ImGui::Dummy(verticalSpacing);

		// Set the desired button size
		ImVec2 buttonSize(150, 30); 

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		ImGui::PushStyleColor(ImGuiCol_Button, Constants::ButtonColor);

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Constants::ButtonHoverColor);

		if (ImGui::Button("Save Scene", buttonSize))
		{
			//saveModels(Models);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene", buttonSize))
		{
			loadModels();
		}

		// Reset the style
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
	ImGui::End();
	//ImGui::PopStyleColor();
}

//
void GameManager::SpawnModel(string name)
{
	ModelLoader* objLoader = new ModelLoader("..\\Assets\\Models\\house.obj", "..\\Assets\\Textures\\house.bmp");
	//bool loaded = objLoader->loadObj(name);
	//if (loaded)
		Models.push_back(objLoader);
}

void GameManager::DeleteModel(int i)
{
	ModelLoader* obj = Models[i];
	Models.erase(Models.begin() + i); 
	delete obj;
}

void GameManager::saveModels(const std::vector<ObjLoader*>& models)
{
	std::ofstream file(Constants::SaveFileName, std::ios::binary | std::ios::trunc);

	if (!file) {
		throw std::runtime_error("Error opening file for writing");
	}

	// Write the number of models to the file
	size_t numModels = models.size();
	file.write(reinterpret_cast<const char*>(&numModels), sizeof(numModels));

	// Write the name and transform of each model to the file
	for (const auto& model : models) {
		const std::string& modelName = model->Name;
		const Transform& transform = model->Transform;

		// Write the name of the model
		file.write(modelName.c_str(), modelName.length() + 1);

		// Write the transform of the model
		file.write(reinterpret_cast<const char*>(&transform), sizeof(Transform));
	}

	file.close();
}

std::vector<ObjLoader*> GameManager::loadModels() {
	std::ifstream file(Constants::SaveFileName, std::ios::binary);

	if (!file) {
		throw std::runtime_error("Error opening file for reading");
	}

	// Read the number of models from the file
	size_t numModels;
	file.read(reinterpret_cast<char*>(&numModels), sizeof(numModels));

	// Read the name and transform of each model from the file
	std::vector<ObjLoader*> models(numModels);
	for (size_t i = 0; i < numModels; i++) {
		// Read the name of the model from the file
		std::string modelName;
		std::getline(file, modelName, '\0');
		cout << modelName;
		// Create a new ObjLoader object and set its name
		models[i] = new ObjLoader();
		models[i]->loadObj(modelName);
		models[i]->Name = modelName;

		// Read the transform of the model from the file
		Transform transform;
		file.read(reinterpret_cast<char*>(&transform), sizeof(Transform));
		cout << transform.Position.x;
		// Set the transform of the model
		models[i]->Transform = transform;
		//Models.push_back(models[i]);
	}

	file.close();
	return models;
}

void GameManager::drawHUD(const std::string& text)
{
	// Save the current matrix mode
	GLint matrixMode;
	glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

	// Switch to 2D mode
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Disable lighting and depth testing
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// Enable blending for text rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set the text color to white
	glColor3f(1.0f, 1.0f, 1.0f);

	// Set the text scale
	glScalef(0.5f, 0.5f, 1.0f);

	// Compute the text position to center it on the screen
	int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)text.c_str());
	int textHeight = 12;
	int x = (glutGet(GLUT_WINDOW_WIDTH) - textWidth) / 2;
	int y = (glutGet(GLUT_WINDOW_HEIGHT) - textHeight) / 2;

	// Position the text
	glTranslatef(x, y, 0.0f);

	// Draw the text string using GLUT bitmap fonts
	for (char c : text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
	}

	// Disable blending
	glDisable(GL_BLEND);

	// Restore the previous matrix mode
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(matrixMode);
}