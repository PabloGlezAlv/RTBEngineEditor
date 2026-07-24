#include "EditorApplication.h"
#include <RTBEngine/Scene/NavGridComponent.h>
#include "EditorOnlineSettings.h"
#include <imgui.h>
#include <filesystem>
#include <vector>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>
#include <RTBEngine/Scripting/SceneSaver.h>
#include <RTBEngine/Scripting/ScriptManager.h>
#include <RTBEngine/Core/Window.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/Scene/MeshRenderer.h>
#include <RTBEngine/Scene/CameraComponent.h>
#include <RTBEngine/Audio/AudioSystem.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/ParticleSystem.h>
#include <RTBEngine/Scene/AnimatedBillboard.h>
#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/UI/CanvasSystem.h>
#include <RTBEngine/Rendering/Lighting/LightingUBO.h>
#include <RTBEngine/Rendering/GI/DDGISystem.h>
#include "../UI/Panels/SceneViewPanel.h"
#include "../Build/BuildSystem.h"

namespace {
    namespace fs = std::filesystem;

    const char* GetEditorBuildConfiguration() {
#ifdef _DEBUG
        return "Debug";
#else
        return "Release";
#endif
    }

    void ReloadProjectPrefabs(const RTBEditor::Project& project) {
        auto& prefabRegistry = RTBEngine::Scene::PrefabRegistry::GetInstance();
        prefabRegistry.Clear();

        const fs::path assetsPath = project.GetAssetRootPath();
        if (fs::exists(assetsPath) && fs::is_directory(assetsPath)) {
            prefabRegistry.LoadAll(assetsPath.string());
        }
    }

    fs::path ResolveProjectFilePath(const fs::path& projectFileName) {
        return RTBEditor::Project::ResolveDefaultProjectFile(projectFileName);
    }

