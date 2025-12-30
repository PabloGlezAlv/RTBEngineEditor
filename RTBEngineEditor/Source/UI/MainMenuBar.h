#pragma once

#include <functional>

namespace RTBEditor {

    class MainMenuBar {
    public:
        using ExitCallback = std::function<void()>;

        MainMenuBar();
        ~MainMenuBar();

        void OnUIRender();
        void SetExitCallback(ExitCallback callback) { exitCallback = callback; }

    private:
        ExitCallback exitCallback;
    };

}
