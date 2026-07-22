#include "Project.h"
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Physics/PhysicsLayerSettings.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>

namespace RTBEditor {

    Project* Project::activeProject = nullptr;

    namespace {
        std::string NormalizeProjectPath(const std::string& path) {
            return std::filesystem::path(path).lexically_normal().generic_string();
        }

        std::string Trim(const std::string& value) {
            const auto first = value.find_first_not_of(" \t\r\n");
            if (first == std::string::npos) {
                return {};
            }
            const auto last = value.find_last_not_of(" \t\r\n");
            return value.substr(first, last - first + 1);
        }
    }

    Project::Project() {}

    Project::~Project() {
        if (activeProject == this) {
            activeProject = nullptr;
        }
    }

    RTBEngine::Rendering::RHI::GraphicsAPI Project::ParseGraphicsAPI(const std::string& value)
    {
        const std::string normalized = Trim(value);
        if (_stricmp(normalized.c_str(), "Vulkan") == 0 || _stricmp(normalized.c_str(), "vk") == 0) {
            return RTBEngine::Rendering::RHI::GraphicsAPI::Vulkan;
        }
        return RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
    }

    const char* Project::GraphicsAPIToProjectString(RTBEngine::Rendering::RHI::GraphicsAPI api)
    {
        return RTBEngine::Rendering::RHI::GraphicsAPIToString(api);
    }

    RTBEngine::Rendering::RHI::GraphicsAPI Project::PeekGraphicsAPI(
        const std::filesystem::path& path,
        RTBEngine::Rendering::RHI::GraphicsAPI fallback)
    {
        std::ifstream file(path);
        if (!file.is_open()) {
            return fallback;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (!std::getline(is_line, key, '=')) {
                continue;
            }
            std::string value;
            if (!std::getline(is_line, value)) {
                continue;
            }
            if (Trim(key) == "GraphicsAPI") {
                return ParseGraphicsAPI(value);
            }
        }
        return fallback;
    }

    std::filesystem::path Project::ResolveDefaultProjectFile(const std::filesystem::path& projectFileName)
    {
        const std::filesystem::path currentDir = std::filesystem::current_path();
        std::filesystem::path bestCandidate;
        int bestScore = -1;

        auto scoreDir = [](const std::filesystem::path& dir) -> int {
            // Never treat build output folders as the project root (avoids
            // ...\x64\Debug\x64\Debug\GameScripts.dll when cwd is the exe dir).
            for (const auto& part : dir) {
                const std::string name = part.string();
                if (_stricmp(name.c_str(), "x64") == 0
                    || _stricmp(name.c_str(), "Debug") == 0
                    || _stricmp(name.c_str(), "Release") == 0) {
                    // Still allow if the folder itself clearly is the editor project.
                }
            }
            if (dir.filename() == "Debug" || dir.filename() == "Release") {
                const std::string parentName = dir.parent_path().filename().string();
                if (_stricmp(parentName.c_str(), "x64") == 0) {
                    return -1;
                }
            }

            int score = 0;
            if (std::filesystem::exists(dir / "RTBEngineEditor.vcxproj")) score += 100;
            if (std::filesystem::exists(dir / "Source")) score += 50;
            if (std::filesystem::exists(dir / "Assets")) score += 10;
            if (std::filesystem::exists(dir / "GameScripts")) score += 20;
            return score;
        };

        for (std::filesystem::path dir = currentDir;; dir = dir.parent_path()) {
            const std::filesystem::path candidate = (dir / projectFileName).lexically_normal();
            if (std::filesystem::exists(candidate)) {
                const int score = scoreDir(dir);
                if (score > bestScore) {
                    bestScore = score;
                    bestCandidate = candidate;
                }
            }

            const std::filesystem::path parent = dir.parent_path();
            if (parent == dir) {
                break;
            }
        }

        if (bestScore > 0 && !bestCandidate.empty()) {
            return bestCandidate;
        }

        const std::filesystem::path localCandidate = (currentDir / projectFileName).lexically_normal();
        if (std::filesystem::exists(localCandidate)) {
            return localCandidate;
        }

        return std::filesystem::absolute(projectFileName).lexically_normal();
    }

