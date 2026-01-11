#include <RTBEngine.h>
#include "GameConfig.h"

int main(int argc, char* argv[]) {
    RTBPlayer::GameConfig gameConfig;
    gameConfig.Load("game.cfg");

    RTBEngine::Core::ApplicationConfig config;
    config.window.title = gameConfig.GetWindowTitle();
    config.window.width = gameConfig.GetWindowWidth();
    config.window.height = gameConfig.GetWindowHeight();
    config.window.fullscreen = gameConfig.IsFullscreen();
    config.initialScenePath = gameConfig.GetStartScene();

    return RTBEngine::Run(config);
}
