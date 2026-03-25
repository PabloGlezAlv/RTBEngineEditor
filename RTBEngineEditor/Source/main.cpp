// RTBEngineEditor - main.cpp
#include "Core/EditorApplication.h"
#include <filesystem>
#include <Windows.h>

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

    RTBEditor::EditorApplication editor;
    if (editor.Initialize(config)) {
        editor.Run();
    }

    return 0;
}
