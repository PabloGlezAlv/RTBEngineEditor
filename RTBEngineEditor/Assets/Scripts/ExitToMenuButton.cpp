#include "ExitToMenuButton.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIButton.h>

using ThisClass = ExitToMenuButton;

RTB_REGISTER_COMPONENT(ExitToMenuButton)
    RTB_PROPERTY_COMPONENT(controller, PauseMenuController)
RTB_END_REGISTER(ExitToMenuButton)

void ExitToMenuButton::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
    RTBEngine::ECS::GameObject* go = GetOwner();
    if (go) {
        auto* button = go->GetComponent<RTBEngine::UI::UIButton>();
        if (button && !button->IsInteractable()) {
            return;
        }
    }

    PauseMenuController* targetController = controller;
    if (!targetController) {
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (scene) {
            for (const auto& gameObject : scene->GetGameObjects()) {
                if (!gameObject) {
                    continue;
                }

                targetController = gameObject->GetComponent<PauseMenuController>();
                if (targetController) {
                    break;
                }
            }
        }
    }

    if (targetController) {
        targetController->ExitToMainMenu();
    }
}
