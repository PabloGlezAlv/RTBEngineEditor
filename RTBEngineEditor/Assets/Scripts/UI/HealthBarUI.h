#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/Elements/UIPanel.h>
#include "HealthComponent.h"

namespace RTBEngine {
    namespace UI {
        class UISlider;
    }
}

class HealthBarUI : public RTBEngine::ECS::Component
{
public:
    HealthBarUI() = default;
    ~HealthBarUI() override = default;

    HealthComponent* health = nullptr;
    RTBEngine::UI::UIPanel* fillPanel = nullptr;
    RTBEngine::Math::Vector4 highHealthColor = RTBEngine::Math::Vector4(0.12f, 0.78f, 0.24f, 1.0f);
    RTBEngine::Math::Vector4 midHealthColor = RTBEngine::Math::Vector4(0.95f, 0.74f, 0.16f, 1.0f);
    RTBEngine::Math::Vector4 lowHealthColor = RTBEngine::Math::Vector4(0.82f, 0.18f, 0.18f, 1.0f);

    void RefreshBinding();

    RTB_COMPONENT(HealthBarUI)

    void OnStart() override;
    void OnValidate() override;
    void OnDestroy() override;

private:
    RTBEngine::UI::UISlider* slider = nullptr;
    HealthComponent* subscribedHealth = nullptr;
    RTBEngine::Core::EventSubscription healthSubscription;

    void ResolveSlider();
    RTBEngine::UI::UIPanel* GetEffectiveFillPanel() const;
    void RebindHealthSubscription();
    void UnsubscribeFromHealth();
    void ApplyHealthEvent(const HealthComponent::HealthChangedEvent& eventData);
    void UpdateVisuals();
    RTBEngine::Math::Vector4 EvaluateFillColor(float normalizedHealth) const;
};
