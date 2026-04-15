#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/EventSystem/IPointerClickHandler.h>

class ApplicationQuitButton : public RTBEngine::ECS::Component
                            , public RTBEngine::UI::IPointerClickHandler
{
public:
    ApplicationQuitButton() = default;
    ~ApplicationQuitButton() override = default;

    RTB_COMPONENT(ApplicationQuitButton)

    void OnPointerClick(const RTBEngine::UI::PointerEventData& eventData) override;
};
