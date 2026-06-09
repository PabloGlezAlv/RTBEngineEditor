#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace UI {
        class UIButton;
        class UIInputField;
        class UIText;
    }
}

class MainMenuController : public RTBEngine::ECS::Component {
public:
    MainMenuController() = default;
    ~MainMenuController() override = default;

    RTBEngine::UI::UIInputField* playerNameInput = nullptr;
    RTBEngine::UI::UIButton* playButton = nullptr;
    RTBEngine::UI::UIButton* multiplayerButton = nullptr;
    RTBEngine::UI::UIText* statusMessageText = nullptr;
    std::string gameScenePath = "Assets/Scenes/DefaultScene.lua";
    std::string multiplayerMenuScenePath = "Assets/Scenes/MultiplayerMenu.lua";

    RTB_COMPONENT(MainMenuController)

    void OnStart() override;
    void OnDestroy() override;

private:
    bool callbacksBound = false;

    void BindButtons();
    void ApplyPendingStatusMessage();
    void ApplyPlayerName();
    void GoToGame();
    void GoToMultiplayer();
};
