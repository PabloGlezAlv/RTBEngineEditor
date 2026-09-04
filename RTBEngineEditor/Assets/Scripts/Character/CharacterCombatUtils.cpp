#include "CharacterCombatUtils.h"

#include "CharacterBase.h"
#include "HealthComponent.h"

#include <RTBEngine/Physics/PhysicsLayerSettings.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Physics/RigidBody.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/PhysicsWorldResolver.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>

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

        int ResolveTeam(const CombatTarget& target)
        {
            return target.character
                ? target.character->GetTeam()
                : static_cast<int>(CharacterTeam::Neutral);
        }

        bool PassesTeamFilter(
            RTBEngine::Scene::GameObject* instigator,
            const CombatTarget& target,
            int instigatorTeam,
            bool ignoreSameTeam)
        {
            if (!instigator || !target.root || instigator == target.root) {
                return false;
            }

            const int targetTeam = ResolveTeam(target);
            return !(ignoreSameTeam &&
                instigatorTeam != static_cast<int>(CharacterTeam::Neutral) &&
                instigatorTeam == targetTeam);
        }

        void CollectHostileHits(
            const std::vector<RTBEngine::Physics::OverlapSphereHit>& physicsHits,
            RTBEngine::Scene::GameObject* instigator,
            bool ignoreSameTeam,
            std::vector<HostileOverlapHit>& results)
        {
            const CombatTarget instigatorTarget = ResolveCombatTarget(instigator);
            const int instigatorTeam = ResolveTeam(instigatorTarget);

            std::unordered_set<RTBEngine::Scene::GameObject*> seenTargets;
            seenTargets.reserve(physicsHits.size());

            for (const RTBEngine::Physics::OverlapSphereHit& physicsHit : physicsHits) {
                if (!physicsHit.gameObject) {
                    continue;
                }

                const CombatTarget target = ResolveCombatTarget(physicsHit.gameObject);
                if (!target.health || target.health->IsDead()) {
                    continue;
                }

                if (!PassesTeamFilter(instigator, target, instigatorTeam, ignoreSameTeam)) {
                    continue;
                }

                if (!seenTargets.insert(target.root).second) {
                    continue;
                }

                HostileOverlapHit hostileHit;
                hostileHit.targetRoot = target.root;
                hostileHit.health = target.health;
                hostileHit.hitPoint = physicsHit.point;
                hostileHit.hitNormal = physicsHit.normal;
                results.push_back(hostileHit);
            }
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

    CombatTarget ResolveCombatTarget(RTBEngine::Scene::GameObject* gameObject)
    {
        CombatTarget result;
        if (!gameObject) {
            return result;
        }

        HealthComponent* nearestHealth = nullptr;
        RTBEngine::Scene::GameObject* nearestHealthRoot = nullptr;

        for (RTBEngine::Scene::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (CharacterBase* character = current->GetComponent<CharacterBase>()) {
                result.root = current;
                result.character = character;
                result.health = character->GetHealth();
                if (!result.health) {
                    result.health = current->GetComponent<HealthComponent>();
                }
                if (!result.health) {
                    result.health = nearestHealth;
                }
                return result;
            }

            if (!nearestHealth) {
                if (HealthComponent* health = current->GetComponent<HealthComponent>()) {
                    nearestHealth = health;
                    nearestHealthRoot = current;
                }
            }
        }

        if (nearestHealth) {
            result.root = nearestHealthRoot;
            result.health = nearestHealth;
            return result;
        }

        if (HealthComponent* childHealth = gameObject->GetComponentInChildren<HealthComponent>()) {
            result.root = childHealth->GetOwner();
            result.health = childHealth;
        }

        return result;
    }

    int ResolveCharacterTeam(RTBEngine::Scene::GameObject* gameObject)
    {
        const CombatTarget target = ResolveCombatTarget(gameObject);
        return target.character
            ? target.character->GetTeam()
            : static_cast<int>(CharacterTeam::Neutral);
    }

    RTBEngine::Scene::GameObject* ResolveHealthRoot(RTBEngine::Scene::GameObject* gameObject)
    {
        return ResolveCombatTarget(gameObject).root;
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

        CollectHostileHits(physicsHits, query.instigator, query.ignoreSameTeam, results);
        return results;
    }

    std::vector<HostileOverlapHit> OverlapHostileTargetsInSphere(const HostileSphereOverlapQuery& query)
    {
        std::vector<HostileOverlapHit> results;

        if (!query.physicsWorld || !query.instigator || query.radius <= 0.0f) {
            return results;
        }

        RTBEngine::Physics::PhysicsQueryOptions physicsOptions;
        physicsOptions.ignoredObject = query.instigator;
        physicsOptions.ignoreIgnoredObjectHierarchy = true;
        physicsOptions.ignoreTriggers = true;

        const std::vector<RTBEngine::Physics::OverlapSphereHit> physicsHits =
            query.physicsWorld->OverlapSphere(
                query.center,
                query.radius,
                query.layerMask,
                physicsOptions);

        CollectHostileHits(physicsHits, query.instigator, query.ignoreSameTeam, results);
        return results;
    }

    RTBEngine::Math::Vector3 ResolveColliderCenterOffset(RTBEngine::Scene::GameObject* actor)
    {
        return CharacterCombatOrigins::ResolveColliderBody(actor).centerOffset;
    }

    ActorPhysicsPose ResolveActorPhysicsPose(RTBEngine::Scene::GameObject* actor)
    {
        ActorPhysicsPose pose;
        if (!actor) {
            return pose;
        }

        pose.rigidBodyComponent = actor->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
        pose.collider = CharacterCombatOrigins::ResolveColliderBody(actor);
        if (pose.rigidBodyComponent &&
            pose.rigidBodyComponent->HasRigidBody() &&
            pose.rigidBodyComponent->GetRigidBody()) {
            pose.physicsWorld = pose.rigidBodyComponent->GetRigidBody()->GetPhysicsWorld();
        }
        return pose;
    }

    void SetActorWorldPosition(
        RTBEngine::Scene::GameObject* actor,
        const RTBEngine::Math::Vector3& position,
        const RTBEngine::Math::Quaternion& rotation,
        const ActorPhysicsPose* cachedPose)
    {
        if (!actor) {
            return;
        }

        actor->GetTransform().SetPosition(position);
        actor->GetTransform().SetRotation(rotation);

        const ActorPhysicsPose localPose = cachedPose ? *cachedPose : ResolveActorPhysicsPose(actor);
        if (!localPose.rigidBodyComponent || !localPose.rigidBodyComponent->HasRigidBody()) {
            return;
        }

        RTBEngine::Physics::RigidBody* rigidBody = localPose.rigidBodyComponent->GetRigidBody();
        if (!rigidBody) {
            return;
        }

        rigidBody->SetLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        rigidBody->SetWorldTransform(
            position + (rotation * localPose.collider.centerOffset),
            rotation);
    }

}
