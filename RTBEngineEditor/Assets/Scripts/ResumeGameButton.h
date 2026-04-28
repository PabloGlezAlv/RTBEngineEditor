#pragma once

#include "PauseMenuController.h"

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/EventSystem/IPointerClickHandler.h>

class ResumeGameButton : public RTBEngine::ECS::Component
                       , public RTBEngine::UI::IPointerClickHandler
{
public:
    ResumeGameButton() = default;
    ~ResumeGameButton() override = default;

    PauseMenuController* controller = nullptr;

    RTB_COMPONENT(ResumeGameButton)

    void OnPointerClick(const RTBEngine::UI::PointerEventData& eventData) override;
};
