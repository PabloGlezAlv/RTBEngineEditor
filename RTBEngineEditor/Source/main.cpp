#include "Core/EditorApplication.h"
#include "Core/EditorOnlineSettings.h"
#include "Project/Project.h"

#include <RTBEngine/Rendering/RHI/GraphicsAPI.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <Windows.h>

namespace {

    void ConfigureEditorOnline(RTBEngine::Core::ApplicationConfig& config)
    {
        const RTBEditor::EditorOnlineSettings settings = RTBEditor::EditorOnlineSettingsStore::Load();
        RTBEditor::EditorOnlineSettingsStore::ApplyToOnlineConfig(settings, config.online);
    }

    void ConfigureGraphicsAPI(RTBEngine::Core::ApplicationConfig& config)
    {
        const std::filesystem::path projectPath =
            RTBEditor::Project::ResolveDefaultProjectFile("MyProject.rtbproj");
        config.rendering.graphicsAPI = RTBEditor::Project::PeekGraphicsAPI(projectPath);

        // Optional override for smoke tests / CI: RTB_GRAPHICS_API=Vulkan|OpenGL
        if (const char* graphicsApi = std::getenv("RTB_GRAPHICS_API")) {
            if (_stricmp(graphicsApi, "Vulkan") == 0 || _stricmp(graphicsApi, "vk") == 0) {
                config.rendering.graphicsAPI = RTBEngine::Rendering::RHI::GraphicsAPI::Vulkan;
            }
            else if (_stricmp(graphicsApi, "OpenGL") == 0 || _stricmp(graphicsApi, "gl") == 0) {
                config.rendering.graphicsAPI = RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
            }
        }

        if (config.rendering.graphicsAPI == RTBEngine::Rendering::RHI::GraphicsAPI::Vulkan) {
            config.window.title = "RTBEngine - Editor (Vulkan)";
        }
        else {
            config.window.title = "RTBEngine - Editor Mode";
        }
    }

}

int main(int argc, char* argv[]) {
    char executablePath[MAX_PATH] = {};
    DWORD pathLength = GetModuleFileNameA(nullptr, executablePath, MAX_PATH);
    if (pathLength > 0 && pathLength < MAX_PATH) {
        std::filesystem::current_path(std::filesystem::path(executablePath).parent_path());
    }

    RTBEngine::Core::ApplicationConfig config;
    config.window.title = "RTBEngine - Editor Mode";
    config.window.width = 1600;
    config.window.height = 900;
    config.window.maximized = true;
    config.initialScenePath = ""; // Start with empty scene
    ConfigureEditorOnline(config);
    ConfigureGraphicsAPI(config);

    RTBEditor::EditorApplication editor;
    if (editor.Initialize(config)) {
        editor.Run();
    }

    return 0;
}
