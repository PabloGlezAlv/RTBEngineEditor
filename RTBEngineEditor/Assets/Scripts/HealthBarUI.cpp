#include "HealthBarUI.h"

#include "HealthComponent.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/UI/Elements/UISlider.h>
#include <algorithm>

using ThisClass = HealthBarUI;

RTB_REGISTER_COMPONENT(HealthBarUI)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_COMPONENT(fillPanel, UIPanel)
    RTB_PROPERTY_COLOR(highHealthColor)
    RTB_PROPERTY_COLOR(midHealthColor)
    RTB_PROPERTY_COLOR(lowHealthColor)
RTB_END_REGISTER(HealthBarUI)

void HealthBarUI::OnStart()
{
    SetUpdateTickEnabled(false);
    ResolveSlider();
    RebindHealthSubscription();
    UpdateVisuals();
}

void HealthBarUI::OnValidate()
{
    ResolveSlider();
    RebindHealthSubscription();
    UpdateVisuals();
}

void HealthBarUI::RefreshBinding()
{
    OnValidate();
}

void HealthBarUI::OnDestroy()
{
    UnsubscribeFromHealth();
}

void HealthBarUI::ResolveSlider()
{
    if (!owner) {
        slider = nullptr;
        return;
    }

    slider = owner->GetComponent<RTBEngine::UI::UISlider>();
}

RTBEngine::UI::UIPanel* HealthBarUI::GetEffectiveFillPanel() const
{
    if (fillPanel) {
        return fillPanel;
    }

    if (slider) {
        return slider->fillPanel;
    }

    return nullptr;
}

void HealthBarUI::RebindHealthSubscription()
{
    if (subscribedHealth == health && healthSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromHealth();

    if (!health) {
        return;
    }

    subscribedHealth = health;
    healthSubscription = health->SubscribeToHealthChanged(
        [this](const HealthComponent::HealthChangedEvent& eventData) {
            ApplyHealthEvent(eventData);
        });
}

void HealthBarUI::UnsubscribeFromHealth()
{
    healthSubscription.Reset();
    subscribedHealth = nullptr;
}

void HealthBarUI::ApplyHealthEvent(const HealthComponent::HealthChangedEvent& eventData)
{
    ResolveSlider();
    if (!slider) {
        return;
    }

    slider->SetNormalizedValue(eventData.normalizedHealth);

    RTBEngine::UI::UIPanel* effectiveFillPanel = GetEffectiveFillPanel();
    if (!effectiveFillPanel) {
        return;
    }

    effectiveFillPanel->SetBackgroundColor(EvaluateFillColor(eventData.normalizedHealth));
}

void HealthBarUI::UpdateVisuals()
{
    if (!health) {
        return;
    }

    ApplyHealthEvent(health->GetHealthChangedEvent());
}

RTBEngine::Math::Vector4 HealthBarUI::EvaluateFillColor(float normalizedHealth) const
{
    const float clamped = std::clamp(normalizedHealth, 0.0f, 1.0f);

    if (clamped <= 0.5f) {
        const float t = clamped / 0.5f;
        return RTBEngine::Math::Lerp(lowHealthColor, midHealthColor, t);
    }

    const float t = (clamped - 0.5f) / 0.5f;
    return RTBEngine::Math::Lerp(midHealthColor, highHealthColor, t);
}
