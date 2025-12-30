#include "EditorApplication.h"
#include <GL/glew.h>
#include <iostream>

namespace RTBEditor {

    EditorApplication::EditorApplication() {}

    EditorApplication::~EditorApplication() {
        Shutdown();
    }

    bool EditorApplication::Initialize(const RTBEngine::Core::ApplicationConfig& config) {
        engineApp = std::make_unique<RTBEngine::Core::Application>(config);
        
        if (!engineApp->Initialize()) {
            std::cerr << "[Editor] Failed to initialize engine application" << std::endl;
            return false;
        }

        engineApp->SetIsRunning(true);

        isRunning = true;
        return true;
    }

    void EditorApplication::Run() {
        float lastTime = (float)SDL_GetTicks() / 1000.0f;

        while (isRunning && engineApp->IsRunning()) {
            float currentTime = (float)SDL_GetTicks() / 1000.0f;
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            
            engineApp->ProcessInput();
            if (engineApp->GetWindow()->GetShouldClose()) {
                std::cout << "[Editor] Window close requested." << std::endl;
                isRunning = false;
            }

            Update(deltaTime);

            Render();
        }
    }

    void EditorApplication::Update(float deltaTime) {
        
    }

    void EditorApplication::Render() {
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        engineApp->GetWindow()->SwapBuffers();
    }

    void EditorApplication::Shutdown() {
        if (engineApp) {
            engineApp->Shutdown();
            engineApp.reset();
        }
        isRunning = false;
    }
}
