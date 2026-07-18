#pragma once

#include "PauseMenuController.h"

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/EventSystem/IPointerClickHandler.h>

class ExitToMenuButton : public RTBEngine::Scene::Component
                       , public RTBEngine::UI::IPointerClickHandler
{
public:
    ExitToMenuButton() = default;
    ~ExitToMenuButton() override = default;

    PauseMenuController* controller = nullptr;

    RTB_COMPONENT(ExitToMenuButton)

    void OnPointerClick(const RTBEngine::UI::PointerEventData& eventData) override;
};
