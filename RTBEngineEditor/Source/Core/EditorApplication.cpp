#include "EditorApplication.h"
#include <imgui.h>
#include <GL/glew.h>
#include <filesystem>
#include <vector>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Scripting/SceneSaver.h>
#include <RTBEngine/Scripting/ScriptManager.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/Audio/AudioSystem.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include "../UI/Panels/SceneViewPanel.h"
#include "../Build/BuildSystem.h"

namespace {
    namespace fs = std::filesystem;

    fs::path ResolveProjectFilePath(const fs::path& projectFileName) {
        const fs::path currentDir = fs::current_path();

        std::vector<fs::path> searchDirs;
        for (fs::path dir = currentDir;; dir = dir.parent_path()) {
            searchDirs.push_back(dir);

            const fs::path parent = dir.parent_path();
            if (parent == dir) {
                break;
            }
        }

        for (const fs::path& dir : searchDirs) {
            const fs::path candidate = (dir / projectFileName).lexically_normal();
            if (!fs::exists(candidate)) {
                continue;
            }

            // Prefer the real editor source root when running from x64/Debug or x64/Release.
            if (fs::exists(dir / "RTBEngineEditor.vcxproj") || fs::exists(dir / "Source")) {
                return candidate;
            }
        }

        const fs::path localCandidate = (currentDir / projectFileName).lexically_normal();
        if (fs::exists(localCandidate)) {
            return localCandidate;
        }

        return fs::absolute(projectFileName).lexically_normal();
    }
}

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
        const fs::path projectFilePath = ResolveProjectFilePath("MyProject.rtbproj");
        RTB_INFO("EditorApplication: Loading project from " + projectFilePath.string());

        if (project->Load(projectFilePath)) {
            fs::path scriptsDllPath = project->GetGameScriptsDllPath("Debug");

            // Load GameScripts.dll before loading the scene so script component types are
            // registered in time for the SceneLoader to instantiate them.
            if (fs::exists(scriptsDllPath)) {
                RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(scriptsDllPath.string());
            }
            else {
                RTB_WARN("GameScripts.dll not found at: " + scriptsDllPath.string());
            }

            const std::string& lastOpen = project->GetLastOpenScene();
            const std::string& sceneToLoad = !lastOpen.empty() ? lastOpen : project->GetStartScene();
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(sceneToLoad);
        }

        // Initialize UI Layer
        uiLayer = std::make_unique<EditorLayer>();
        uiLayer->Initialize(engineApp->GetImGuiContext());

        uiLayer->SetupToolbar(
            [this]() { TryPlay(); },
            [this]() { OnPause(); },
            [this]() { OnStop(); },
            [this]() { return GetState(); },
            [this]() { return IsCompilingScripts(); },
            [this]() { OnCompileScripts(); }
        );

        // Set up menu bar callbacks
        uiLayer->GetMenuBar()->SetBuildCallback([this]() {
            TryBuild();
        });

        uiLayer->GetMenuBar()->SetExitCallback([this]() {
            TryExit();
        });

        uiLayer->SetRenderPopupCallback([this]() {
            RenderUnsavedScenePopup();
            });

        uiLayer->GetMenuBar()->SetSaveSceneCallback([this]() {
            auto& sm = RTBEngine::ECS::SceneManager::GetInstance();
            RTBEngine::Scripting::SceneSaver::SaveScene(sm.GetActiveScene(), sm.GetActiveScenePath());
            sm.ClearSceneDirty();
            uiLayer->GetMenuBar()->SetSceneDirty(false);
        });

        // Intercept window close button so the editor can show the unsaved-scene popup
        engineApp->SetOnQuitRequested([this]() {
            TryExit();
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
                engineApp->GetWindow()->SetShouldClose(false);
                TryExit();
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

        // Consume scene open request from Content Browser double-click
        if (!uiLayer->GetContext().pendingSceneLoad.empty() && state == EditorState::Edit) {
            std::filesystem::path requested = uiLayer->GetContext().pendingSceneLoad;
            uiLayer->GetContext().pendingSceneLoad.clear();
            TryOpenScene(requested);
        }

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
                fs::path targetDllPath = BuildSystem::GetCompiledScriptsDllPath("Debug");

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
        auto& sm = RTBEngine::ECS::SceneManager::GetInstance();

        // Remember which scene was open so OnStop can restore it
        scenePathBeforePlay = sm.GetActiveScenePath();

        // Reset and re-initialize physics for the current scene before entering Play
        RTBEngine::ECS::Scene* scene = sm.GetActiveScene();
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

        const std::string& sceneToRestore = !scenePathBeforePlay.empty()
            ? scenePathBeforePlay
            : (project ? project->GetStartScene() : std::string("Default/Scenes/DefaultScene.lua"));
        RTBEngine::ECS::SceneManager::GetInstance().LoadScene(sceneToRestore);
        scenePathBeforePlay.clear();
    }

    void EditorApplication::TryOpenScene(const std::filesystem::path& path) {
        pendingOpenScenePath = path;
        if (RTBEngine::ECS::SceneManager::GetInstance().IsSceneDirty()) {
            pendingAction = PendingAction::OpenScene;
            showUnsavedScenePopup = true;
        }
        else {
            OnOpenScene();
        }
    }

    void EditorApplication::OnOpenScene() {
        if (pendingOpenScenePath.empty()) return;

        if (uiLayer)
            uiLayer->SetSelectedGameObject(nullptr);
        if (engineApp)
            engineApp->ResetPhysics();

        RTBEngine::ECS::SceneManager::GetInstance().LoadScene(
            pendingOpenScenePath.string());

        pendingOpenScenePath.clear();
    }

    void EditorApplication::TryPlay() {
        if (RTBEngine::ECS::SceneManager::GetInstance().IsSceneDirty()) {
            pendingAction = PendingAction::Play;
            showUnsavedScenePopup = true;
        }
        else {
            OnPlay();
        }
    }

    void EditorApplication::TryBuild() {
        if (RTBEngine::ECS::SceneManager::GetInstance().IsSceneDirty()) {
            pendingAction = PendingAction::Build;
            showUnsavedScenePopup = true;
        }
        else {
            uiLayer->OpenBuildDialog();
        }
    }

    void EditorApplication::TryExit() {
        if (RTBEngine::ECS::SceneManager::GetInstance().IsSceneDirty()) {
            pendingAction = PendingAction::Exit;
            showUnsavedScenePopup = true;
        }
        else {
            isRunning = false;
        }
    }

    void EditorApplication::ExecutePendingAction() {
        switch (pendingAction) {
        case PendingAction::Play:  OnPlay(); break;
        case PendingAction::Build: uiLayer->OpenBuildDialog(); break;
        case PendingAction::Exit:      isRunning = false; break;
        case PendingAction::OpenScene: OnOpenScene(); break;
        case PendingAction::None:      break;
        }
        pendingAction = PendingAction::None;
    }

    void EditorApplication::RenderUnsavedScenePopup() {
        if (showUnsavedScenePopup) {
            ImGui::OpenPopup("Scene is not saved");
            showUnsavedScenePopup = false;
        }

        bool popupOpen = true;
        if (ImGui::BeginPopupModal("Scene is not saved", &popupOpen,
            ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("The current scene has unsaved changes.");
            ImGui::Text("What would you like to do?");
            ImGui::Spacing();

            if (ImGui::Button("Save")) {
                auto& sm = RTBEngine::ECS::SceneManager::GetInstance();
                RTBEngine::Scripting::SceneSaver::SaveScene(
                    sm.GetActiveScene(), sm.GetActiveScenePath());
                sm.ClearSceneDirty();
                uiLayer->GetMenuBar()->SetSceneDirty(false);
                ExecutePendingAction();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Ignore")) {
                ExecutePendingAction();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                pendingAction = PendingAction::None;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Si el usuario cerró con la X, limpiar la acción pendiente
        if (!popupOpen) {
            pendingAction = PendingAction::None;
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

        if (!project || Project::GetActiveProject() != project.get())
            return;

        // Path to GameScripts.vcxproj — relative to the editor working directory
        const std::filesystem::path vcxprojPath = project->GetGameScriptsProjectPath();

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
            ScriptCompileResult result = BuildSystem::CompileScripts(vcxprojPath.string());
            compileJobResult.store(static_cast<int>(result), std::memory_order_release);
            compileJobDone.store(true, std::memory_order_release);
        });
    }

    void EditorApplication::Shutdown() {
        // Persist the last open scene so it reopens on next editor launch
        if (project && Project::GetActiveProject() == project.get()) {
            const std::string& lastScene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScenePath();
            if (!lastScene.empty()) {
                project->SetLastOpenScene(lastScene);
            }
            project->Save();
        }

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