    void TickAnimatorPreview(RTBEngine::Scene::Scene* scene, float deltaTime)
    {
        if (!scene) {
            return;
        }

        for (const auto& goPtr : scene->GetGameObjects()) {
            RTBEngine::Scene::GameObject* go = goPtr.get();
            if (!go || !go->IsActiveInHierarchy()) {
                continue;
            }

            auto* animator = go->GetComponent<RTBEngine::Animation::Animator>();
            if (!animator || !animator->IsPlaying()) {
                continue;
            }

            animator->OnUpdate(deltaTime);
        }
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

        {
            const EditorOnlineSettings onlineSettings = EditorOnlineSettingsStore::Load();
            if (!EditorOnlineSettingsStore::ApplyAndInitializeOnline(onlineSettings)) {
                RTB_WARN("EditorApplication: failed to apply online settings from disk.");
            }
        }

        engineApp->SetIsRunning(true);

        // Load Project Settings
        project = std::make_unique<Project>();
        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        resources.SetAssetRootPath({});
        const fs::path projectFilePath = RTBEditor::Project::ResolveDefaultProjectFile("MyProject.rtbproj");
        RTB_INFO("EditorApplication: Loading project from " + projectFilePath.string());

        if (project->Load(projectFilePath)) {
            resources.SetAssetRootPath(project->GetAssetRootPath());
            resources.ScanShaderAssets(project->GetAssetRootPath());
            resources.ReloadAllShaderAssets();
            const char* buildConfiguration = GetEditorBuildConfiguration();
            fs::path scriptsDllPath = project->GetGameScriptsDllPath(buildConfiguration);

            // Load GameScripts.dll before loading the scene so script component types are
            // registered in time for the SceneLoader to instantiate them.
            if (fs::exists(scriptsDllPath)) {
                RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(scriptsDllPath.string());
            }
            else {
                RTB_WARN("GameScripts.dll not found at: " + scriptsDllPath.string());
            }

            ReloadProjectPrefabs(*project);

            const std::string& lastOpen = project->GetLastOpenScene();
            const std::string& sceneToLoad = !lastOpen.empty() ? lastOpen : project->GetStartScene();
            RTBEngine::Scene::SceneManager::GetInstance().LoadScene(sceneToLoad);
        }

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

        uiLayer->SetPrefabModeCallbacks(
            [this]() { TryClosePrefab(); },
            [this]() { uiLayer->SavePrefab(); });

        uiLayer->GetContext().ensureScenePhysicsReady = [this]() -> bool {
            RTBEngine::Scene::Scene* scene = GetEditingScene(uiLayer->GetContext());
            if (!scene || !engineApp) {
                return false;
            }

            engineApp->RebuildPhysicsForScene(scene);
            return engineApp->GetPhysicsWorld() != nullptr;
        };

        uiLayer->GetMenuBar()->SetSaveSceneCallback([this]() {
            auto& sm = RTBEngine::Scene::SceneManager::GetInstance();
            if (RTBEngine::Scripting::SceneSaver::SaveScene(sm.GetActiveScene(), sm.GetActiveScenePath())) {
                sm.ClearSceneDirty();
                uiLayer->GetMenuBar()->SetSceneDirty(false);
            }
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
            
            RTBEngine::Audio::AudioSystem::GetInstance().Update();
            Render();
        }
    }

    void EditorApplication::Update(float deltaTime) {
        if (RTBEngine::Core::Application::ConsumeQuitRequest()) {
            if (state == EditorState::Play || state == EditorState::Pause) {
                OnStop();
            }
            return;
        }

        // Update smoothed FPS and frame time
        if (uiLayer) {
            StatsData& stats = uiLayer->GetContext().stats;
            float instantFps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
            smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;
            stats.fps = smoothedFps;
            stats.frameTimeMs = deltaTime * 1000.0f;

            // ECS / scene counters
            RTBEngine::Scene::Scene* scene = GetEditingScene(uiLayer->GetContext());
            if (scene) {
                stats.gameObjects = scene->GetActiveGameObjectCount();
                stats.components = scene->GetActiveComponentCount();
            }

            // Physics
            if (engineApp) {
                RTBEngine::Physics::PhysicsWorld* pw = engineApp->GetPhysicsWorld();
                stats.physicsBodies = pw ? static_cast<uint32_t>(pw->GetActiveBodyCount()) : 0u;
                const RTBEngine::ECS::EcsSimulationStats& ecsStats =
                    engineApp->GetEcsSimulationStats();
                stats.ecsEntityCount = ecsStats.aliveEntityCount;
                stats.ecsSimMs = static_cast<float>(ecsStats.lastSimulationMilliseconds);
            }

            // Audio
            stats.audioSources = static_cast<uint32_t>(
                RTBEngine::Audio::AudioSystem::GetInstance().GetActiveSourceCount());
        }

        // Sync dirty flag from SceneManager to menu bar
        if (!uiLayer) {
            return;
        }

        uiLayer->GetMenuBar()->SetSceneDirty(
            RTBEngine::Scene::SceneManager::GetInstance().IsSceneDirty()
        );

        // Keep editor-side online tools alive while the game simulation is not running.
        if (state == EditorState::Edit) {
            RTBEngine::Online::OnlineSystem::GetInstance().Tick(deltaTime);

            RTBEngine::Scene::Scene* editingScene = GetEditingScene(uiLayer->GetContext());
            if (editingScene) {
                RTBEngine::Scene::ParticleSystem::TickScenePreview(editingScene, deltaTime);
                RTBEngine::Scene::AnimatedBillboard::TickScenePreview(editingScene, deltaTime);
                TickAnimatorPreview(editingScene, deltaTime);
            }
        }

        // Consume scene open request from Content Browser double-click
        if (!uiLayer->GetContext().pendingSceneLoad.empty() && state == EditorState::Edit) {
            std::filesystem::path requested = uiLayer->GetContext().pendingSceneLoad;
            uiLayer->GetContext().pendingSceneLoad.clear();
            TryOpenScene(requested);
        }

        // Consume prefab open request from Content Browser double-click
        if (!uiLayer->GetContext().pendingPrefabOpen.empty() && state == EditorState::Edit) {
            std::filesystem::path requested = uiLayer->GetContext().pendingPrefabOpen;
            uiLayer->GetContext().pendingPrefabOpen.clear();
            TryOpenPrefab(requested);
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
                fs::path targetDllPath = BuildSystem::GetCompiledScriptsDllPath(GetEditorBuildConfiguration());

                if (fs::exists(targetDllPath)) {
                    RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(targetDllPath.string());
                    if (project) {
                        ReloadProjectPrefabs(*project);
                    }

                    // Reload using pendingScenePath saved in OnCompileScripts before UnloadScripts
                    // cleared activeScenePath.
                    if (!pendingScenePath.empty()) {
                        RTBEngine::Scene::SceneManager::GetInstance().LoadScene(pendingScenePath);
                        pendingScenePath.clear();
                    }
                    // Scene was reloaded — bail out of Update now. Any component pointers
                    // from before LoadScripts are dangling and must not be touched.
                    if (uiLayer) {
                        uiLayer->ClearSelection();
                    }
                    return;
                }
            }
        }

        if (state == EditorState::Play || state == EditorState::Pause) {
            RTBEngine::Scene::Scene* sceneBeforeUpdate =
                RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();

            if (state == EditorState::Pause) {
                RTBEngine::Core::Time::SetPaused(true);
            }

            engineApp->Update(deltaTime);

            if (state == EditorState::Pause) {
                RTBEngine::Core::Time::SetPaused(true);
            }

            if (RTBEngine::Core::Application::ConsumeQuitRequest()) {
                OnStop();
                return;
            }

            RTBEngine::Scene::Scene* sceneAfterUpdate =
                RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
            if (sceneBeforeUpdate != sceneAfterUpdate && uiLayer) {
                uiLayer->ClearSelection();
            }
        }
    }

