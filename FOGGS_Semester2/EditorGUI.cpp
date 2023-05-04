#include "EditorGUI.h"
#include "imgui/imgui.h"
#include <windows.h>
#include <commdlg.h>

using namespace std;

EditorGUI::EditorGUI(Scene& scene) : sceneRef(scene)
{
	ReadFiles();
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
				sceneRef.LoadScene();
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				sceneRef.SaveScene();
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

void EditorGUI::PropertiesWindow(Scene& gContext)
{
	if (ImGui::Begin("My Scene Properties", false))
	{
		//ImGui::RadioButton("external view", &gContext.isDogView, 0); ImGui::SameLine();
		//ImGui::RadioButton("doggy view", &gContext.isDogView, 1);

		ImGui::Text("Select Model to import:");
		ImGui::SameLine();
		if (ImGui::Button("Refresh"))
		{
			ReadFiles();
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
			//cout << Constants::NameWithoutExt(files[importSelectionIndex]) << endl;

			SpawnModel(NameWithoutExt(files[importSelectionIndex]));
		}

		ImVec2 verticalSpacing(0.0f, 10.0f);
		ImGui::Dummy(verticalSpacing);

		static bool pointlight = true;
		static bool spotlight = true;
		//ImGui::SliderFloat("FOV", &Constants::FOV, 0.0f, 100.0f);
		if (ImGui::Button("Import HDRI", fullSizeButton))
		{
			//string filepath = Constants::openFileDialog();
			//std::cout << filepath << std::endl;
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
				}, &gContext.GameObjects, gContext.GameObjects.size());

			ImGui::Indent();

			if (gContext.GameObjects.size() != 0 && sceneSelectionIndex != -1) {
				ImGui::Text("Transform");
				if (ImGui::CollapsingHeader("Position"))
				{
					float position = gContext.GameObjects[sceneSelectionIndex]->Transform.Position.x;
					ImGui::SliderFloat3("Position", &gContext.GameObjects[sceneSelectionIndex]->Transform.Position.x, -10.0f, 10.0f);
					//ImGui::SliderFloat("Y Position", &gContext.GameObjects[sceneSelectionIndex]->Transform.Position.y, -10.0f, 10.0f);
					//ImGui::SliderFloat("Z Position", &gContext.GameObjects[sceneSelectionIndex]->Transform.Position.z, -10.0f, 10.0f);
				}
				if (ImGui::CollapsingHeader("Rotation"))
				{
					float rotation = gContext.GameObjects[sceneSelectionIndex]->Transform.Rotation.x;
					if (ImGui::SliderFloat("All Rotation", &rotation, 0.0f, 360.0f, "%.2f", 2.0f))
					{
						gContext.GameObjects[sceneSelectionIndex]->Transform.Rotation.x = rotation;
						gContext.GameObjects[sceneSelectionIndex]->Transform.Rotation.y = rotation;
						gContext.GameObjects[sceneSelectionIndex]->Transform.Rotation.z = rotation;
					}
					ImGui::SliderFloat3("Rotation", &gContext.GameObjects[sceneSelectionIndex]->Transform.Rotation.x, 0.0f, 360.0f);
				}
				if (ImGui::CollapsingHeader("Scale"))
				{
					float scale = gContext.GameObjects[sceneSelectionIndex]->Transform.Scale.x;
					if (ImGui::SliderFloat("All Scale", &scale, 0.0f, 100.0f, "%.2f", 2.0f))
					{
						gContext.GameObjects[sceneSelectionIndex]->Transform.Scale.x = scale;
						gContext.GameObjects[sceneSelectionIndex]->Transform.Scale.y = scale;
						gContext.GameObjects[sceneSelectionIndex]->Transform.Scale.z = scale;
					}
					ImGui::SliderFloat3("Scale", &gContext.GameObjects[sceneSelectionIndex]->Transform.Scale.x, 0.0f, 100.0f);
				}

				if (ImGui::Button("Reset"))
				{
					gContext.GameObjects[sceneSelectionIndex]->Transform.Position = Vector3::Zero();
					gContext.GameObjects[sceneSelectionIndex]->Transform.Rotation = Vector3::Zero();
					gContext.GameObjects[sceneSelectionIndex]->Transform.Scale = Vector3::One();
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
		ImGui::SameLine();
		if (ImGui::Button("Close", buttonSize))
		{
			exit(0);
		}

		// Reset the style
		ImGui::PopStyleVar();
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();
	}
	ImGui::End();
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

void EditorGUI::SpawnModel(string name)
{
	ModelLoader* objLoader = new ModelLoader(name);
	//bool loaded = objLoader->loadObj(name);
	//if (loaded)
	sceneRef.GameObjects.push_back(objLoader);
}

void EditorGUI::DeleteModel(int i)
{
	ModelLoader* obj = sceneRef.GameObjects[i];
	sceneRef.GameObjects.erase(sceneRef.GameObjects.begin() + i);
	delete obj;
}

void EditorGUI::ReadFiles()
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