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
            return false;
        }

        engineApp->SetIsRunning(true);

        // Initialize UI Layer
        uiLayer = std::make_unique<EditorLayer>();
        uiLayer->Initialize(engineApp->GetWindow()->GetSDLWindow());

        // Set up menu bar callbacks
        uiLayer->GetMenuBar()->SetExitCallback([this]() {
            isRunning = false;
        });

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

        if (uiLayer) {
            uiLayer->Begin();
            uiLayer->OnUIRender();
            uiLayer->End();
        }

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