    void EditorApplication::OnPlay() {
        RTBEngine::Core::Application::ClearQuitRequest();
        RTBEngine::Core::Time::Reset();
        if (engineApp) {
            RTBEngine::Core::Time::SetFixedDeltaTime(engineApp->GetConfig().physics.timeStep);
        }
        RTBEngine::Core::Time::SetPaused(false);

        auto& sm = RTBEngine::Scene::SceneManager::GetInstance();

        // Remember which scene was open so OnStop can restore it
        scenePathBeforePlay = sm.GetActiveScenePath();

        // Reset and re-initialize physics for the current scene before entering Play
        RTBEngine::Scene::Scene* scene = sm.GetActiveScene();
        if (scene) {
            scene->PrepareForPlayMode();
        }
        if (scene && engineApp) {
            engineApp->RebuildPhysicsForScene(scene);
            RTBEngine::Scene::NavGridComponent::ActivateAllBakedInScene(scene);
        }

        if (engineApp && engineApp->GetWindow()) {
            engineApp->GetWindow()->SetMouseCaptured(false);
            engineApp->GetWindow()->SetCursorVisible(true);
        }

        state = EditorState::Play;
        ImGui::SetWindowFocus("Game");
    }

    void EditorApplication::OnPause() {
        if (state == EditorState::Pause) {
            RTBEngine::Core::Time::SetPaused(false);
            state = EditorState::Play;
            ImGui::SetWindowFocus("Game");
            return;
        }

        if (state != EditorState::Play) {
            return;
        }

        if (engineApp && engineApp->GetWindow()) {
            engineApp->GetWindow()->SetMouseCaptured(false);
            engineApp->GetWindow()->SetCursorVisible(true);
        }
        RTBEngine::Core::Time::SetPaused(true);
        state = EditorState::Pause;
    }

