#include "EditorApplication.h"
#include <imgui.h>
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

        uiLayer->SetupToolbar(
            [this]() { OnPlay(); },
            [this]() { OnPause(); },
            [this]() { OnStop(); },
            [this]() { return GetState(); }
        );

        // Set up menu bar callbacks
        uiLayer->GetMenuBar()->SetBuildCallback([this]() {
            uiLayer->OpenBuildDialog();
        });

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
        if (state == EditorState::Play) {
            engineApp->Update(deltaTime);
        }
    }

    void EditorApplication::OnPlay() {
        state = EditorState::Play;
        // Focus the Game window when playing
        ImGui::SetWindowFocus("Game");
    }

    void EditorApplication::OnPause() {
        state = EditorState::Pause;
    }

    void EditorApplication::OnStop() {
        state = EditorState::Edit;
        
        if (project) {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(project->GetStartScene());
        } else {
            // Fallback
             RTBEngine::ECS::SceneManager::GetInstance().LoadScene("Default/Scenes/DefaultScene.lua");
        }
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
        if (!uiLayer) return;

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) return;

        // 1. Render Scene View (Editor Camera)
        SceneViewPanel* sceneView = uiLayer->GetSceneViewPanel();
        if (sceneView) {
            RTBEngine::Rendering::Framebuffer* framebuffer = sceneView->GetFramebuffer();
            RTBEngine::Rendering::Camera* editorCamera = sceneView->GetEditorCamera();
            int vpWidth = sceneView->GetViewportWidth();
            int vpHeight = sceneView->GetViewportHeight();

            if (framebuffer && editorCamera && vpWidth > 0 && vpHeight > 0) {
                framebuffer->Bind();
                glViewport(0, 0, vpWidth, vpHeight);
                engineApp->RenderShadowPass(scene);
                framebuffer->Bind();
                glViewport(0, 0, vpWidth, vpHeight);
                engineApp->RenderGeometryPass(scene, editorCamera);

                // Render editor grid and axes
                if (sceneView->GetGridRenderer()) {
                    sceneView->GetGridRenderer()->Render(editorCamera);
                }

                framebuffer->Unbind();
            }
        }

        // 2. Render Game View (Main Camera)
        GameViewPanel* gameView = uiLayer->GetGameViewPanel();
        if (gameView) {
            RTBEngine::Rendering::Framebuffer* framebuffer = gameView->GetFramebuffer();
            RTBEngine::ECS::CameraComponent* mainCamComp = scene->GetMainCamera();
            int vpWidth = gameView->GetViewportWidth();
            int vpHeight = gameView->GetViewportHeight();

            // We only need to render Game view if it exists and has a camera
            if (framebuffer && mainCamComp && vpWidth > 0 && vpHeight > 0) {
                RTBEngine::Rendering::Camera* mainCamera = mainCamComp->GetCamera();
                if (mainCamera) {
                    framebuffer->Bind();
                    glViewport(0, 0, vpWidth, vpHeight);
                    // For now, reuse shadow maps from first pass
                    engineApp->RenderGeometryPass(scene, mainCamera);

                    // Note: Scene UI is rendered in GameViewPanel::OnUIRender()
                    // after the framebuffer image, within the ImGui frame

                    framebuffer->Unbind();
                }
            }
        }

        // Restore main viewport
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
