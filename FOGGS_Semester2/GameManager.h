#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_freeglut.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL\freeglut.h>
#include "Scene.h"
#include "Skybox.h"
#include <freeglut-3.0.0/src/fg_internal.h>
#include "Skybox.h"
#include <cstdint>
#include "ModelLoader.h"
#include "Player.h"
#include "EditorGUI.h"

using namespace std;
using namespace ImGui;

class GameManager
{
public:
	GameManager(int argc, char** argv);
	~GameManager();

	void Init(int argc, char** argv);
	void Display();
	void Update();
	void Keyboard(unsigned char key, int, int);
	void GameInput(unsigned char key);
	void EditorInput(unsigned char key);
	void Motion(int x, int y);
	void MouseWheel(int wheel, int direction, int x, int y);
private:
	void drawHUD(const char* text, float x, float y);
	void guiInteraction();
	void mouse(int button, int state, int x, int y);
	void drawScene();

	//single point of access to all rendered objects
	Scene gContext;
	float* velocity = new float();
	bool freeLook = true;
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

	Player* RoboPlayer;
	ImFont* largerFont;
	EditorGUI* EditorGUIRenderer;
	const float MIN_PITCH_ANGLE = -89.0f;
	const float MAX_PITCH_ANGLE = 89.0f;
};

