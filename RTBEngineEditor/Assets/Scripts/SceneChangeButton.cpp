#include "SceneChangeButton.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/UI/Elements/UIButton.h>

using ThisClass = SceneChangeButton;

RTB_REGISTER_COMPONENT(SceneChangeButton)
    RTB_PROPERTY(scenePath)
RTB_END_REGISTER(SceneChangeButton)

void SceneChangeButton::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
    RTBEngine::ECS::GameObject* go = GetOwner();
    if (go) {
        auto* button = go->GetComponent<RTBEngine::UI::UIButton>();
        if (button && !button->IsInteractable()) {
            return;
        }
    }

    if (scenePath.empty()) {
        RTB_WARN("SceneChangeButton: scenePath is empty.");
        return;
    }

    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(scenePath.c_str());
}
