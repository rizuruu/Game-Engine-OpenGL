#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_freeglut.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL\freeglut.h>
#include "Context.h"
#include "Skybox.h"
#include <freeglut-3.0.0/src/fg_internal.h>
#include "Skybox.h"
#include <cstdint>
#include "ModelLoader.h"

using namespace std;
using namespace ImGui;

class GameManager
{
public:
	GameManager(int argc, char** argv);
	~GameManager();

	void keyboard(unsigned char key, int, int);
	void Display();
	void Init(int argc, char** argv);
	void Motion(int x, int y);
	void MouseWheel(int wheel, int direction, int x, int y);
private:
	void SpawnModel(string name);
	void DeleteModel(int i);
	void saveModels(const std::vector<ModelLoader*>& models);
	void loadModels();
	void drawHUD(const char* text, float x, float y);
	void guiInteraction();
	void mouse(int button, int state, int x, int y);
	void drawScene();
	void readFiles();

	//single point of access to all rendered objects
	Context gContext;
	Skybox* SkyboxRenderer;
	float* velocity = new float();
	bool freeLook = true;
	vector<ModelLoader*> Models;
	char inputText[256] = { 0 };
	ImFont* customFont = nullptr;

	// paths for the skybox textures
	const char* skyboxFilenames[6] = {
		"..\\Assets\\Skybox\\left.bmp",
		"..\\Assets\\Skybox\\right.bmp",
		"..\\Assets\\Skybox\\bottom.bmp",
		"..\\Assets\\Skybox\\top.bmp",
		"..\\Assets\\Skybox\\back.bmp",
		"..\\Assets\\Skybox\\front.bmp"
	};

	float pitch = 0.0f;
	float yaw = 0.0f;

	int lastX, lastY;

	float lastTargetX = 0;
	float lastTargetY = 0;
	float lastTargetZ = 0;

	float freeLookSpeed = 0.1f;

	std::string path = "..\\Assets\\Models\\";

	std::vector<std::string> files;
	int importSelectionIndex = 0;
	int sceneSelectionIndex = 0;
};

