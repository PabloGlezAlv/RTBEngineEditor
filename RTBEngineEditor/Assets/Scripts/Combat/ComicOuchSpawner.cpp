#include "ComicOuchSpawner.h"

#include "ComicBubbleLifetime.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/PrefabRegistry.h>

#include <algorithm>
#include <cmath>

using ThisClass = ComicOuchSpawner;

namespace {
    constexpr float kHitPointEpsilon = 0.0001f;
}

RTB_REGISTER_COMPONENT(ComicOuchSpawner)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_ASSET_PATH(bubblePrefabRef, "prefab")
    RTB_PROPERTY_STRING_LIST(bubbleTextures)
    RTB_PROPERTY(hitFallbackOffset)
    RTB_PROPERTY_RANGE(cooldown, 0.0f, 5.0f)
RTB_END_REGISTER(ComicOuchSpawner)

void ComicOuchSpawner::OnStart()
{
    EnsureDefaultTextures();
    ResolvePrefab();

    if (!health && owner) {
        health = owner->GetComponent<HealthComponent>();
    }

    RebindDamageSubscription();
}

void ComicOuchSpawner::OnUpdate(float deltaTime)
{
    if (cooldownRemaining > 0.0f) {
        cooldownRemaining = std::max(0.0f, cooldownRemaining - std::max(0.0f, deltaTime));
    }
}

void ComicOuchSpawner::OnDestroy()
{
    UnbindDamageSubscription();
}

void ComicOuchSpawner::EnsureDefaultTextures()
{
    if (!bubbleTextures.empty()) {
        return;
    }

    bubbleTextures = {
        "Assets/UI/Comic/Ouch/ouch_01.png",
        "Assets/UI/Comic/Ouch/ow_01.png",
        "Assets/UI/Comic/Ouch/ugh_01.png"
    };
}

void ComicOuchSpawner::ResolvePrefab()
{
    bubblePrefab = nullptr;
    if (bubblePrefabRef.empty()) {
        return;
    }

    const std::string resolvedPath =
        RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(bubblePrefabRef);
    bubblePrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(resolvedPath);
    if (!bubblePrefab) {
        RTB_WARN("[ComicOuchSpawner] Bubble prefab not found: '" + bubblePrefabRef + "'.");
    }
}

void ComicOuchSpawner::RebindDamageSubscription()
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

void ComicOuchSpawner::UnbindDamageSubscription()
{
    damageSubscription.Reset();
}

void ComicOuchSpawner::HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData)
{
    if (eventData.damage.amount <= 0.0f) {
        return;
    }

    if (cooldownRemaining > 0.0f) {
        return;
    }

    cooldownRemaining = std::max(0.0f, cooldown);

    RTBEngine::Math::Vector3 spawnPosition = eventData.damage.hitPoint;
    if (spawnPosition.LengthSquared() <= kHitPointEpsilon) {
        spawnPosition = owner->GetWorldPosition() + hitFallbackOffset;
    }

    SpawnBubble(spawnPosition);
}

void ComicOuchSpawner::SpawnBubble(const RTBEngine::Math::Vector3& worldPosition)
{
    if (!bubblePrefab || bubbleTextures.empty()) {
        return;
    }

    const std::string& texturePath = bubbleTextures[nextTextureIndex % bubbleTextures.size()];
    nextTextureIndex = (nextTextureIndex + 1) % bubbleTextures.size();

    RTBEngine::Scene::GameObject* spawnedBubble =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(
            *bubblePrefab,
            worldPosition,
            RTBEngine::Math::Quaternion::Identity());

    if (!spawnedBubble) {
        RTB_WARN("[ComicOuchSpawner] Failed to instantiate comic bubble prefab.");
        return;
    }

    auto* lifetime = spawnedBubble->GetComponent<ComicBubbleLifetime>();
    if (!lifetime) {
        lifetime = spawnedBubble->GetComponentInChildren<ComicBubbleLifetime>();
    }

    if (!lifetime) {
        RTB_WARN("[ComicOuchSpawner] Comic bubble prefab is missing ComicBubbleLifetime.");
        if (RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
            scene->RemoveGameObject(spawnedBubble);
        }
        return;
    }

    lifetime->Play(texturePath, worldPosition);
}
