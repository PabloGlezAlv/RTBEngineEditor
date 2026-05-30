#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace UI {
        class UIButton;
        class UIInputField;
    }
}

class MainMenuController : public RTBEngine::ECS::Component {
public:
    MainMenuController() = default;
    ~MainMenuController() override = default;

    RTBEngine::UI::UIInputField* playerNameInput = nullptr;
    RTBEngine::UI::UIButton* playButton = nullptr;
    std::string lobbyScenePath = "Assets/Scenes/LobbyScene.lua";

    RTB_COMPONENT(MainMenuController)

    void OnStart() override;
    void OnDestroy() override;

private:
    bool callbacksBound = false;

    void BindButtons();
    void GoToLobby();
};
