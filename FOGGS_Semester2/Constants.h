#pragma once
#include <string>
#include <imgui/imgui.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <commdlg.h>

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
    static const bool EditorMode = false;

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

    static std::string wstringToString(const std::wstring& wstr)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        std::string result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, NULL, NULL);
        return result;
    }

    static void ModernDarkTheme()
    {
        auto& style = ImGui::GetStyle();
        style.ChildRounding = 0;
        style.GrabRounding = 0;
        style.FrameRounding = 2;
        style.PopupRounding = 0;
        style.ScrollbarRounding = 0;
        style.WindowRounding = 0;
        style.FramePadding = { 4, 4 };

        style.WindowTitleAlign = { 0.0, 0.5 };

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = { 0.7f, 0.7f, 0.7f, 1.0f };				//
        colors[ImGuiCol_TextDisabled] = { 0.25f, 0.25f, 0.25f, 1.00f };		//
        colors[ImGuiCol_WindowBg] = { 0.09f, 0.09f, 0.09f, 0.94f };			//
        colors[ImGuiCol_ChildBg] = { 0.11f, 0.11f, 0.11f, 1.00f };			//
        colors[ImGuiCol_PopupBg] = { 0.11f, 0.11f, 0.11f, 0.94f };			//
        colors[ImGuiCol_Border] = { 0.07f, 0.08f, 0.08f, 1.00f };
        colors[ImGuiCol_BorderShadow] = { 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_FrameBg] = { 0.35f, 0.35f, 0.35f, 0.54f };			//
        colors[ImGuiCol_FrameBgHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
        colors[ImGuiCol_FrameBgActive] = { 0.40f, 0.36f, 0.33f, 0.67f };
        colors[ImGuiCol_TitleBg] = { 0.1f, 0.1f, 0.1f, 1.00f };
        colors[ImGuiCol_TitleBgActive] = { 0.3f, 0.3f, 0.3f, 1.00f };		//
        colors[ImGuiCol_TitleBgCollapsed] = { 0.0f, 0.0f, 0.0f, 0.61f };
        colors[ImGuiCol_MenuBarBg] = { 0.18f, 0.18f, 0.18f, 0.94f };		//
        colors[ImGuiCol_ScrollbarBg] = { 0.00f, 0.00f, 0.00f, 0.16f };
        colors[ImGuiCol_ScrollbarGrab] = { 0.24f, 0.22f, 0.21f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
        colors[ImGuiCol_CheckMark] = { 0.84f, 0.84f, 0.84f, 1.0f };			//
        colors[ImGuiCol_SliderGrab] = { 0.8f, 0.8f, 0.8f, 1.0f };			//		
        colors[ImGuiCol_SliderGrabActive] = { 0.55f, 0.55f, 0.55f, 1.00f }; //
        colors[ImGuiCol_Button] = { 0.55f, 0.55f, 0.55f, 0.40f };			//
        colors[ImGuiCol_ButtonHovered] = { 0.15f, 0.15f, 0.15f, 0.62f };	//	
        colors[ImGuiCol_ButtonActive] = { 0.60f, 0.60f, 0.60f, 1.00f };		//
        colors[ImGuiCol_Header] = { 0.84f, 0.36f, 0.05f, 0.0f };			//
        colors[ImGuiCol_HeaderHovered] = { 0.42f, 0.42f, 0.42f, 1.00f };	//
        colors[ImGuiCol_HeaderActive] = { 0.25f, 0.25f, 0.25f, 0.80f };
        colors[ImGuiCol_Separator] = { 0.35f, 0.35f, 0.35f, 0.50f };		//
        colors[ImGuiCol_SeparatorHovered] = { 0.31f, 0.29f, 0.27f, 0.78f };
        colors[ImGuiCol_SeparatorActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
        colors[ImGuiCol_ResizeGrip] = { 1.0f, 1.0f, 1.0f, 0.25f };			//
        colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.0f, 1.0f, 0.4f };	//
        colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.0f, 0.95f };	//
        colors[ImGuiCol_PlotLines] = { 0.66f, 0.60f, 0.52f, 1.00f };
        colors[ImGuiCol_PlotLinesHovered] = { 0.98f, 0.29f, 0.20f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = { 0.60f, 0.59f, 0.10f, 1.00f };
        colors[ImGuiCol_PlotHistogramHovered] = { 0.72f, 0.73f, 0.15f, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = { 0.27f, 0.52f, 0.53f, 0.35f };
        colors[ImGuiCol_DragDropTarget] = { 0.60f, 0.59f, 0.10f, 0.90f };
        colors[ImGuiCol_NavHighlight] = { 0.51f, 0.65f, 0.60f, 1.00f };
        colors[ImGuiCol_NavWindowingHighlight] = { 1.00f, 1.00f, 1.00f, 0.70f };
        colors[ImGuiCol_NavWindowingDimBg] = { 0.80f, 0.80f, 0.80f, 0.20f };
        colors[ImGuiCol_ModalWindowDimBg] = { 0.11f, 0.13f, 0.13f, 0.35f };
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