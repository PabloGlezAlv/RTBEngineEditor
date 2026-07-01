#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>
#include <vector>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

class ComicOuchSpawner : public RTBEngine::ECS::Component
{
public:
    ComicOuchSpawner() = default;
    ~ComicOuchSpawner() override = default;

    HealthComponent* health = nullptr;
    std::string bubblePrefabRef = "Assets/Prefabs/Combat/Effects/Comic Ouch Bubble.prefab";
    std::vector<std::string> bubbleTextures = {
        "Assets/UI/Comic/Ouch/ouch_01.png",
        "Assets/UI/Comic/Ouch/ow_01.png",
        "Assets/UI/Comic/Ouch/ugh_01.png"
    };
    RTBEngine::Math::Vector3 hitFallbackOffset = RTBEngine::Math::Vector3(0.0f, 1.20f, 0.0f);
    float cooldown = 0.15f;

    RTB_COMPONENT(ComicOuchSpawner)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

private:
    RTBEngine::ECS::Prefab* bubblePrefab = nullptr;
    RTBEngine::Core::EventSubscription damageSubscription;
    float cooldownRemaining = 0.0f;
    size_t nextTextureIndex = 0;

    void ResolvePrefab();
    void EnsureDefaultTextures();
    void RebindDamageSubscription();
    void UnbindDamageSubscription();
    void HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData);
    void SpawnBubble(const RTBEngine::Math::Vector3& worldPosition);
};
