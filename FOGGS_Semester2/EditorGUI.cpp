#include "EditorGUI.h"
#include "imgui/imgui.h"
//#include "Constants.h"
#include <windows.h>
#include <commdlg.h>

using namespace std;

EditorGUI::EditorGUI()
{
}

void EditorGUI::Render()
{
	MenuBar();
}

void EditorGUI::MenuBar()
{
	// Begin a new window for the menu bar
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2));

	// Change the menu bar window background color and border size
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("MenuBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				// Handle "New" option click
			}

			if (ImGui::MenuItem("Load Scene"))
			{
				std::string filePath = LoadScene();
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				std::string filePath = SaveScene();
			}
			if (ImGui::MenuItem("Exit"))
			{
				exit(0);
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Edit")) {
			// Handle "Edit" menu item click
		}

		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("Properties"))
			{
				// Handle "New" option click
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// End the menu bar window
	ImGui::End();

	// Restore the original background color and border size
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void EditorGUI::PropertiesWindow()
{

}

string EditorGUI::LoadScene()
{
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
		return wstringToString(fileName);
	}
	else
	{
		return "";
	}
}

string EditorGUI::SaveScene()
{
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
		return wstringToString(fileName);
	}
	else
	{
		return "";
	}
}

string EditorGUI::wstringToString(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string result(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, NULL, NULL);
	return result;
}
