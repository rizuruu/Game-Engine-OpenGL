#pragma once
#include "Dog.h"
#include "Floor.h"
#include "PointLight.h"
#include "Spotlight.h"
#include "Table.h"
#include "Teapot.h"
#include "Snowman.h"
#include "Camera.h"
#include "Art.h"
#include "Mirror.h"
#include "Walls.h"
#include "ModelLoader.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <commdlg.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

using namespace std;
/*
The context holds all rendered objects, a single point to access the scene objects.
*/
class Scene {
public:
	GLfloat globalAmbient = 0.3f;
	int isDogView = 1;
	Camera camera;
	Floor floor;
	PointLight pointlight;
	SpotLight spotlight;
	Mirror mirror;
	std::vector<ModelLoader*> GameObjects;
	
	void SaveScene()
	{
		string filePath = saveFileDialog();
		std::ofstream file(filePath, std::ios::binary | std::ios::trunc);

		if (!file) {
			throw std::runtime_error("Error opening file for writing");
		}

		// Save models
		// Write the number of models to the file
		size_t numModels = GameObjects.size();
		file.write(reinterpret_cast<const char*>(&numModels), sizeof(numModels));

		// Write the name and transform of each model to the file
		for (const auto& model : GameObjects) {
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
		file.write(reinterpret_cast<const char*>(pointlight.color), sizeof(pointlight.color));

		// Write the position of the light
		file.write(reinterpret_cast<const char*>(pointlight.position), sizeof(pointlight.position));

		// Write global light
		file.write(reinterpret_cast<const char*>(&globalAmbient), sizeof(globalAmbient));

		file.close();
	}

	void LoadScene()
	{
		string filePath = openFileDialog();
		std::ifstream file(filePath, std::ios::binary);

		if (!file) {
			throw std::runtime_error("Error opening file for reading");
		}

		// Load models
		// Read the number of models from the file
		size_t numModels = 0;
		file.read(reinterpret_cast<char*>(&numModels), sizeof(numModels));

		GameObjects.clear();

		// Read the name and transform of each model from the file
		for (size_t i = 0; i < numModels; ++i) {
			// Read the name of the model
			std::string modelName;
			std::getline(file, modelName, '\0');

			// Read the transform of the model
			Transform transform;
			file.read(reinterpret_cast<char*>(&transform), sizeof(Transform));

			// Create a ModelLoader object with the loaded data and add it to the models vector
			ModelLoader* model = new ModelLoader(extractFileName(modelName));
			model->Transform = transform;
			GameObjects.push_back(model);
		}

		// Load light properties
		// Read the color of the light
		file.read(reinterpret_cast<char*>(pointlight.color), sizeof(pointlight.color));

		// Read the position of the light
		file.read(reinterpret_cast<char*>(&pointlight.position), sizeof(pointlight.position));
		file.read(reinterpret_cast<char*>(&globalAmbient), sizeof(globalAmbient));

		file.close();
	}

	string openFileDialog()
	{
		fs::path currentPath = fs::current_path();

		OPENFILENAME ofn;
		wchar_t fileName[MAX_PATH] = L"";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL; // Set this to your application's window handle if available
		ofn.lpstrFilter = L"Scene Files\0*.scene\0All Files\0*.*\0";
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = L"scene";

		if (GetOpenFileName(&ofn))
		{
			fs::current_path(currentPath);
			return wstringToString(fileName);
		}
		else
		{
			fs::current_path(currentPath);
			return "";
		}
	}

	string saveFileDialog()
	{
		// Save the current working directory
		fs::path currentPath = fs::current_path();
		OPENFILENAME ofn;
		wchar_t fileName[MAX_PATH] = L"";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL; // Set this to your application's window handle if available
		ofn.lpstrFilter = L"Scene Files\0*.scene\0All Files\0*.*\0";
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = L"scene";

		if (GetSaveFileName(&ofn))
		{
			fs::current_path(currentPath);
			return wstringToString(fileName);
		}
		else
		{
			fs::current_path(currentPath);
			return "";
		}
	}

	string wstringToString(const std::wstring& wstr)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		std::string result(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, NULL, NULL);
		return result;
	}

	std::string extractFileName(const std::string& filePath) {
		size_t lastSlash = filePath.find_last_of("/\\");
		size_t lastDot = filePath.find_last_of('.');

		if (lastSlash == std::string::npos) {
			lastSlash = 0; // No directory path found, start from the beginning
		}
		else {
			lastSlash += 1; // Move past the last slash
		}

		if (lastDot == std::string::npos || lastDot < lastSlash) {
			lastDot = filePath.size(); // No file extension found or before the last slash, use the whole string
		}

		return filePath.substr(lastSlash, lastDot - lastSlash);
	}
};