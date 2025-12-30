#include "EditorApplication.h"
#include <GL/glew.h>
#include <iostream>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Rendering/FrameBuffer.h>
#include "../UI/Panels/SceneViewPanel.h"

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

        // Load Project Settings
        project = std::make_unique<Project>();
        if (project->Load("MyProject.rtbproj")) {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(project->GetStartScene());
        }

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

            RTBEngine::Input::InputManager::GetInstance().Update();
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
        // Render scene to Scene View Panel framebuffer
        RenderSceneToFramebuffer();

        // Clear the main window
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render editor UI
        if (uiLayer) {
            uiLayer->Begin();
            uiLayer->OnUIRender();
            uiLayer->End();
        }

        engineApp->GetWindow()->SwapBuffers();
    }

    void EditorApplication::RenderSceneToFramebuffer() {
        SceneViewPanel* sceneView = uiLayer ? uiLayer->GetSceneViewPanel() : nullptr;
        if (!sceneView) return;

        RTBEngine::Rendering::Framebuffer* framebuffer = sceneView->GetFramebuffer();
        RTBEngine::Rendering::Camera* editorCamera = sceneView->GetEditorCamera();
        
        if (!framebuffer || !editorCamera) return;
        
        int vpWidth = sceneView->GetViewportWidth();
        int vpHeight = sceneView->GetViewportHeight();
        if (vpWidth <= 0 || vpHeight <= 0) return;

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) return;

        // Bind framebuffer and set viewport
        framebuffer->Bind();
        glViewport(0, 0, vpWidth, vpHeight);
        
        // Render shadows first (this might change FBO and viewport)
        engineApp->RenderShadowPass(scene);

        // CRITICAL: Rebind our framebuffer and restore viewport because RenderShadowPass unbinds to 0
        framebuffer->Bind();
        glViewport(0, 0, vpWidth, vpHeight);
        
        // Render the scene geometry
        engineApp->RenderGeometryPass(scene, editorCamera);

        // Unbind framebuffer and restore main viewport
        framebuffer->Unbind();
        glViewport(0, 0, engineApp->GetWindow()->GetWidth(), engineApp->GetWindow()->GetHeight());
    }

    void EditorApplication::Shutdown() {
        if (engineApp) {
            engineApp->Shutdown();
            engineApp.reset();
        }
        isRunning = false;
    }
}
