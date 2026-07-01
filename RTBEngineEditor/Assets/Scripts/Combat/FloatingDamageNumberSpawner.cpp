#include "FloatingDamageNumberSpawner.h"

#include "FloatingDamageNumberLifetime.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/PrefabRegistry.h>

using ThisClass = FloatingDamageNumberSpawner;

namespace {
    constexpr float kHitPointEpsilon = 0.0001f;
}

RTB_REGISTER_COMPONENT(FloatingDamageNumberSpawner)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_ASSET_PATH(damageNumberPrefabRef, "prefab")
    RTB_PROPERTY(hitFallbackOffset)
    RTB_PROPERTY_COLOR(textColor)
RTB_END_REGISTER(FloatingDamageNumberSpawner)

void FloatingDamageNumberSpawner::OnStart()
{
    ResolvePrefab();

    if (!health && owner) {
        health = owner->GetComponent<HealthComponent>();
    }

    RebindDamageSubscription();
}

void FloatingDamageNumberSpawner::OnDestroy()
{
    UnbindDamageSubscription();
}

void FloatingDamageNumberSpawner::ResolvePrefab()
{
    damageNumberPrefab = nullptr;
    if (damageNumberPrefabRef.empty()) {
        return;
    }

    const std::string resolvedPath =
        RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(damageNumberPrefabRef);
    damageNumberPrefab = RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(resolvedPath);
    if (!damageNumberPrefab) {
        RTB_WARN("[FloatingDamageNumberSpawner] Damage number prefab not found: '" + damageNumberPrefabRef + "'.");
    }
}

void FloatingDamageNumberSpawner::RebindDamageSubscription()
{
    UnbindDamageSubscription();

    if (!health) {
        return;
    }

    damageSubscription = health->SubscribeToDamageTaken(
        [this](const HealthComponent::DamageTakenEvent& eventData) {
            HandleDamageTaken(eventData);
        });
}

void FloatingDamageNumberSpawner::UnbindDamageSubscription()
{
    damageSubscription.Reset();
}

void FloatingDamageNumberSpawner::HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData)
{
    if (eventData.damage.amount <= 0.0f || !owner) {
        return;
    }

    RTBEngine::Math::Vector3 spawnPosition = eventData.damage.hitPoint;
    if (spawnPosition.LengthSquared() <= kHitPointEpsilon) {
        spawnPosition = owner->GetWorldPosition() + hitFallbackOffset;
    }

    SpawnDamageNumber(eventData.damage.amount, spawnPosition);
}

void FloatingDamageNumberSpawner::SpawnDamageNumber(float amount, const RTBEngine::Math::Vector3& worldPosition)
{
    if (!owner || amount <= 0.0f || !damageNumberPrefab) {
        return;
    }

    RTBEngine::ECS::GameObject* spawnedNumber =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate(
            *damageNumberPrefab,
            worldPosition,
            RTBEngine::Math::Quaternion::Identity());

    if (!spawnedNumber) {
        RTB_WARN("[FloatingDamageNumberSpawner] Failed to instantiate floating damage number prefab.");
        return;
    }

    auto* lifetime = spawnedNumber->GetComponent<FloatingDamageNumberLifetime>();
    if (!lifetime) {
        lifetime = spawnedNumber->GetComponentInChildren<FloatingDamageNumberLifetime>();
    }

    if (!lifetime) {
        RTB_WARN("[FloatingDamageNumberSpawner] Prefab is missing FloatingDamageNumberLifetime.");
        if (RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene()) {
            scene->RemoveGameObject(spawnedNumber);
        }
        return;
    }

    lifetime->textColor = textColor;
    lifetime->Play(amount, worldPosition);
}
