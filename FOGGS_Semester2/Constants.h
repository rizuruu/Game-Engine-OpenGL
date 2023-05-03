#pragma once
#include <string>
#include <imgui/imgui.h>
#include <fstream>

class Constants {
public:
    // Title of the Game/Project/Window
    static constexpr char* GameTitle = "FOGGS Project";

    // Name of the file to save engine data in
    static const std::string& SaveFileName;

    // Camera Constants
    static float FOV;
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

    template<typename T>
    static const char* Combine(const std::string& st, T var)
    {
        static std::string text;

        std::ostringstream stream;

        if constexpr (std::is_same_v<T, bool>) {
            stream << st << " " << (var ? "true" : "false");
        }
        else if constexpr (std::is_floating_point_v<T>) {
            stream << st << " " << std::fixed << std::setprecision(2) << var;
        }
        else {
            stream << var;
        }

        text = stream.str(); 

        return text.c_str();
    }

    // Saving
    static bool SaveLights(const PointLight& pointLight)
    {
        std::ofstream file("SceneLightsData", std::ios::binary | std::ios::trunc);

        if (!file) {
            std::cerr << "Error opening file for writing" << std::endl;
            return false;
        }

        // Write the color of the light
        file.write(reinterpret_cast<const char*>(&pointLight.color), sizeof(pointLight.color));

        // Write the position of the light
        file.write(reinterpret_cast<const char*>(&pointLight.position), sizeof(pointLight.position));

        file.close();
        return true;
    }
    static bool LoadLights(PointLight& pointLight)
    {
        std::ifstream file("SceneLightsData", std::ios::binary);

        if (!file) {
            std::cerr << "Error opening file for reading" << std::endl;
            return false;
        }

        // Read the color of the light
        file.read(reinterpret_cast<char*>(&pointLight.color), sizeof(pointLight.color));

        // Read the position of the light
        file.read(reinterpret_cast<char*>(&pointLight.position), sizeof(pointLight.position));

        file.close();
        return true;
    }
};

const std::string& Constants::SaveFileName = "EngineSaveData";

// Colors
const ImVec4 Constants::ButtonColor = ImVec4(0.1, 0.1, 0.1, 0.8f);
const ImVec4 Constants::ButtonHoverColor = ImVec4(0.129, 0.129, 0.129, 0.8f);

// Camera Constants
 float Constants::FOV = 40.0f;
const float Constants::NearClippingPlane = 1.0f;
const float Constants::FarClippingPlane = 13500.0f;