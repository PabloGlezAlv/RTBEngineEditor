#include "PauseMenuController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>

using ThisClass = PauseMenuController;

RTB_REGISTER_COMPONENT(PauseMenuController)
    RTB_PROPERTY_GAMEOBJECT(menuRoot)
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
}

void PauseMenuController::OnUpdate(float /*deltaTime*/)
{
    RTBEngine::Input::InputManager& input = RTBEngine::Input::InputManager::GetInstance();
    if (input.IsKeyJustPressed(RTBEngine::Input::KeyCode::Tab)) {
        TogglePause();
    }
}

void PauseMenuController::TogglePause()
{
    if (menuVisible || RTBEngine::Core::Time::IsPaused()) {
        ResumeGame();
        return;
    }

    PauseGame();
}

void PauseMenuController::PauseGame()
{
    CaptureMenuElementStates();
    SetMenuVisible(true);
    RTBEngine::Core::Time::SetPaused(true);
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);
}

void PauseMenuController::ResumeGame()
{
    SetMenuVisible(false);
    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(true);
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
        menuVisible = false;
        return;
    }

    CaptureMenuElementStates();

    for (UIElementState& state : capturedElementStates) {
        if (!state.element) {
            continue;
        }

        state.element->SetVisible(visible ? state.visible : false);
        state.element->SetRaycastTarget(visible ? state.raycastTarget : false);
    }

    menuVisible = visible;
}
