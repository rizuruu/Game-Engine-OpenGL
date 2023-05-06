#include "EditorGUI.h"
#include "imgui/imgui.h"
#include <windows.h>
#include <commdlg.h>
#include <thread>
#include <Objbase.h>

using namespace std;

EditorGUI::EditorGUI(Scene& scene) : sceneRef(scene)
{
	ReadFiles("obj");
	ReadSkies("png");
}

void EditorGUI::Render()
{
	MenuBar();

	if (ScenePropVisibility)
		PropertiesWindow();
	if (ImportWindowVisibility)
		ImportWindow();

	InspectorWindow();
	SkyWindow();

	if (showLoading)
	{
		ProgressBar(LoadingTitle);
	}
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
				sceneRef.LoadScene("..\\Assets\\Editor\\Default.scene");
			}

			if (ImGui::MenuItem("Load Scene"))
			{
				LoadScene();
				//showLoading = true;
				//std::thread loadingThread(LoadSceneThread, std::ref(sceneRef), std::ref(*this));
				//loadingThread.detach();
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

		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Hierarchy"))
			{
				ScenePropVisibility = true;
			}

			if (ImGui::MenuItem("Import Game Object"))
			{
				ImportWindowVisibility = true;
			}

			if (ImGui::MenuItem("Change Sky"))
			{
				sceneRef.SkyboxRenderer->UpdateSky("..\\Assets\\Textures\\Fantasy.png");
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
	if (ImGui::Begin("My Scene Properties", false))
	{
		ImVec2 verticalSpacing(0.0f, 10.0f);
		ImGui::Dummy(verticalSpacing);

		static bool pointlight = true;
		static bool spotlight = true;
		if (ImGui::CollapsingHeader("Lights"))
		{
			ImGui::SliderFloat("ambient light adjust", &sceneRef.globalAmbient, 0.0f, 1.0f);
			ImGui::Checkbox("enable pointlight", &pointlight);

			ImGui::ColorEdit3("point light color", reinterpret_cast<float*>(&sceneRef.pointlight.color));
			ImGui::SliderFloat("pointlight source x", &sceneRef.pointlight.position[0], -10.0f, 10.0f);
			ImGui::SliderFloat("pointlight source y", &sceneRef.pointlight.position[1], 0.0f, 100.0f);
			ImGui::SliderFloat("pointlight source z", &sceneRef.pointlight.position[2], -10.0f, 10.0f);

			ImGui::Checkbox("enable spotlight", &spotlight);
			ImGui::ColorEdit3("spotlight color", reinterpret_cast<float*>(&sceneRef.spotlight.color));
			ImGui::SliderFloat("spotlight source x", &sceneRef.spotlight.position[0], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight source y", &sceneRef.spotlight.position[1], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight source z", &sceneRef.spotlight.position[2], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight target x", &sceneRef.spotlight.target[0], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight target y", &sceneRef.spotlight.target[1], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight target z", &sceneRef.spotlight.target[2], -10.0f, 10.0f);
			ImGui::SliderFloat("spotlight cutoff", &sceneRef.spotlight.cutoff, 0.0f, 90.0f);
			ImGui::SliderFloat("spotlight exponent", &sceneRef.spotlight.exponent, 0.0f, 90.0f);

			pointlight ? sceneRef.pointlight.enable() : sceneRef.pointlight.disable();
			spotlight ? sceneRef.spotlight.enable() : sceneRef.spotlight.disable();
		}

		if (ImGui::CollapsingHeader("Scene Hierarchy")) {
			ImGui::Text("Scene");
			// Display the list of model names using ImGui::ListBox()
			ImGui::ListBox("##GameObjects", &sceneSelectionIndex,
				[](void* data, int index, const char** out_text) {
					std::vector<ModelLoader*>* models = (std::vector<ModelLoader*>*)data;
					*out_text = (*models)[index]->Name.c_str();
					return true;
				}, &sceneRef.GameObjects, sceneRef.GameObjects.size());
		}

		verticalSpacing = ImVec2(0.0f, 10.0f);
		ImGui::Dummy(verticalSpacing);

		// Set the desired button size
		ImVec2 buttonSize(150, 30);

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::SameLine();
		if (ImGui::Button("Close", buttonSize))
		{
			ScenePropVisibility = false;
		}

		// Reset the style
		ImGui::PopStyleVar();
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();
	}
	ImGui::End();
}

void EditorGUI::ImportWindow()
{
	if (ImGui::Begin("Import Game Objects"))
	{
		float fullWidth = ImGui::GetContentRegionAvailWidth();

		ImGui::Text("Select Model to import:");
		ImGui::SameLine();
		if (ImGui::Button("Refresh"))
		{
			ReadFiles("obj");
		}

		ImGui::ListBox("##list", &importSelectionIndex,
			[](void* data, int index, const char** out_text) {
				*out_text = (*(std::vector<std::string>*)data)[index].c_str();
				return true;
			}, (void*)&files, files.size(), 20);


		ImVec2 fullSizeButton(fullWidth, 30);
		if (ImGui::Button("IMPORT", fullSizeButton))
		{
			SpawnModel(NameWithoutExt(files[importSelectionIndex]));
		}
		if (ImGui::Button("Close", fullSizeButton))
		{
			ImportWindowVisibility = false;
		}
	}
	ImGui::End();
}

void EditorGUI::InspectorWindow()
{
	if (ImGui::Begin("Inspector"))
	{
		if (sceneRef.GameObjects.size() != 0 && sceneSelectionIndex >= 0) {
			ImGui::Text("Transform");
			if (ImGui::CollapsingHeader("Position"))
			{
				ImGui::SliderFloat3("Position", &sceneRef.GameObjects[sceneSelectionIndex]->Transform.Position.x, -10.0f, 10.0f);
			}
			if (ImGui::CollapsingHeader("Rotation"))
			{
				ImGui::SliderFloat3("Rotation", &sceneRef.GameObjects[sceneSelectionIndex]->Transform.Rotation.x, 0.0f, 360.0f);
			}
			if (ImGui::CollapsingHeader("Scale"))
			{
				float scale = sceneRef.GameObjects[sceneSelectionIndex]->Transform.Scale.x;
				if (ImGui::SliderFloat("All Scale", &scale, 0.0f, 100.0f, "%.2f", 2.0f))
				{
					sceneRef.GameObjects[sceneSelectionIndex]->Transform.Scale.x = scale;
					sceneRef.GameObjects[sceneSelectionIndex]->Transform.Scale.y = scale;
					sceneRef.GameObjects[sceneSelectionIndex]->Transform.Scale.z = scale;
				}
				ImGui::SliderFloat3("Scale", &sceneRef.GameObjects[sceneSelectionIndex]->Transform.Scale.x, 0.0f, 100.0f);
			}

			if (ImGui::Button("Reset"))
			{
				sceneRef.GameObjects[sceneSelectionIndex]->Transform.Position = Vector3::Zero();
				sceneRef.GameObjects[sceneSelectionIndex]->Transform.Rotation = Vector3::Zero();
				sceneRef.GameObjects[sceneSelectionIndex]->Transform.Scale = Vector3::One();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				DeleteModel(sceneSelectionIndex);
			}
		}
	}
	ImGui::End();
}

void EditorGUI::SkyWindow()
{
	if (ImGui::Begin("Sky Settings"))
	{
		float fullWidth = ImGui::GetContentRegionAvailWidth();

		ImGui::Text("Select cubemap for sky:");
		ImGui::SameLine();
		if (ImGui::Button("Refresh"))
		{
			ReadSkies("png");
		}

		ImGui::ListBox("##Skies", &skySelectionIndex,
			[](void* data, int index, const char** out_text) {
				*out_text = (*(std::vector<std::string>*)data)[index].c_str();
				return true;
			}, (void*)&skyboxFiles, skyboxFiles.size(), 5);

		ImVec2 buttonSize(fullWidth, 30);
		if (ImGui::Button("APPLY", buttonSize))
		{
			sceneRef.SkyboxRenderer->UpdateSky(skyPath + skyboxFiles[skySelectionIndex]);
		}
		if (ImGui::Button("CLOSE", buttonSize))
		{
			//sceneRef.SkyboxRenderer->UpdateSky(skyPath + skyboxFiles[skySelectionIndex]);
		}
	}
	ImGui::End();
}

void EditorGUI::ProgressBar(const char* title)
{
	// Inside your render loop or function where ImGui is being used
	{
		// Update progress value (replace this with your actual loading status)
		static float progress = 0.0f;
		progress += 0.0004f;
		if (progress > 1.0f) {
			progress = 0.0f;
		}
		// Set the window size and position to be in the center of the screen
		ImVec2 windowSize(300, 100);
		ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f - windowSize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f - windowSize.y * 0.5f);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);

		ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		// Display the loading bar
		ImGui::Text("Loading...");
		ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f)); // -1.0f width makes the progress bar fit the window width

		ImGui::End();
	}
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

