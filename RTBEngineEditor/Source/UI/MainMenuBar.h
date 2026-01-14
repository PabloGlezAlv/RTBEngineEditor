#pragma once

#include <functional>

namespace RTBEditor {

    class MainMenuBar {
    public:
        using BuildCallback = std::function<void()>;
        using ExitCallback = std::function<void()>;
        using SaveSceneCallback = std::function<void()>;
        using SaveSceneAsCallback = std::function<void()>;

        MainMenuBar();
        ~MainMenuBar();

        void OnUIRender();
        void SetExitCallback(ExitCallback callback) { exitCallback = callback; }
        void SetBuildCallback(BuildCallback callback) { buildCallback = callback; }

        void SetSaveSceneCallback(SaveSceneCallback callback) { saveSceneCallback = callback; }
        void SetSaveSceneAsCallback(SaveSceneAsCallback callback) { saveSceneAsCallback = callback; }
        void SetSceneDirty(bool dirty) { sceneDirty = dirty; }

    private:
        ExitCallback exitCallback;
        BuildCallback buildCallback;

        SaveSceneCallback saveSceneCallback;
        SaveSceneAsCallback saveSceneAsCallback;
        bool sceneDirty = false;
    };

}
