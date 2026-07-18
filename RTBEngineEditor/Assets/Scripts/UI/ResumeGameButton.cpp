#include "ResumeGameButton.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/UI/Elements/UIButton.h>

using ThisClass = ResumeGameButton;

RTB_REGISTER_COMPONENT(ResumeGameButton)
    RTB_PROPERTY_COMPONENT(controller, PauseMenuController)
RTB_END_REGISTER(ResumeGameButton)

void ResumeGameButton::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
    RTBEngine::Scene::GameObject* go = GetOwner();
    if (go) {
        auto* button = go->GetComponent<RTBEngine::UI::UIButton>();
        if (button && !button->IsInteractable()) {
            return;
        }
    }

    if (controller) {
        controller->ResumeGame();
    }
}
