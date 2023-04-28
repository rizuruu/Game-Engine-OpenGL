#pragma once
#include <string>
#include <imgui/imgui.h>

class Constants {
public:
    // Title of the Game/Project/Window
    static constexpr char* GameTitle = "FOGGS Project";

    // Name of the file to save engine data in
    static const std::string& SaveFileName;

    // To check if it has to run it in Editor mode
    static const bool EditorMode = true;

    // Colors
    static const ImVec4 ButtonColor;
    static const ImVec4 ButtonHoverColor;
};
const std::string& Constants::SaveFileName = "EngineSaveData";

// Colors
const ImVec4 Constants::ButtonColor = ImVec4(0.1, 0.1, 0.1, 0.8f);
const ImVec4 Constants::ButtonHoverColor = ImVec4(0.129, 0.129, 0.129, 0.8f);