    void EditorApplication::OnStop() {
        if (engineApp && engineApp->GetWindow()) {
            engineApp->GetWindow()->SetMouseCaptured(false);
            engineApp->GetWindow()->SetCursorVisible(true);
        }
        state = EditorState::Edit;

        RTBEngine::Core::Application::ClearQuitRequest();
        RTBEngine::Core::Time::SetPaused(false);
        RTBEngine::Core::Time::Reset();
        if (engineApp) {
            RTBEngine::Core::Time::SetFixedDeltaTime(engineApp->GetConfig().physics.timeStep);
        }
        RTBEngine::Scene::SceneManager::GetInstance().ClearPendingSceneLoad();

        // Clear selection before scene reload to avoid dangling pointer crashes.
        if (uiLayer)
            uiLayer->ClearSelection();

        // Clear physics world before reloading so btRigidBody/btCollisionObject
        // from the play session are removed before their GameObjects are destroyed.
        if (engineApp)
            engineApp->ResetPhysics();

        const std::string& sceneToRestore = !scenePathBeforePlay.empty()
            ? scenePathBeforePlay
            : (project ? project->GetStartScene() : std::string("Assets/Scenes/DefaultScene.lua"));

        RTBEngine::Scene::Scene* sceneBeforeReload =
            RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
        if (sceneBeforeReload) {
            RTBEngine::Scene::NavGridComponent::SaveNavMeshForScene(sceneToRestore, sceneBeforeReload);
        }

        RTBEngine::Scene::SceneManager::GetInstance().LoadScene(sceneToRestore);
        scenePathBeforePlay.clear();
    }

    void EditorApplication::TryOpenScene(const std::filesystem::path& path) {
        pendingOpenScenePath = path;
        if (HasBlockingUnsavedChanges(PendingAction::OpenScene)) {
            pendingAction = PendingAction::OpenScene;
            showUnsavedScenePopup = true;
        }
        else {
            OnOpenScene();
        }
    }

    void EditorApplication::TryOpenPrefab(const std::filesystem::path& path) {
        pendingOpenPrefabPath = path;
        if (HasBlockingUnsavedChanges(PendingAction::OpenPrefab)) {
            pendingAction = PendingAction::OpenPrefab;
            showUnsavedScenePopup = true;
        }
        else {
            OnOpenPrefab();
        }
    }

    void EditorApplication::TryClosePrefab() {
        if (!uiLayer || !uiLayer->IsPrefabEditMode()) {
            return;
        }

        if (HasBlockingUnsavedChanges(PendingAction::ClosePrefab)) {
            pendingAction = PendingAction::ClosePrefab;
            showUnsavedScenePopup = true;
        }
        else {
            OnClosePrefab();
        }
    }

    bool EditorApplication::HasUnsavedPrefabChanges() const {
        if (!uiLayer) {
            return false;
        }

        PrefabEditorSession* session = uiLayer->GetPrefabEditorSession();
        return session && session->IsOpen() && session->IsDirty();
    }

    bool EditorApplication::HasBlockingUnsavedChanges(PendingAction action) const {
        auto& sm = RTBEngine::Scene::SceneManager::GetInstance();

        if (action == PendingAction::ClosePrefab) {
            return HasUnsavedPrefabChanges();
        }

        if (action == PendingAction::OpenPrefab) {
            if (sm.IsSceneDirty()) {
                return true;
            }
            if (uiLayer && uiLayer->IsPrefabEditMode() && HasUnsavedPrefabChanges()) {
                return true;
            }
            return false;
        }

        if (HasUnsavedPrefabChanges()) {
            return true;
        }

        if (uiLayer && uiLayer->IsPrefabEditMode()) {
            if (action == PendingAction::Play || action == PendingAction::Build ||
                action == PendingAction::OpenScene) {
                return true;
            }
            return false;
        }

        return sm.IsSceneDirty();
    }

