#pragma once
#include <string>
using namespace std;

class EditorGUI
{
public:
	EditorGUI();

	void Render();
	void MenuBar();
	void PropertiesWindow();
	string LoadScene();
	string SaveScene();
	string wstringToString(const std::wstring& wstr);
};

