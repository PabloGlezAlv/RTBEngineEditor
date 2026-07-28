#include "CharacterCombatUtils.h"

#include "CharacterBase.h"
#include "HealthComponent.h"

#include <RTBEngine/Physics/PhysicsLayerSettings.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/PhysicsWorldResolver.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <unordered_set>

namespace CharacterCombatUtils {
    namespace {
        constexpr float kMinClipLength = 0.05f;
        constexpr float kGroundCastSkipEpsilon = 0.02f;
        constexpr int kMaxEnvironmentCastIterations = 16;

        bool IsWalkableGroundHit(const RTBEngine::Physics::PhysicsQueryHit& hit)
        {
            return hit.normal.y > 0.65f;
        }

        bool PassesTeamFilter(
            RTBEngine::Scene::GameObject* instigator,
            RTBEngine::Scene::GameObject* targetRoot,
            bool ignoreSameTeam)
        {
            if (!instigator || !targetRoot || instigator == targetRoot) {
                return false;
            }

            const int instigatorTeam = ResolveCharacterTeam(instigator);
            const int targetTeam = ResolveCharacterTeam(targetRoot);
            return !(ignoreSameTeam &&
                instigatorTeam != static_cast<int>(CharacterTeam::Neutral) &&
                instigatorTeam == targetTeam);
        }
    }

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        constexpr float kDirectionEpsilon = 0.0001f;
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    RTBEngine::Math::Vector3 NormalizePlanarDirection(RTBEngine::Math::Vector3 direction)
    {
        direction.y = 0.0f;
        if (!HasPlanarDirection(direction)) {
            return {};
        }

        direction.Normalize();
        return direction;
    }

    int ResolveCharacterTeam(RTBEngine::Scene::GameObject* gameObject)
    {
        if (!gameObject) {
            return static_cast<int>(CharacterTeam::Neutral);
        }

        for (RTBEngine::Scene::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam();
            }
        }

