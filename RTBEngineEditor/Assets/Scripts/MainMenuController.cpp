#include "MainMenuController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIInputField.h>

using ThisClass = MainMenuController;

RTB_REGISTER_COMPONENT(MainMenuController)
    RTB_PROPERTY_COMPONENT(playerNameInput, UIInputField)
    RTB_PROPERTY_COMPONENT(playButton, UIButton)
    RTB_PROPERTY_ASSET_PATH(lobbyScenePath, "lua")
RTB_END_REGISTER(MainMenuController)

void MainMenuController::OnStart()
{
    BindButtons();
}

void MainMenuController::OnDestroy()
{
    callbacksBound = false;
    playButton = nullptr;
    playerNameInput = nullptr;
}

void MainMenuController::BindButtons()
{
    if (callbacksBound || !playButton) {
        return;
    }

    playButton->SetOnClick([this]() { GoToLobby(); });
    callbacksBound = true;
}

void MainMenuController::GoToLobby()
{
    if (playButton && !playButton->IsInteractable()) {
        return;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    const std::string playerName = playerNameInput ? playerNameInput->GetText() : "";
    online.SetSessionDisplayName(playerName);

    if (RTBEngine::Online::IOnlineIdentity* identity = online.GetIdentity()) {
        if (identity->IsLoggedIn()) {
            identity->Logout();
        }
    }

    if (lobbyScenePath.empty()) {
        RTB_WARN("MainMenuController: lobbyScenePath is empty.");
        return;
    }

    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(lobbyScenePath.c_str());
}
