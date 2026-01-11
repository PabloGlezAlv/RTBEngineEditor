#pragma once

#include <functional>

namespace RTBEditor {

    class MainMenuBar {
    public:
        using BuildCallback = std::function<void()>;
        using ExitCallback = std::function<void()>;

        MainMenuBar();
        ~MainMenuBar();

        void OnUIRender();
        void SetExitCallback(ExitCallback callback) { exitCallback = callback; }
        void SetBuildCallback(BuildCallback callback) { buildCallback = callback; }

    private:
        ExitCallback exitCallback;
        BuildCallback buildCallback;
    };

}
