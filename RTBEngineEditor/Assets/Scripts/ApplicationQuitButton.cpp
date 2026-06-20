#include "ApplicationQuitButton.h"

#include <RTBEngine/Core/Application.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/UI/Elements/UIButton.h>

using ThisClass = ApplicationQuitButton;

RTB_REGISTER_COMPONENT(ApplicationQuitButton)
RTB_END_REGISTER(ApplicationQuitButton)

void ApplicationQuitButton::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
    RTBEngine::ECS::GameObject* go = GetOwner();
    if (go) {
        auto* button = go->GetComponent<RTBEngine::UI::UIButton>();
        if (button && !button->IsInteractable()) {
            return;
        }
    }

    RTBEngine::Core::Application::RequestQuit();
}
