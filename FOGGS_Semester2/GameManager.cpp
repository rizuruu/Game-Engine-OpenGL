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
	for (ModelLoader* model : Models) {
		delete model;
	}
	Models.clear();

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

	glutTimerFunc(1000 / 60, GLUTCallbacks::Timer, 0);
	glutDisplayFunc(GLUTCallbacks::Display);
	// Setup ImGui binding
	ImGui::CreateContext();

	ImGui_ImplFreeGLUT_Init();
	ImGui_ImplFreeGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();
	// Load the model and texture
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
	
	readFiles();

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
	
	SkyboxRenderer = new Skybox(SkyType::Spherical);
	gContext.pointlight.enable();
	gContext.spotlight.enable();
	gContext.dog.init();
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
	if (!Constants::EditorMode) {
		GLfloat viewModelMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, viewModelMatrix);
		glLoadMatrixf(gContext.dog.local);

		glRotatef(gContext.dog.headVerticalAngle, 1, 0, 0);
		glRotatef(gContext.dog.headHorizontalAngle, 0, 1, 0);
		glTranslated(0, 0.75, 0.9);

		GLfloat cameraPoseInDogView[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, cameraPoseInDogView);
		glLoadMatrixf(viewModelMatrix);

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

	gContext.floor.draw();

	if (!Constants::EditorMode)
		RoboPlayer->Render();

	for (int i = 0; i < Models.size(); i++)
	{
		glPushMatrix();
		Models[i]->render();
		glPopMatrix();
	}

	if (Constants::EditorMode)
		drawHUD(Constants::Combine("Freelook Speed:", freeLookSpeed), 0, 0);
	drawHUD(Constants::Combine("Editor Mode:", Constants::EditorMode), 0, 20);
}

void GameManager::readFiles()
{
	files.clear();
#ifdef _WIN32
	WIN32_FIND_DATAA fileData;
	HANDLE hFind = FindFirstFileA((path + "*").c_str(), &fileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string filename = fileData.cFileName;
				if (filename.substr(filename.find_last_of(".") + 1) == "obj") {
					files.push_back(filename);
				}
			}
		} while (FindNextFileA(hFind, &fileData));
		FindClose(hFind);
	}
#else
	DIR* dir = opendir(path.c_str());

	if (dir != nullptr) {
		dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			std::string filename = entry->d_name;
			if (filename.substr(filename.find_last_of(".") + 1) == "obj") {
				files.push_back(filename);
			}
		}
		closedir(dir);
	}
