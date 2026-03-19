#include "EditorApplication.h"
#include <imgui.h>
#include <GL/glew.h>
#include <iostream>
#include <filesystem>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Scripting/SceneSaver.h>
#include <RTBEngine/Scripting/ScriptManager.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/Audio/AudioSystem.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include "../UI/Panels/SceneViewPanel.h"
#include "../Build/BuildSystem.h"

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

        // Load GameScripts.dll before loading the scene so script component types are
        // registered in time for the SceneLoader to instantiate them.
        {
            namespace fs = std::filesystem;
            fs::path projectRoot = fs::current_path();
            fs::path binDirDebug = projectRoot / "x64" / "Debug";
            fs::path binDllPath = binDirDebug / "GameScripts.dll";
            fs::path legacyPath = projectRoot / "GameScripts.dll";

            if (fs::exists(binDllPath)) {
                RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(binDllPath.string());
            }
            else if (fs::exists(legacyPath)) {
                RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(legacyPath.string());
            }
        }

        // Load Project Settings
        project = std::make_unique<Project>();
        if (project->Load("MyProject.rtbproj")) {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(project->GetStartScene());
        }

        // Initialize UI Layer
        uiLayer = std::make_unique<EditorLayer>();
        uiLayer->Initialize(engineApp->GetWindow()->GetSDLWindow(), engineApp->GetImGuiContext());

        uiLayer->SetupToolbar(
            [this]() { OnPlay(); },
            [this]() { OnPause(); },
            [this]() { OnStop(); },
            [this]() { return GetState(); },
            [this]() { return IsCompilingScripts(); },
            [this]() { OnCompileScripts(); }
        );

        // Set up menu bar callbacks
        uiLayer->GetMenuBar()->SetBuildCallback([this]() {
            uiLayer->OpenBuildDialog();
        });

        uiLayer->GetMenuBar()->SetExitCallback([this]() {
            isRunning = false;
        });

        uiLayer->GetMenuBar()->SetSaveSceneCallback([this]() {
            auto& sm = RTBEngine::ECS::SceneManager::GetInstance();
            RTBEngine::Scripting::SceneSaver::SaveScene(sm.GetActiveScene(), sm.GetActiveScenePath());
            sm.ClearSceneDirty();
            uiLayer->GetMenuBar()->SetSceneDirty(false);
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

        // Update smoothed FPS and frame time
        if (uiLayer) {
            StatsData& stats = uiLayer->GetContext().stats;
            float instantFps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
            smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;
            stats.fps = smoothedFps;
            stats.frameTimeMs = deltaTime * 1000.0f;

            // ECS / scene counters
            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            if (scene) {
                stats.gameObjects = scene->GetActiveGameObjectCount();
                stats.components = scene->GetActiveComponentCount();
            }

            // Physics
            if (engineApp) {
                RTBEngine::Physics::PhysicsWorld* pw = engineApp->GetPhysicsWorld();
                stats.physicsBodies = pw ? static_cast<uint32_t>(pw->GetActiveBodyCount()) : 0u;
            }

            // Audio
            stats.audioSources = static_cast<uint32_t>(
                RTBEngine::Audio::AudioSystem::GetInstance().GetActiveSourceCount());
        }

        // Sync dirty flag from SceneManager to menu bar
        uiLayer->GetMenuBar()->SetSceneDirty(
            RTBEngine::ECS::SceneManager::GetInstance().IsSceneDirty()
        );

        // If an async script compilation finished, join the worker and reload the DLL.
        if (isCompilingScripts && compileJobDone.load(std::memory_order_acquire)) {
            if (compileThread.joinable()) {
                compileThread.join();
            }

            isCompilingScripts = false;

            ScriptCompileResult result =
                static_cast<ScriptCompileResult>(compileJobResult.load(std::memory_order_acquire));

            if (result != ScriptCompileResult::Success) {
                pendingScenePath.clear();
            }

            if (result == ScriptCompileResult::Success) {
                namespace fs = std::filesystem;
                fs::path projectRoot = fs::current_path();
                fs::path binDirDebug = projectRoot / "x64" / "Debug";
                fs::path targetDllPath = binDirDebug / "GameScripts.dll";

                if (fs::exists(targetDllPath)) {
                    RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(targetDllPath.string());

                    // Reload using pendingScenePath saved in OnCompileScripts before UnloadScripts
                    // cleared activeScenePath.
                    if (!pendingScenePath.empty()) {
                        RTBEngine::ECS::SceneManager::GetInstance().LoadScene(pendingScenePath);
                        pendingScenePath.clear();
                    }
                    // Scene was reloaded — bail out of Update now. Any component pointers
                    // from before LoadScripts are dangling and must not be touched.
                    return;
                }
            }
        }

        if (state == EditorState::Play) {
            engineApp->Update(deltaTime);
        }
    }

    void EditorApplication::OnPlay() {
        // Reset and re-initialize physics for the current scene before entering Play
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (scene && engineApp) {
            engineApp->ResetPhysics();
            engineApp->InitializePhysicsForScene(scene);
        }

        state = EditorState::Play;
        ImGui::SetWindowFocus("Game");
    }

    void EditorApplication::OnPause() {
        state = EditorState::Pause;
    }

    void EditorApplication::OnStop() {
        state = EditorState::Edit;

        // Clear selection before scene reload to avoid dangling pointer crashes.
        if (uiLayer)
            uiLayer->SetSelectedGameObject(nullptr);

        // Clear physics world before reloading so btRigidBody/btCollisionObject
        // from the play session are removed before their GameObjects are destroyed.
        if (engineApp)
            engineApp->ResetPhysics();

        if (project) {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(project->GetStartScene());
        } else {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene("Default/Scenes/DefaultScene.lua");
        }
    }

    void EditorApplication::Render() {
        // Render scene to Scene View Panel framebuffer
        RenderSceneToFramebuffer();

        // Read render stats after all draw calls have been submitted
        if (uiLayer) {
            StatsData& stats = uiLayer->GetContext().stats;
            stats.drawCalls = RTBEngine::ECS::MeshRenderer::GetDrawCallCount();
            stats.triangles = RTBEngine::ECS::MeshRenderer::GetTriangleCount();
            stats.culledObjects = RTBEngine::ECS::MeshRenderer::GetCulledObjectCount();
        }

        // Clear the main window
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render editor UI
        if (uiLayer) {
            uiLayer->SetEditorState(state);
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

        // Reset per-frame render counters before any draw submissions
        RTBEngine::ECS::MeshRenderer::ResetRenderStats();

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

                // Render collider wireframe for selected object
                if (sceneView->GetColliderRenderer()) {
                    sceneView->GetColliderRenderer()->RenderSelection(editorCamera, uiLayer->GetSelectedGameObject());
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

    void EditorApplication::OnCompileScripts()
    {
        if (isCompilingScripts)
            return;

        namespace fs = std::filesystem;

        // Path to GameScripts.vcxproj — relative to the editor working directory
        std::string vcxprojPath = (fs::current_path() / "GameScripts" / "GameScripts.vcxproj").string();

        auto& scriptManager = RTBEngine::Scripting::ScriptManager::GetInstance();

        // Save scene path and auto-save to disk BEFORE UnloadScripts clears everything.
        // UnloadScripts -> UnloadCurrentScene clears activeScenePath, so we must capture
        // it here while the scene is still loaded.
        {
            auto& sm = RTBEngine::ECS::SceneManager::GetInstance();
            pendingScenePath = sm.GetActiveScenePath();
            if (!pendingScenePath.empty() && sm.GetActiveScene()) {
                RTBEngine::Scripting::SceneSaver::SaveScene(sm.GetActiveScene(), pendingScenePath);
                sm.ClearSceneDirty();
            }
        }

        // Unload current DLL before recompiling so MSBuild can overwrite the file.
        // This also calls UnloadCurrentScene internally.
        scriptManager.UnloadScripts();

        // Ensure previous job is finished
        if (compileThread.joinable()) {
            compileThread.join();
        }

        isCompilingScripts = true;
        compileJobDone.store(false, std::memory_order_release);
        compileJobResult.store(static_cast<int>(ScriptCompileResult::Failure), std::memory_order_release);

        // Launch MSBuild in a background thread. It will compile and copy the DLL,
        // and we will load it back on the main thread once finished.
        compileThread = std::thread([this, vcxprojPath]() {
            ScriptCompileResult result = BuildSystem::CompileScripts(vcxprojPath);
            compileJobResult.store(static_cast<int>(result), std::memory_order_release);
            compileJobDone.store(true, std::memory_order_release);
        });
    }

    void EditorApplication::Shutdown() {
        // Unload GameScripts.dll before destroying the engine to avoid dangling pointers
        RTBEngine::Scripting::ScriptManager::GetInstance().UnloadScripts();

        if (compileThread.joinable()) {
            compileThread.join();
        }

        if (engineApp) {
            engineApp->Shutdown();
            engineApp.reset();
        }
        isRunning = false;
    }
}
