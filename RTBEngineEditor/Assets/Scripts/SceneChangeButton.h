#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/EventSystem/IPointerClickHandler.h>
#include <string>

class SceneChangeButton : public RTBEngine::ECS::Component
                        , public RTBEngine::UI::IPointerClickHandler
{
public:
    SceneChangeButton() = default;
    ~SceneChangeButton() override = default;

    std::string scenePath;

    RTB_COMPONENT(SceneChangeButton)

    void OnPointerClick(const RTBEngine::UI::PointerEventData& eventData) override;
};
