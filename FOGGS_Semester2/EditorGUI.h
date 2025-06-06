#pragma once
#include <string>
#include "Scene.h"
#include <mutex>
#include <unordered_map>

using namespace std;

class EditorGUI
{
public:
	EditorGUI(Scene& scene);

	void Render();
	void ProgressBar(const char* title);
	void SetLoading(bool loading) { showLoading = loading; }
	void SetLoadingTitle(const char* title) { LoadingTitle = title; }
private:
	void MenuBar();
	void PropertiesWindow();
	void ImportWindow();
	void InspectorWindow();
	void SkyWindow();

	string LoadScene();
	string SaveScene();
	string wstringToString(const std::wstring& wstr);

	void SpawnModel(string name);
	void DeleteModel(int i);

	void ReadFiles(string type);

	void ReadSkies(string type);

	static const std::string NameWithoutExt(std::string n)
	{
		std::string filename = n;

		size_t pos = filename.find_last_of('.');
		if (pos != std::string::npos)
		{
			std::string filenameWithoutExtension = filename.substr(0, pos);
			return filenameWithoutExtension;
		}
		return n;
	}

	Scene& sceneRef;

	std::vector<std::string> files;
	std::vector<std::string> skyboxFiles;
	int importSelectionIndex = 0;
	int sceneSelectionIndex = 0;
	int skySelectionIndex = 0;

	std::string path = "..\\Assets\\Models\\";
	std::string skyPath = "..\\Assets\\Skybox\\";

	bool ScenePropVisibility = false;
	bool ImportWindowVisibility = false;
	bool showLoading = false;
	const char* LoadingTitle = "";

	std::unordered_map<std::string, bool> windowVisibility;
	void ToggleWindowVisibility(const std::string& windowName);
};

void LoadSceneThread(Scene& sceneRef, EditorGUI& editorGUI);

#define InspectorTitle "Inspector"
#define SkyTitle "SkySettings"