#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace UI {
        class UIButton;
    }
}

class MultiplayerMenuController : public RTBEngine::ECS::Component {
public:
    MultiplayerMenuController() = default;
    ~MultiplayerMenuController() override = default;

    RTBEngine::UI::UIButton* lanLobbyButton = nullptr;
    RTBEngine::UI::UIButton* onlineLobbyButton = nullptr;
    RTBEngine::UI::UIButton* backButton = nullptr;
    std::string lobbyScenePath = "Assets/Scenes/LobbyScene.lua";
    std::string mainMenuScenePath = "Assets/Scenes/MainMenu.lua";

    RTB_COMPONENT(MultiplayerMenuController)

    void OnStart() override;
    void OnDestroy() override;

private:
    bool callbacksBound = false;

    void BindButtons();
    void RefreshButtonState();
    void GoToLobby(bool useRelayLobby);
    void GoToMainMenu();
};
