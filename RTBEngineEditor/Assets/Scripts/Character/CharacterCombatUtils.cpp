#include "CharacterCombatUtils.h"

#include "CharacterBase.h"
#include "HealthComponent.h"

#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/PhysicsWorldResolver.h>

#include <cmath>
#include <unordered_set>

namespace CharacterCombatUtils {
    namespace {
        constexpr float kDirectionEpsilon = 0.0001f;

        bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
        {
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
