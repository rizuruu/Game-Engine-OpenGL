#pragma once
#include <string>
#include "Scene.h"
using namespace std;

class EditorGUI
{
public:
	EditorGUI(Scene& scene);

	void Render();
	void MenuBar();
	void PropertiesWindow(Scene& gContext);

private:
	string LoadScene();
	string SaveScene();
	string wstringToString(const std::wstring& wstr);

	void SpawnModel(string name);
	void DeleteModel(int i);

	void ReadFiles();

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
	int importSelectionIndex = 0;
	int sceneSelectionIndex = 0;

	std::string path = "..\\Assets\\Models\\";
};

