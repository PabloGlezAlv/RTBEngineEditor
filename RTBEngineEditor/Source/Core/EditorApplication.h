#pragma once

#include <RTBEngine.h>
#include <memory>
#include "../UI/EditorLayer.h"
#include "../Project/Project.h"

namespace RTBEditor {

    class EditorApplication {
    public:
        EditorApplication();
        ~EditorApplication();

        bool Initialize(const RTBEngine::Core::ApplicationConfig& config);
        void Run();
        void Shutdown();

    private:
        void Update(float deltaTime);
        void Render();
        void RenderSceneToFramebuffer();

    private:
        std::unique_ptr<RTBEngine::Core::Application> engineApp;
        std::unique_ptr<EditorLayer> uiLayer;
        std::unique_ptr<Project> project;
        bool isRunning = false;
    };

}
