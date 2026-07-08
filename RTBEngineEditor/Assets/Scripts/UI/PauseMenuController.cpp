#include "PauseMenuController.h"

#include "OnlineGameNetMessages.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Scene/ComponentQuery.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIText.h>

using ThisClass = PauseMenuController;

namespace {
    int g_openPauseMenus = 0;
}

RTB_REGISTER_COMPONENT(PauseMenuController)
    RTB_PROPERTY_GAMEOBJECT(menuRoot)
    RTB_PROPERTY_COMPONENT(resumeButton, UIButton)
    RTB_PROPERTY_COMPONENT(exitButton, UIButton)
    RTB_PROPERTY_COMPONENT(notificationText, UIText)
    RTB_PROPERTY(pauseSimulation)
    RTB_PROPERTY(useRelativeMouseWhenClosed)
    RTB_PROPERTY_ASSET_PATH(mainMenuScenePath, "lua")
RTB_END_REGISTER(PauseMenuController)

void PauseMenuController::OnAwake()
{
    SetTimeMode(RTBEngine::ECS::ComponentTimeMode::Unscaled);
    SetUpdateTickEnabled(true);
}

void PauseMenuController::OnStart()
{
    CaptureMenuElementStates();
    SetMenuVisible(false);
    BindButtons();
    RefreshMatchNotification();
    ApplyMouseModeForMenuState();
}

void PauseMenuController::OnUpdate(float deltaTime)
{
    RTBEngine::Input::InputManager& input = RTBEngine::Input::InputManager::GetInstance();
    if (input.IsKeyJustPressed(RTBEngine::Input::KeyCode::Tab)) {
        TogglePause();
    }

    GameNet::OnlineGameNetSubsystem::TickMatchNotifications(deltaTime);
    RefreshMatchNotification();
}

void PauseMenuController::OnValidate()
{
    BindButtons();
}

void PauseMenuController::OnDestroy()
{
    if (menuVisible && g_openPauseMenus > 0) {
        --g_openPauseMenus;
    }

    if (pauseSimulation && RTBEngine::Core::Time::IsPaused()) {
        RTBEngine::Core::Time::SetPaused(false);
    }

    resumeButtonBound = false;
    exitButtonBound = false;
    resumeButton = nullptr;
    exitButton = nullptr;
    notificationText = nullptr;
    menuRoot = nullptr;
}

void PauseMenuController::BindButtons()
{
    if (resumeButtonBound && exitButtonBound) {
        return;
    }

    if (!resumeButtonBound && resumeButton) {
        resumeButton->SetOnClick([this]() { ResumeGame(); });
        resumeButtonBound = true;
    }

    if (!exitButtonBound && exitButton) {
        exitButton->SetOnClick([this]() { ExitToMainMenu(); });
        exitButtonBound = true;
    }
}

void PauseMenuController::TogglePause()
{
    if (menuVisible) {
        ResumeGame();
        return;
    }

    if (pauseSimulation && RTBEngine::Core::Time::IsPaused()) {
        ResumeGame();
        return;
    }

    PauseGame();
}

void PauseMenuController::PauseGame()
{
    CaptureMenuElementStates();
    SetMenuVisible(true);
    if (pauseSimulation) {
        RTBEngine::Core::Time::SetPaused(true);
    }
    ApplyMouseModeForMenuState();
}

void PauseMenuController::ResumeGame()
{
    SetMenuVisible(false);
    if (pauseSimulation) {
        RTBEngine::Core::Time::SetPaused(false);
    }
    ApplyMouseModeForMenuState();
}

void PauseMenuController::ExitToMainMenu()
{
    ResumeGame();

    if (mainMenuScenePath.empty()) {
        RTB_WARN("[PauseMenuController] mainMenuScenePath is empty.");
        return;
    }

    GameNet::OnlineGameNetSubsystem::RequestExitMatch(mainMenuScenePath.c_str());
}

void PauseMenuController::ApplyMouseModeForMenuState()
{
    const bool menuOpen = menuVisible;
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(
        !menuOpen && useRelativeMouseWhenClosed);
}

void PauseMenuController::RefreshMatchNotification()
{
    if (!notificationText) {
        return;
    }

    std::string message;
    float secondsRemaining = 0.0f;
    if (!GameNet::OnlineGameNetSubsystem::TryGetActiveMatchNotification(message, secondsRemaining)) {
        notificationText->SetVisible(false);
        return;
    }

    notificationText->SetText(message);
    notificationText->SetVisible(true);
}

void PauseMenuController::CaptureMenuElementStates()
{
    if (capturedInitialState) {
        return;
    }

    if (!menuRoot) {
        RTB_WARN("[PauseMenuController] Assign menuRoot to show the pause menu.");
        capturedInitialState = true;
        return;
    }

    capturedElementStates.clear();
    CaptureMenuElementStatesRecursive(menuRoot);
    capturedInitialState = true;
}

void PauseMenuController::CaptureMenuElementStatesRecursive(RTBEngine::ECS::GameObject* root)
{
    if (!root) {
        return;
    }

    if (auto* element = root->GetComponent<RTBEngine::UI::UIElement>()) {
        capturedElementStates.push_back({ element, element->IsVisible(), element->IsRaycastTarget() });
    }

    for (RTBEngine::ECS::GameObject* child : root->GetChildren()) {
        CaptureMenuElementStatesRecursive(child);
    }
}

void PauseMenuController::SetMenuVisible(bool visible)
{
    if (!menuRoot) {
        if (menuVisible && g_openPauseMenus > 0) {
            --g_openPauseMenus;
        }
        menuVisible = false;
        return;
    }

    if (menuVisible == visible) {
        return;
    }

    CaptureMenuElementStates();
    menuRoot->SetActive(visible);

    for (UIElementState& state : capturedElementStates) {
        if (!state.element) {
            continue;
        }

        state.element->SetVisible(visible ? state.visible : false);
        state.element->SetRaycastTarget(visible ? state.raycastTarget : false);
    }

    if (visible) {
        ++g_openPauseMenus;
    } else if (g_openPauseMenus > 0) {
        --g_openPauseMenus;
    }

    menuVisible = visible;
}

bool PauseMenuController::IsAnyMenuOpen()
{
    return g_openPauseMenus > 0;
}

void PauseMenuController::ApplyGameplayMouseMode()
{
    if (PauseMenuController* controller =
            RTBEngine::ECS::ComponentQuery::FindFirst<PauseMenuController>()) {
        controller->ApplyMouseModeForMenuState();
    }
}
