#include <RTBEngine.h>
#include "GameConfig.h"

int main(int argc, char* argv[]) {
    RTBPlayer::GameConfig gameConfig;
    gameConfig.Load("game.cfg");
    gameConfig.ApplyCommandLine(argc, argv);

    RTBEngine::Core::ApplicationConfig config;
    config.window.title = gameConfig.GetWindowTitle();
    config.logoPath = gameConfig.GetLogoPath();
    config.window.width = gameConfig.GetWindowWidth();
    config.window.height = gameConfig.GetWindowHeight();
    config.window.fullscreen = gameConfig.IsFullscreen();
    config.initialScenePath = gameConfig.GetStartScene();
    config.rendering.graphicsAPI = gameConfig.GetGraphicsAPI();
    config.online = gameConfig.GetOnlineConfig();

    return RTBEngine::Run(config);
}
