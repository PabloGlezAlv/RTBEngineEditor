#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <vector>

namespace RTBEngine {
    namespace ECS {
        class MeshRenderer;
    }
}

class HitFlashComponent : public RTBEngine::ECS::Component
{
public:
    HitFlashComponent() = default;
    ~HitFlashComponent() override = default;

    HealthComponent* health = nullptr;
    RTBEngine::Math::Vector4 flashColor = RTBEngine::Math::Vector4(2.5f, 0.50f, 0.45f, 1.0f);
    float flashDuration = 0.15f;
    bool includeChildren = true;

    RTB_COMPONENT(HitFlashComponent)

public:
    void OnStart() override;
    void OnLateUpdate(float deltaTime) override;
    void OnDestroy() override;
    void OnValidate() override;

    void TriggerFlash();

private:
    struct FlashTarget {
        RTBEngine::ECS::MeshRenderer* renderer = nullptr;
        RTBEngine::Math::Vector4 baseColor = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        RTBEngine::Math::Vector3 baseDiffuseColor = RTBEngine::Math::Vector3(1.0f, 1.0f, 1.0f);
    };

    std::vector<FlashTarget> flashTargets;
    RTBEngine::Core::EventSubscription damageSubscription;
    float flashTimeRemaining = 0.0f;

    void RebindDamageSubscription();
    void UnbindDamageSubscription();
    void CollectFlashTargets();
    void HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData);
    void BeginFlash();
    void ApplyFlashStrength(float strength);
    void RestoreBaseColors();
    void PushColorToRenderer(FlashTarget& target, const RTBEngine::Math::Vector4& color);
};