#endif
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
	ImGuiWindowFlags window_flags = 0;
	if (ImGui::Begin("My Scene Properties", false, window_flags))
	{
		//ImGui::RadioButton("external view", &gContext.isDogView, 0); ImGui::SameLine();
		//ImGui::RadioButton("doggy view", &gContext.isDogView, 1);

		ImGui::Text("Select Model to import:");
		ImGui::SameLine();
		if (ImGui::Button("Refresh"))
		{
			readFiles();
		}

		ImGui::ListBox("", &importSelectionIndex,
			[](void* data, int index, const char** out_text) {
				*out_text = (*(std::vector<std::string>*)data)[index].c_str();
				return true;
			}, (void*)&files, files.size());

		float fullWidth = ImGui::GetContentRegionAvailWidth();
		ImVec2 fullSizeButton(fullWidth, 20); 
		if (ImGui::Button("IMPORT", fullSizeButton))
		{
			cout << Constants::NameWithoutExt(files[importSelectionIndex]) << endl;

			SpawnModel(Constants::NameWithoutExt(files[importSelectionIndex]));
		}

		ImVec2 verticalSpacing(0.0f, 10.0f); 
		ImGui::Dummy(verticalSpacing);

		static bool pointlight = true;
		static bool spotlight = true;
		ImGui::SliderFloat("FOV", &Constants::FOV, 0.0f, 100.0f);
		if (ImGui::Button("Import HDRI", fullSizeButton))
		{
			string filepath = Constants::openFileDialog();
			std::cout << filepath << std::endl;
			//SkyboxRenderer->SkySphere->loadTexture(filepath);
		}
		if (ImGui::CollapsingHeader("Lights"))
		{
			ImGui::SliderFloat("ambient light adjust", &gContext.globalAmbient, 0.0f, 1.0f);
			ImGui::Checkbox("enable pointlight", &pointlight);

			ImGui::ColorEdit3("point light color", reinterpret_cast<float*>(&gContext.pointlight.color));
			ImGui::SliderFloat("pointlight source x", &gContext.pointlight.position[0], -10.0f, 10.0f);
			ImGui::SliderFloat("pointlight source y", &gContext.pointlight.position[1], 0.0f, 100.0f);
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

		if (ImGui::CollapsingHeader("Scene Hierarchy")) {
			ImGui::Text("Scene");
			// Display the list of model names using ImGui::ListBox()
			ImGui::ListBox(".", &sceneSelectionIndex,
				[](void* data, int index, const char** out_text) {
					std::vector<ModelLoader*>* models = (std::vector<ModelLoader*>*)data;
					*out_text = (*models)[index]->Name.c_str();
					return true;
				}, &Models, Models.size());

			ImGui::Indent();

			if (Models.size() != 0 && sceneSelectionIndex != -1) {
				ImGui::Text("Transform");
				if (ImGui::CollapsingHeader("Position"))
				{
					ImGui::SliderFloat("X Position", &Models[sceneSelectionIndex]->Transform.Position.x, -10.0f, 10.0f);
					ImGui::SliderFloat("Y Position", &Models[sceneSelectionIndex]->Transform.Position.y, -10.0f, 10.0f);
					ImGui::SliderFloat("Z Position", &Models[sceneSelectionIndex]->Transform.Position.z, -10.0f, 10.0f);
				}
				if (ImGui::CollapsingHeader("Rotation"))
				{
					ImGui::SliderFloat("X Rotation", &Models[sceneSelectionIndex]->Transform.Rotation.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Y Rotation", &Models[sceneSelectionIndex]->Transform.Rotation.y, 0.0f, 360.0f);
					ImGui::SliderFloat("Z Rotation", &Models[sceneSelectionIndex]->Transform.Rotation.z, 0.0f, 360.0f);
				}
				if (ImGui::CollapsingHeader("Scale"))
				{
					float scale = Models[sceneSelectionIndex]->Transform.Scale.x;
					if (ImGui::SliderFloat("All Scale", &scale, 0.0f, 100.0f, "%.2f", 2.0f))
					{
						Models[sceneSelectionIndex]->Transform.Scale.x = scale;
						Models[sceneSelectionIndex]->Transform.Scale.y = scale;
						Models[sceneSelectionIndex]->Transform.Scale.z = scale;
					}
					ImGui::SliderFloat("X Scale", &Models[sceneSelectionIndex]->Transform.Scale.x, 0.0f, 100.0f);
					ImGui::SliderFloat("Y Scale", &Models[sceneSelectionIndex]->Transform.Scale.y, 0.0f, 100.0f);
					ImGui::SliderFloat("Z Scale", &Models[sceneSelectionIndex]->Transform.Scale.z, 0.0f, 100.0f);
				}

				if (ImGui::Button("Reset"))
				{
					Models[sceneSelectionIndex]->Transform.Position = Vector3::Zero();
					Models[sceneSelectionIndex]->Transform.Rotation = Vector3::Zero();
					Models[sceneSelectionIndex]->Transform.Scale = Vector3::One();
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					DeleteModel(sceneSelectionIndex);
				}
			}
			else {
				// An element is selected
			}
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
			std::string filePath = Constants::saveFileDialog();
			if (!filePath.empty()) {
				saveModels(Models, gContext.pointlight, filePath);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene", buttonSize))
		{
			std::string filePath = Constants::openFileDialog();
			if (!filePath.empty()) {
				loadScene(filePath, Models, gContext.pointlight);
			}
			//Constants::LoadLights(gContext.pointlight);
		}
		ImGui::SameLine();
		if (ImGui::Button("Quit", buttonSize))
		{
			exit(0);
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
	ModelLoader* objLoader = new ModelLoader(name);
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

void GameManager::saveModels(const std::vector<ModelLoader*>& models, const PointLight& pointLight, const std::string& filePath)
{
	std::ofstream file(filePath, std::ios::binary | std::ios::trunc);

	if (!file) {
		throw std::runtime_error("Error opening file for writing");
	}

	// Save models
	// Write the number of models to the file
	size_t numModels = models.size();
	file.write(reinterpret_cast<const char*>(&numModels), sizeof(numModels));

	// Write the name and transform of each model to the file
	for (const auto& model : models) {
		cout << model->Name << endl;
		const std::string& modelName = model->Name;
		const Transform& transform = model->Transform;

		// Write the name of the model
		file.write(modelName.c_str(), modelName.length() + 1);

		// Write the transform of the model
		file.write(reinterpret_cast<const char*>(&transform), sizeof(Transform));
	}

	// Save light properties
	// Write the color of the light
	file.write(reinterpret_cast<const char*>(&pointLight.color), sizeof(pointLight.color));

	// Write the position of the light
	file.write(reinterpret_cast<const char*>(&pointLight.position), sizeof(pointLight.position));

	file.close();
}

void GameManager::loadScene(const std::string& filePath, std::vector<ModelLoader*>& models, PointLight& pointLight)
{
	std::ifstream file(filePath, std::ios::binary);

	if (!file) {
		throw std::runtime_error("Error opening file for reading");
	}

	// Load models
	// Read the number of models from the file
	size_t numModels = 0;
	file.read(reinterpret_cast<char*>(&numModels), sizeof(numModels));

	models.clear();

	// Read the name and transform of each model from the file
	for (size_t i = 0; i < numModels; ++i) {
		// Read the name of the model
		std::string modelName;
		std::getline(file, modelName, '\0');

		// Read the transform of the model
		Transform transform;
		file.read(reinterpret_cast<char*>(&transform), sizeof(Transform));

		// Create a ModelLoader object with the loaded data and add it to the models vector
		ModelLoader* model = new ModelLoader(modelName);
		model->Transform = transform;
		models.push_back(model);
	}

	// Load light properties
	// Read the color of the light
	file.read(reinterpret_cast<char*>(&pointLight.color), sizeof(pointLight.color));

	// Read the position of the light
	file.read(reinterpret_cast<char*>(&pointLight.position), sizeof(pointLight.position));

	file.close();
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
