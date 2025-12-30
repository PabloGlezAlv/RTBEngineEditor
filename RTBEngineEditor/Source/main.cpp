// RTBEngineEditor - main.cpp
#include "Core/EditorApplication.h"

int main(int argc, char* argv[]) {
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
