#pragma once

#include <string>

namespace RTBPlayer {

    class GameConfig {
    public:
        GameConfig();
        ~GameConfig();

        bool Load(const std::string& path);

        const std::string& GetWindowTitle() const { return windowTitle; }
        int GetWindowWidth() const { return windowWidth; }
        int GetWindowHeight() const { return windowHeight; }
        bool IsFullscreen() const { return fullscreen; }
        const std::string& GetStartScene() const { return startScene; }

    private:
        std::string windowTitle = "RTBEngine Game";
        int windowWidth = 1280;
        int windowHeight = 720;
        bool fullscreen = false;
        std::string startScene = "Assets/Scenes/DefaultScene.lua";
    };

}