    bool Project::Load(const std::filesystem::path& path) {
        const std::filesystem::path absolutePath = std::filesystem::absolute(path).lexically_normal();
        std::ifstream file(absolutePath);
        if (!file.is_open()) {
            RTB_ERROR("Failed to open project file: " + absolutePath.string());
            return false;
        }

        name = "New Project";
        startScene = "Assets/Scenes/DefaultScene.lua";
        lastOpenScene.clear();
        graphicsAPI = RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
        projectFilePath = absolutePath;
        projectDirectory = projectFilePath.parent_path();
        assetDirectory = "Assets";

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    if (key == "Name") name = value;
                    else if (key == "StartScene") startScene = NormalizeProjectPath(value);
                    else if (key == "LastOpenScene") lastOpenScene = NormalizeProjectPath(value);
                    else if (key == "AssetDirectory") assetDirectory = std::filesystem::path(value);
                    else if (key == "GraphicsAPI") graphicsAPI = ParseGraphicsAPI(value);
                }
            }
        }

        activeProject = this;

        const std::filesystem::path physicsLayersPath =
            projectDirectory / RTBEngine::Physics::PhysicsLayerSettings::GetDefaultSettingsFileName();
        auto& physicsLayers = RTBEngine::Physics::PhysicsLayerSettings::Get();
        if (!physicsLayers.LoadFromFile(physicsLayersPath)) {
            physicsLayers.ResetToDefaults();
            physicsLayers.SaveToFile(physicsLayersPath);
        }

        return true;
    }

    void Project::SetStartScene(const std::string& path) {
        startScene = NormalizeProjectPath(path);
    }

    void Project::SetLastOpenScene(const std::string& path) {
        lastOpenScene = NormalizeProjectPath(path);
    }

    bool Project::Save(const std::filesystem::path& path) {
        std::filesystem::path savePath = path.empty() ? projectFilePath : std::filesystem::absolute(path).lexically_normal();
        if (savePath.empty()) {
            return false;
        }

        std::ofstream file(savePath);
        if (!file.is_open()) return false;

        file << "Name=" << name << "\n";
        file << "StartScene=" << startScene << "\n";
        if (!lastOpenScene.empty())
            file << "LastOpenScene=" << lastOpenScene << "\n";
        file << "AssetDirectory=" << assetDirectory.string() << "\n";
        file << "GraphicsAPI=" << GraphicsAPIToProjectString(graphicsAPI) << "\n";

        projectFilePath = savePath;
        projectDirectory = projectFilePath.parent_path();

        return true;
    }

    std::filesystem::path Project::GetAssetRootPath() const {
        if (projectDirectory.empty()) {
            return assetDirectory;
        }
        return (projectDirectory / assetDirectory).lexically_normal();
    }

    std::filesystem::path Project::ResolveAssetPath(const std::filesystem::path& relativePath) const {
        return (GetAssetRootPath() / relativePath).lexically_normal();
    }

    std::string Project::GetAssetReferencePath(const std::filesystem::path& relativePath) const {
        return (assetDirectory / relativePath).lexically_normal().generic_string();
    }

    std::filesystem::path Project::GetGameScriptsProjectPath() const {
        if (projectDirectory.empty()) {
            return {};
        }
        return (projectDirectory / "GameScripts" / "GameScripts.vcxproj").lexically_normal();
    }

    std::filesystem::path Project::GetGameScriptsDllPath(const std::string& configuration) const {
        if (projectDirectory.empty()) {
            return {};
        }
        return (projectDirectory / "x64" / configuration / "GameScripts.dll").lexically_normal();
    }

    std::filesystem::path Project::GetSDKPath() const {
        if (projectDirectory.empty()) {
            return {};
        }
        return (projectDirectory / "RTBEngine_SDK").lexically_normal();
    }

}
