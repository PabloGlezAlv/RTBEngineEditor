#include "ExitToMenuButton.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/UI/Elements/UIButton.h>

using ThisClass = ExitToMenuButton;

RTB_REGISTER_COMPONENT(ExitToMenuButton)
    RTB_PROPERTY_COMPONENT(controller, PauseMenuController)
RTB_END_REGISTER(ExitToMenuButton)

void ExitToMenuButton::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
    RTBEngine::Scene::GameObject* go = GetOwner();
    if (go) {
        auto* button = go->GetComponent<RTBEngine::UI::UIButton>();
        if (button && !button->IsInteractable()) {
            return;
        }
    }

    if (controller) {
        controller->ExitToMainMenu();
    }
}
