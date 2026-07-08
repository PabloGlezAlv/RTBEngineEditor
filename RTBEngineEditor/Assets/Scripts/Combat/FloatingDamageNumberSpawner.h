#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

class FloatingDamageNumberSpawner : public RTBEngine::ECS::Component
{
public:
    FloatingDamageNumberSpawner() = default;
    ~FloatingDamageNumberSpawner() override = default;

    HealthComponent* health = nullptr;
    std::string damageNumberPrefabRef = "Assets/Prefabs/Combat/Effects/Floating Damage Number.prefab";
    RTBEngine::Math::Vector3 hitFallbackOffset = RTBEngine::Math::Vector3(0.0f, 1.20f, 0.0f);
    RTBEngine::Math::Vector4 textColor = RTBEngine::Math::Vector4(1.0f, 0.92f, 0.35f, 1.0f);

    RTB_COMPONENT(FloatingDamageNumberSpawner)

public:
    void OnStart() override;
    void OnDestroy() override;

    void SpawnDamageNumber(float amount, const RTBEngine::Math::Vector3& worldPosition);

private:
    RTBEngine::ECS::Prefab* damageNumberPrefab = nullptr;
    std::string damageNumberPoolKey;
    RTBEngine::Core::EventSubscription damageSubscription;

    void ResolvePrefab();
    void RebindDamageSubscription();
    void UnbindDamageSubscription();
    void HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData);
};
