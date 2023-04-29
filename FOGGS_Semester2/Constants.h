#pragma once
#include <string>
#include <imgui/imgui.h>

class Constants {
public:
    // Title of the Game/Project/Window
    static constexpr char* GameTitle = "FOGGS Project";

    // Name of the file to save engine data in
    static const std::string& SaveFileName;

    // Camera Constants
    static const float FOV;
    static const float NearClippingPlane;
    static const float FarClippingPlane;

    // To check if it has to run it in Editor mode
    static const bool EditorMode = true;

    // Colors
    static const ImVec4 ButtonColor;
    static const ImVec4 ButtonHoverColor;

    // Paths
    static const std::string GetModelPath(std::string Name)
    {
        return "..\\Assets\\Models\\" + Name + ".obj";
    }
    static const std::string GetTexturePath(std::string Name)
    {
        return "..\\Assets\\Textures\\" + Name + ".bmp";
    }

    static const char* OrderNaming(int i, const std::string& n) {
        static std::string s; // Make 's' static so it persists after the function scope ends
        s = "(" + std::to_string(i) + ") " + n;
        return s.c_str();
    }

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
};
const std::string& Constants::SaveFileName = "EngineSaveData";

// Colors
const ImVec4 Constants::ButtonColor = ImVec4(0.1, 0.1, 0.1, 0.8f);
const ImVec4 Constants::ButtonHoverColor = ImVec4(0.129, 0.129, 0.129, 0.8f);

// Camera Constants
const float Constants::FOV = 40.0f;
const float Constants::NearClippingPlane = 1.0f;
const float Constants::FarClippingPlane = 3500.0f;