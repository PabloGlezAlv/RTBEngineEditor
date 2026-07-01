#include "MultiplayerMenuController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/Online/OnlineTypes.h>
#include <RTBEngine/UI/Elements/UIButton.h>

using ThisClass = MultiplayerMenuController;

RTB_REGISTER_COMPONENT(MultiplayerMenuController)
    RTB_PROPERTY_COMPONENT(lanLobbyButton, UIButton)
    RTB_PROPERTY_COMPONENT(onlineLobbyButton, UIButton)
    RTB_PROPERTY_COMPONENT(backButton, UIButton)
    RTB_PROPERTY_ASSET_PATH(lobbyScenePath, "lua")
    RTB_PROPERTY_ASSET_PATH(mainMenuScenePath, "lua")
RTB_END_REGISTER(MultiplayerMenuController)

void MultiplayerMenuController::OnStart()
{
    BindButtons();
    RefreshButtonState();
}

void MultiplayerMenuController::RefreshButtonState()
{
    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    const bool onlineReady = online.IsInitialized();

    if (lanLobbyButton) {
        lanLobbyButton->SetInteractable(onlineReady && online.IsLanLobbyReady());
    }

    if (onlineLobbyButton) {
        onlineLobbyButton->SetInteractable(onlineReady && online.IsRelayLobbyReady());
    }
}

void MultiplayerMenuController::OnDestroy()
{
    callbacksBound = false;
    lanLobbyButton = nullptr;
    onlineLobbyButton = nullptr;
    backButton = nullptr;
}

void MultiplayerMenuController::BindButtons()
{
    if (callbacksBound) {
        return;
    }

    if (lanLobbyButton) {
        lanLobbyButton->SetOnClick([this]() { GoToLobby(false); });
    }

    if (onlineLobbyButton) {
        onlineLobbyButton->SetOnClick([this]() { GoToLobby(true); });
    }

    if (backButton) {
        backButton->SetOnClick([this]() { GoToMainMenu(); });
    }

    callbacksBound = lanLobbyButton != nullptr ||
        onlineLobbyButton != nullptr ||
        backButton != nullptr;
}

void MultiplayerMenuController::GoToLobby(bool useRelayLobby)
{
    RTBEngine::UI::UIButton* sourceButton = useRelayLobby ? onlineLobbyButton : lanLobbyButton;
    if (sourceButton && !sourceButton->IsInteractable()) {
        return;
    }

    if (lobbyScenePath.empty()) {
        RTB_WARN("MultiplayerMenuController: lobbyScenePath is empty.");
        return;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    online.SetSessionLobbyBackend(
        useRelayLobby
            ? RTBEngine::Online::OnlineBackendType::RelayOnline
            : RTBEngine::Online::OnlineBackendType::Lan);

    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(lobbyScenePath.c_str());
}

void MultiplayerMenuController::GoToMainMenu()
{
    if (backButton && !backButton->IsInteractable()) {
        return;
    }

    if (mainMenuScenePath.empty()) {
        RTB_WARN("MultiplayerMenuController: mainMenuScenePath is empty.");
        return;
    }

    RTBEngine::Online::OnlineSystem::GetInstance().ClearSessionLobbyBackend();
    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(mainMenuScenePath.c_str());
}
