#pragma once

#include <RTBEngine.h>
#include <memory>

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

    private:
        std::unique_ptr<RTBEngine::Core::Application> engineApp;
        bool isRunning = false;
    };

}