    void EditorApplication::OnOpenScene() {
        if (pendingOpenScenePath.empty()) return;

        if (uiLayer && uiLayer->IsPrefabEditMode()) {
            OnClosePrefab();
        }

        if (uiLayer)
            uiLayer->ClearSelection();
        if (engineApp)
            engineApp->ResetPhysics();

        RTBEngine::Scene::SceneManager::GetInstance().LoadScene(
            pendingOpenScenePath.string());

        pendingOpenScenePath.clear();
    }

    void EditorApplication::OnOpenPrefab() {
        if (!uiLayer || pendingOpenPrefabPath.empty()) {
            return;
        }

        if (uiLayer->IsPrefabEditMode()) {
            OnClosePrefab();
        }

        uiLayer->OpenPrefab(pendingOpenPrefabPath);
        pendingOpenPrefabPath.clear();
    }

    void EditorApplication::OnClosePrefab() {
        if (!uiLayer) {
            return;
        }

        uiLayer->ClosePrefab();
    }

    void EditorApplication::TryPlay() {
        if (uiLayer && uiLayer->IsPrefabEditMode()) {
            return;
        }

        if (HasBlockingUnsavedChanges(PendingAction::Play)) {
            pendingAction = PendingAction::Play;
            showUnsavedScenePopup = true;
        }
        else {
            OnPlay();
        }
    }

    void EditorApplication::TryBuild() {
        if (HasBlockingUnsavedChanges(PendingAction::Build)) {
            pendingAction = PendingAction::Build;
            showUnsavedScenePopup = true;
        }
        else {
            uiLayer->OpenBuildDialog();
        }
    }

    void EditorApplication::TryExit() {
        if (uiLayer && uiLayer->IsPrefabEditMode() && !HasUnsavedPrefabChanges()) {
            OnClosePrefab();
        }

        if (HasBlockingUnsavedChanges(PendingAction::Exit)) {
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
        case PendingAction::Exit:
            if (uiLayer && uiLayer->IsPrefabEditMode()) {
                OnClosePrefab();
            }
            isRunning = false;
            break;
        case PendingAction::OpenScene: OnOpenScene(); break;
        case PendingAction::OpenPrefab: OnOpenPrefab(); break;
        case PendingAction::ClosePrefab: OnClosePrefab(); break;
        case PendingAction::None:      break;
        }
        pendingAction = PendingAction::None;
    }