void EditorGUI::ReadFiles(string type)
{
	files.clear();
#ifdef _WIN32
	WIN32_FIND_DATAA fileData;
	string filePath = type == "png" ? skyPath : path;
	HANDLE hFind = FindFirstFileA((filePath + "*").c_str(), &fileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string filename = fileData.cFileName;
				if (filename.substr(filename.find_last_of(".") + 1) == type) {
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

void EditorGUI::ReadSkies(string type)
{
	skyboxFiles.clear();
#ifdef _WIN32
	WIN32_FIND_DATAA fileData;
	string filePath = type == "png" ? skyPath : path;
	HANDLE hFind = FindFirstFileA((filePath + "*").c_str(), &fileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string filename = fileData.cFileName;
				if (filename.substr(filename.find_last_of(".") + 1) == type) {
					skyboxFiles.push_back(filename);
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
				skyboxFiles.push_back(filename);
			}
		}
		closedir(dir);
	}
#endif
}


void LoadSceneThread(Scene& sceneRef, EditorGUI& editorGUI)
{
	// Initialize COM on this thread
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize COM on the new thread. Error: " << hr << std::endl;
		return;
	}

	std::cout << "loading" << std::endl;
	editorGUI.SetLoadingTitle("Loading Scene");
	sceneRef.LoadScene();
	editorGUI.SetLoading(false);

	// Uninitialize COM on this thread
	CoUninitialize();
}