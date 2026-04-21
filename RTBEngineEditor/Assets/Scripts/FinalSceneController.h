#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/Elements/UIText.h>
#include <string>

class FinalSceneController : public RTBEngine::ECS::Component
{
public:
    FinalSceneController() = default;
    ~FinalSceneController() override = default;

    void OnStart() override;
    void OnValidate() override;

    RTBEngine::UI::UIText* resultText = nullptr;
    std::string winMessage = "You won\nThanks for playing the demo";
    std::string loseMessage = "You lost";
    std::string fallbackMessage = "Thanks for playing the demo";

    RTB_COMPONENT(FinalSceneController)

private:
    void ApplyResult() const;
};