    void EditorApplication::RenderUnsavedScenePopup() {
        if (showUnsavedScenePopup) {
            ImGui::OpenPopup("UnsavedChangesPopup");
            showUnsavedScenePopup = false;
        }

        bool popupOpen = true;
        if (ImGui::BeginPopupModal("UnsavedChangesPopup", &popupOpen,
            ImGuiWindowFlags_AlwaysAutoResize)) {
            const bool prefabDirty = HasUnsavedPrefabChanges();
            const bool sceneDirty = RTBEngine::Scene::SceneManager::GetInstance().IsSceneDirty();
            const bool closingPrefab = pendingAction == PendingAction::ClosePrefab;
            const bool openingPrefab = pendingAction == PendingAction::OpenPrefab;
            const bool inPrefabMode = uiLayer && uiLayer->IsPrefabEditMode();

            if (closingPrefab || (openingPrefab && prefabDirty)) {
                ImGui::Text("The prefab has unsaved changes.");
            } else if (inPrefabMode && pendingAction != PendingAction::ClosePrefab) {
                ImGui::Text("You are editing a prefab. Close prefab mode before continuing.");
            } else if (sceneDirty) {
                ImGui::Text("The current scene has unsaved changes.");
            } else {
                ImGui::Text("There are unsaved changes.");
            }

            ImGui::Text("What would you like to do?");
            ImGui::Spacing();

            const bool canSavePrefab = prefabDirty &&
                (closingPrefab || openingPrefab || pendingAction == PendingAction::Exit);
            const bool canSaveScene = sceneDirty && !inPrefabMode;
            const bool canSave = canSavePrefab || canSaveScene;

            if (!canSave) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Save")) {
                bool saved = false;
                if (canSavePrefab) {
                    saved = uiLayer->SavePrefab();
                }
                if (canSaveScene) {
                    auto& sm = RTBEngine::Scene::SceneManager::GetInstance();
                    if (RTBEngine::Scripting::SceneSaver::SaveScene(
                        sm.GetActiveScene(), sm.GetActiveScenePath())) {
                        sm.ClearSceneDirty();
                        uiLayer->GetMenuBar()->SetSceneDirty(false);
                        saved = true;
                    }
                }

                if (saved || !canSave) {
                    ExecutePendingAction();
                    ImGui::CloseCurrentPopup();
                }
            }
            if (!canSave) {
                ImGui::EndDisabled();
            }
            ImGui::SameLine();
            if (ImGui::Button("Ignore")) {
                ExecutePendingAction();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                pendingAction = PendingAction::None;
                pendingOpenPrefabPath.clear();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (!popupOpen) {
            pendingAction = PendingAction::None;
            pendingOpenPrefabPath.clear();
        }
    }

    void EditorApplication::Render() {
        // Render scene to Scene View Panel framebuffer
        RenderSceneToFramebuffer();

        // Read render stats after all draw calls have been submitted
        if (uiLayer) {
            StatsData& stats = uiLayer->GetContext().stats;
            stats.drawCalls = RTBEngine::Scene::MeshRenderer::GetDrawCallCount();
            stats.triangles = RTBEngine::Scene::MeshRenderer::GetTriangleCount();
            stats.culledObjects = RTBEngine::Scene::MeshRenderer::GetCulledObjectCount();
        }

        // Clear the main window
        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        device.SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        device.Clear(RTBEngine::Rendering::RHI::ClearMask::ColorDepth);

        // Render editor UI
        if (uiLayer) {
            uiLayer->SetEditorState(state);
            uiLayer->GetContext().window = engineApp->GetWindow();
            uiLayer->Begin();
            uiLayer->OnUIRender();
            uiLayer->End();
        }

        engineApp->GetWindow()->SwapBuffers();
    }

    void EditorApplication::RenderSceneToFramebuffer() {
        if (!uiLayer) return;

        EditorContext& editorContext = uiLayer->GetContext();
        RTBEngine::Scene::Scene* sceneViewScene = GetEditingScene(editorContext);
        RTBEngine::Scene::Scene* activeScene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
        if (!sceneViewScene) return;

        PruneSelectionToScene(editorContext, sceneViewScene);

        // Reset per-frame render counters before any draw submissions
        RTBEngine::Scene::MeshRenderer::ResetRenderStats();

        RTBEngine::UI::CanvasSystem::GetInstance().Update(sceneViewScene);

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();

        // 1. Render Scene View (Editor Camera)
        SceneViewPanel* sceneView = uiLayer->GetSceneViewPanel();
        if (sceneView) {
            RTBEngine::Rendering::Framebuffer* framebuffer = sceneView->GetFramebuffer();
            RTBEngine::Rendering::Camera* editorCamera = sceneView->GetEditorCamera();
            int vpWidth = sceneView->GetViewportWidth();
            int vpHeight = sceneView->GetViewportHeight();

            if (framebuffer && editorCamera && vpWidth > 0 && vpHeight > 0) {
                framebuffer->Bind();
                device.SetViewport(0, 0, vpWidth, vpHeight);
                engineApp->RenderShadowPass(sceneViewScene);
                framebuffer->Bind();
                device.SetViewport(0, 0, vpWidth, vpHeight);
                engineApp->UploadSceneLighting(sceneViewScene);
                RTBEngine::Rendering::LightingUBO::GetInstance().Bind();
                RTBEngine::Rendering::GI::DDGISystem::GetInstance().Update(sceneViewScene);
                engineApp->RenderGeometryPass(sceneViewScene, editorCamera);

                // Render editor grid and axes
                if (sceneView->GetGridRenderer()) {
                    sceneView->GetGridRenderer()->Render(editorCamera);
                }

                if (sceneView->GetColliderRenderer()) {
                    sceneView->GetColliderRenderer()->RenderDebugQueries(editorCamera);
                }

                // Render collider wireframe for selected object
                if (sceneView->GetColliderRenderer()) {
                    sceneView->GetColliderRenderer()->RenderSelection(editorCamera, uiLayer->GetSelectedGameObject());
                }

                if (sceneView->GetNavGridDebugRenderer()) {
                    sceneView->GetNavGridDebugRenderer()->Render(
                        editorCamera,
                        sceneViewScene,
                        uiLayer->GetSelectedGameObject(),
                        editorContext.navDebug);
                }

                if (sceneView->GetDDGIDebugRenderer()) {
                    sceneView->GetDDGIDebugRenderer()->Render(
                        editorCamera,
                        editorContext.ddgiDebug);
                }

                framebuffer->Unbind();
            }
        }

        // 2. Render Game View (Main Camera)
        GameViewPanel* gameView = uiLayer->GetGameViewPanel();
        if (gameView && activeScene) {
            RTBEngine::Rendering::Framebuffer* framebuffer = gameView->GetFramebuffer();
            RTBEngine::Scene::CameraComponent* mainCamComp = activeScene->GetMainCamera();
            int vpWidth = static_cast<int>(gameView->GetRenderWidth());
            int vpHeight = static_cast<int>(gameView->GetRenderHeight());

            // We only need to render Game view if it exists and has a camera
            if (framebuffer && mainCamComp && vpWidth > 0 && vpHeight > 0) {
                mainCamComp->SyncNow();
                RTBEngine::Rendering::Camera* mainCamera = mainCamComp->GetCamera();
                if (mainCamera) {
                    framebuffer->Bind();
                    device.SetViewport(0, 0, vpWidth, vpHeight);
                    // For now, reuse shadow maps from first pass
                    engineApp->RenderGeometryPass(activeScene, mainCamera);

                    // Note: Scene UI is rendered in GameViewPanel::OnUIRender()
                    // after the framebuffer image, within the ImGui frame

                    framebuffer->Unbind();
                }
            }
        }

        // Restore main viewport
        device.SetViewport(0, 0, engineApp->GetWindow()->GetWidth(), engineApp->GetWindow()->GetHeight());
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
            auto& sm = RTBEngine::Scene::SceneManager::GetInstance();
            pendingScenePath = sm.GetActiveScenePath();
            if (!pendingScenePath.empty() && sm.GetActiveScene()) {
                if (RTBEngine::Scripting::SceneSaver::SaveScene(sm.GetActiveScene(), pendingScenePath)) {
                    sm.ClearSceneDirty();
                }
            }
        }

        if (uiLayer) {
            uiLayer->ClearSelection();
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
            ScriptCompileResult result = BuildSystem::CompileScripts(
                vcxprojPath.string(),
                GetEditorBuildConfiguration());
            compileJobResult.store(static_cast<int>(result), std::memory_order_release);
            compileJobDone.store(true, std::memory_order_release);
        });
    }

    void EditorApplication::Shutdown() {
        RTBEngine::Scene::SceneManager::GetInstance().ClearPendingSceneLoad();

        // Persist the last open scene so it reopens on next editor launch
        if (project && Project::GetActiveProject() == project.get()) {
            const std::string& activeScenePath = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScenePath();
            const std::string& lastScene =
                (state != EditorState::Edit && !scenePathBeforePlay.empty())
                    ? scenePathBeforePlay
                    : activeScenePath;
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

        // Destroy editor GPU resources (SceneView FBO, debug renderers) while the
        // RenderDevice is still alive. Application::Shutdown tears the device down.
        uiLayer.reset();

        if (engineApp) {
            engineApp->Shutdown();
            engineApp.reset();
        }
        isRunning = false;
    }
}
