#pragma once

#include <RTBEngine.h>
#include <memory>
#include <thread>
#include <atomic>
#include <filesystem>
#include "../UI/EditorLayer.h"
#include "../Project/Project.h"

namespace RTBEditor {

#include "EditorTypes.h"

    enum class PendingAction { None, Play, Build, Exit, OpenScene, OpenPrefab, ClosePrefab };

    class EditorApplication {
    public:
        EditorApplication();
        ~EditorApplication();

        bool Initialize(const RTBEngine::Core::ApplicationConfig& config);
        void Run();
        void Shutdown();

        // Play/Pause/Stop controls
        void OnPlay(); 
        void OnStop();
        void OnPause();

        void TryPlay();
        void TryBuild();
        void TryExit();
        void TryOpenScene(const std::filesystem::path& path);
        void TryOpenPrefab(const std::filesystem::path& path);
        void TryClosePrefab();

        // Script compilation
        void OnCompileScripts();
        bool IsCompilingScripts() const { return isCompilingScripts; }
        
        EditorState GetState() const { return state; }

    private:
        void Update(float deltaTime);
        void Render();
        void RenderSceneToFramebuffer();

        void ExecutePendingAction();
        void RenderUnsavedScenePopup();
        void OnOpenScene();
        void OnOpenPrefab();
        void OnClosePrefab();
        bool HasUnsavedPrefabChanges() const;
        bool HasBlockingUnsavedChanges(PendingAction action) const;
    private:
        std::unique_ptr<RTBEngine::Core::Application> engineApp;
        std::unique_ptr<EditorLayer> uiLayer;
        std::unique_ptr<Project> project;
        bool isRunning = false;
        EditorState state = EditorState::Edit;

        // Async script compilation state
        bool isCompilingScripts = false;
        std::thread compileThread;
        std::atomic<bool> compileJobDone{ false };
        std::atomic<int> compileJobResult{ 0 };
        std::string pendingScenePath;
        std::string scenePathBeforePlay;
        std::filesystem::path pendingOpenScenePath;
        std::filesystem::path pendingOpenPrefabPath;

        // Stats smoothing
        float smoothedFps = 0.0f;

        PendingAction pendingAction = PendingAction::None;
        bool showUnsavedScenePopup = false;
    };

}
