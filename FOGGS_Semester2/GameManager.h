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
#include "ObjLoader.h"
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
private:
	void SpawnModel(string name);
	void DeleteModel(int i);
	void saveModels(const std::vector<ObjLoader*>& models);
	std::vector<ObjLoader*> loadModels();
	void drawHUD(const std::string& text);
	void guiInteraction();
	void OnSliderValueChanged(float* pValue);
	void mouse(int button, int state, int x, int y);
	void drawScene();

	//single point of access to all rendered objects
	Context gContext;
	Skybox* SkyboxRenderer;
	float* velocity = new float();
	bool isEditor = true;
	vector<ModelLoader*> Models;
	char inputText[256] = { 0 };
	ImFont* customFont = nullptr;

	// Define the file paths for the skybox textures
	const char* skyboxFilenames[6] = {
		"..\\Assets\\left.bmp",
		"..\\Assets\\right.bmp",
		"..\\Assets\\bottom.bmp",
		"..\\Assets\\top.bmp",
		"..\\Assets\\back.bmp",
		"..\\Assets\\front.bmp"
	};

	float pitch = 0.0f;
	float yaw = 0.0f;

	int lastX, lastY;
};

