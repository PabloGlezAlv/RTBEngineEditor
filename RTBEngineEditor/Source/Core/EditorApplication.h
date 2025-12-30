#pragma once

#include <RTBEngine.h>
#include <memory>
#include "../UI/EditorLayer.h"
#include "../Project/Project.h"

namespace RTBEditor {

#include "EditorTypes.h"

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
        
        EditorState GetState() const { return m_State; }

    private:
        void Update(float deltaTime);
        void Render();
        void RenderSceneToFramebuffer();

    private:
        std::unique_ptr<RTBEngine::Core::Application> engineApp;
        std::unique_ptr<EditorLayer> uiLayer;
        std::unique_ptr<Project> project;
        bool isRunning = false;
        EditorState m_State = EditorState::Edit;
    };

}
