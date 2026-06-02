#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/Elements/UIText.h>

class HealthBarUI;

class PlayerNameplateUI : public RTBEngine::ECS::Component
{
public:
    PlayerNameplateUI() = default;
    ~PlayerNameplateUI() override = default;

    RTBEngine::UI::UIText* displayNameText = nullptr;
    HealthBarUI* healthBarUI = nullptr;

    RTB_COMPONENT(PlayerNameplateUI)

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

private:
    float refreshTimer = 0.0f;

    void RefreshDisplayName() const;
    void BindHealthBar();
    bool ShouldStopRefreshing() const;
};