        return static_cast<int>(CharacterTeam::Neutral);
    }

    RTBEngine::Scene::GameObject* ResolveHealthRoot(RTBEngine::Scene::GameObject* gameObject)
    {
        if (!gameObject) {
            return nullptr;
        }

        for (RTBEngine::Scene::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (current->GetComponent<HealthComponent>()) {
                return current;
            }
        }

        return nullptr;
    }

    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld(RTBEngine::Scene::GameObject* gameObject)
    {
        return RTBEngine::Scene::ResolvePhysicsWorldFromGameObject(gameObject, true);
    }

    std::uint32_t GetPhysicsLayerBit(const char* layerName)
    {
        static const std::uint32_t defaultBit = 1u << static_cast<std::uint32_t>(std::max(
            0,
            RTBEngine::Physics::PhysicsLayerSettings::Get().GetLayerIndex("Default")));
        static const std::uint32_t charactersBit = 1u << static_cast<std::uint32_t>(std::max(
            0,
            RTBEngine::Physics::PhysicsLayerSettings::Get().GetLayerIndex("Characters")));

        if (layerName) {
            if (std::strcmp(layerName, "Default") == 0) {
                return defaultBit;
            }
            if (std::strcmp(layerName, "Characters") == 0) {
                return charactersBit;
            }
        }

        const int layerIndex =
            RTBEngine::Physics::PhysicsLayerSettings::Get().GetLayerIndex(layerName ? layerName : "Default");
        return 1u << static_cast<std::uint32_t>(std::max(0, layerIndex));
    }

    float ResolvePlanarEnvironmentClipLength(const PlanarEnvironmentClipQuery& query)
    {
        if (!query.physicsWorld || !query.instigator || query.maxLength <= 0.0f || query.castRadius <= 0.0f) {
            return std::max(0.0f, query.maxLength);
        }

        const RTBEngine::Math::Vector3 castDirection = NormalizePlanarDirection(query.direction);
        if (!HasPlanarDirection(castDirection)) {
            return 0.0f;
        }

        const std::uint32_t environmentLayerMask = query.layerMask != 0u
            ? query.layerMask
            : GetPhysicsLayerBit("Default");

        RTBEngine::Physics::PhysicsQueryOptions options;
        options.ignoredObject = query.instigator;
        options.ignoreIgnoredObjectHierarchy = true;
        options.ignoreTriggers = true;
        options.layerMask = environmentLayerMask;

        float traveled = 0.0f;
        for (int iteration = 0; iteration < kMaxEnvironmentCastIterations; ++iteration) {
            const float remaining = query.maxLength - traveled;
            if (remaining <= kMinClipLength) {
                return std::max(traveled, kMinClipLength);
            }

            const RTBEngine::Math::Vector3 castStart = query.origin + castDirection * traveled;
            const RTBEngine::Math::Vector3 castEnd = query.origin + castDirection * query.maxLength;

            RTBEngine::Physics::PhysicsQueryHit hit;
            if (!query.physicsWorld->SphereCastClosest(castStart, castEnd, query.castRadius, hit, options)) {
                return query.maxLength;
            }

            const float hitDistance = std::clamp(hit.fraction, 0.0f, 1.0f) * remaining;
            if (IsWalkableGroundHit(hit)) {
                traveled += std::max(hitDistance, kGroundCastSkipEpsilon);
                if (traveled >= query.maxLength) {
                    return query.maxLength;
                }
                continue;
            }

            return std::clamp(traveled + hitDistance, kMinClipLength, query.maxLength);
        }

        return query.maxLength;
    }

    std::vector<HostileOverlapHit> OverlapHostileTargets(const HostileOverlapQuery& query)
    {
        std::vector<HostileOverlapHit> results;

        if (!query.physicsWorld || !query.instigator || query.distance <= 0.0f || query.radius <= 0.0f) {
            return results;
        }

        const RTBEngine::Math::Vector3 castDirection = NormalizePlanarDirection(query.direction);
        if (!HasPlanarDirection(castDirection)) {
            return results;
        }

        const RTBEngine::Math::Vector3 castEnd = query.origin + castDirection * query.distance;

        RTBEngine::Physics::PhysicsQueryOptions physicsOptions;
        physicsOptions.ignoredObject = query.instigator;
        physicsOptions.ignoreIgnoredObjectHierarchy = true;
        physicsOptions.ignoreTriggers = true;

        const std::vector<RTBEngine::Physics::OverlapSphereHit> physicsHits =
            query.physicsWorld->OverlapCapsuleSegment(
                query.origin,
                castEnd,
                query.radius,
                query.layerMask,
                physicsOptions);

        std::unordered_set<RTBEngine::Scene::GameObject*> seenTargets;
        seenTargets.reserve(physicsHits.size());

        for (const RTBEngine::Physics::OverlapSphereHit& physicsHit : physicsHits) {
            if (!physicsHit.gameObject) {
                continue;
            }

            HealthComponent* targetHealth = physicsHit.gameObject->GetComponent<HealthComponent>();
            if (!targetHealth) {
                targetHealth = physicsHit.gameObject->GetComponentInChildren<HealthComponent>();
            }
            if (!targetHealth || targetHealth->IsDead()) {
                continue;
            }

            RTBEngine::Scene::GameObject* targetRoot = ResolveHealthRoot(targetHealth->GetOwner());
            if (!targetRoot || !PassesTeamFilter(query.instigator, targetRoot, query.ignoreSameTeam)) {
                continue;
            }

            if (!seenTargets.insert(targetRoot).second) {
                continue;
            }

            HostileOverlapHit hostileHit;
            hostileHit.targetRoot = targetRoot;
            hostileHit.health = targetHealth;
            hostileHit.hitPoint = physicsHit.point;
            hostileHit.hitNormal = physicsHit.normal;
            results.push_back(hostileHit);
        }

        return results;
    }

}
