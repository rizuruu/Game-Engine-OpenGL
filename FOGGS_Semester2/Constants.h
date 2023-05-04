#pragma once
#include <string>
#include <imgui/imgui.h>
#include <fstream>
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

    static std::string wstringToString(const std::wstring& wstr)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        std::string result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, NULL, NULL);
        return result;
    }

    static std::string openFileDialog()
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

    static std::string saveFileDialog()
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

    void savePixelsToBMP(const char* filename, GLubyte* pixels, int width, int height) {
        std::ofstream outFile(filename, std::ios::out | std::ios::binary);

        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }

        int fileSize = 54 + 3 * width * height;
        int pixelDataSize = 3 * width * height;

        // BMP file header (14 bytes)
        unsigned char fileHeader[14] = {
            'B', 'M',                 // ID field
            0, 0, 0, 0,               // File size
            0, 0,                     // Reserved
            0, 0,                     // Reserved
            54, 0, 0, 0               // Pixel data offset
        };

        // Write the file size to the header
        fileHeader[2] = (unsigned char)(fileSize);
        fileHeader[3] = (unsigned char)(fileSize >> 8);
        fileHeader[4] = (unsigned char)(fileSize >> 16);
        fileHeader[5] = (unsigned char)(fileSize >> 24);

        // BMP info header (40 bytes)
        unsigned char infoHeader[40] = {
            40, 0, 0, 0,              // Header size
            0, 0, 0, 0,               // Image width
            0, 0, 0, 0,               // Image height
            1, 0,                     // Number of color planes
            24, 0,                    // Bits per pixel
            0, 0, 0, 0,               // Compression method
            0, 0, 0, 0,               // Pixel data size
            0, 0, 0, 0,               // Horizontal resolution
            0, 0, 0, 0,               // Vertical resolution
            0, 0, 0, 0,               // Number of colors in the palette
            0, 0, 0, 0                // Number of important colors
        };

        // Write the width and height to the info header
        infoHeader[4] = (unsigned char)(width);
        infoHeader[5] = (unsigned char)(width >> 8);
        infoHeader[6] = (unsigned char)(width >> 16);
        infoHeader[7] = (unsigned char)(width >> 24);
        infoHeader[8] = (unsigned char)(height);
        infoHeader[9] = (unsigned char)(height >> 8);
        infoHeader[10] = (unsigned char)(height >> 16);
        infoHeader[11] = (unsigned char)(height >> 24);

        // Write headers to the file
        outFile.write(reinterpret_cast<char*>(fileHeader), sizeof(fileHeader));
        outFile.write(reinterpret_cast<char*>(infoHeader), sizeof(infoHeader));

        // Write the pixel data to the file (bottom-up)
        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                int index = (i * width + j) * 4;
                outFile.put(pixels[index + 2]); // Blue
                outFile.put(pixels[index + 1]); // Green
                outFile.put(pixels[index]);     // Red
            }

            // Add padding to align rows to 4-byte boundaries
            for (int j = 0; j < (4 - (width * 3) % 4) % 4; j++) {
                outFile.put(0);
            }
        }

        outFile.close();
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