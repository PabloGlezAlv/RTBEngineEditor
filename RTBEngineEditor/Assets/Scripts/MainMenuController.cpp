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
    RTB_PROPERTY_COMPONENT(multiplayerButton, UIButton)
    RTB_PROPERTY_ASSET_PATH(gameScenePath, "lua")
    RTB_PROPERTY_ASSET_PATH(multiplayerMenuScenePath, "lua")
RTB_END_REGISTER(MainMenuController)

void MainMenuController::OnStart()
{
    BindButtons();
}

void MainMenuController::OnDestroy()
{
    callbacksBound = false;
    playButton = nullptr;
    multiplayerButton = nullptr;
    playerNameInput = nullptr;
}

void MainMenuController::BindButtons()
{
    if (callbacksBound) {
        return;
    }

    if (playButton) {
        playButton->SetOnClick([this]() { GoToGame(); });
    }

    if (multiplayerButton) {
        multiplayerButton->SetOnClick([this]() { GoToMultiplayer(); });
    }

    callbacksBound = playButton != nullptr || multiplayerButton != nullptr;
}

void MainMenuController::ApplyPlayerName()
{
    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    const std::string playerName = playerNameInput ? playerNameInput->GetText() : "";
    online.SetSessionDisplayName(playerName);

    if (RTBEngine::Online::IOnlineIdentity* identity = online.GetIdentity()) {
        if (identity->IsLoggedIn()) {
            identity->Logout();
        }
    }
}

void MainMenuController::GoToGame()
{
    if (playButton && !playButton->IsInteractable()) {
        return;
    }

    ApplyPlayerName();
    RTBEngine::Online::OnlineSystem::GetInstance().ClearSessionLobbyBackend();

    if (gameScenePath.empty()) {
        RTB_WARN("MainMenuController: gameScenePath is empty.");
        return;
    }

    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(gameScenePath.c_str());
}

void MainMenuController::GoToMultiplayer()
{
    if (multiplayerButton && !multiplayerButton->IsInteractable()) {
        return;
    }

    ApplyPlayerName();

    if (multiplayerMenuScenePath.empty()) {
        RTB_WARN("MainMenuController: multiplayerMenuScenePath is empty.");
        return;
    }

    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(multiplayerMenuScenePath.c_str());
}
