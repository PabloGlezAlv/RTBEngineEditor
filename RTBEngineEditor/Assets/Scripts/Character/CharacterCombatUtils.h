#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

#include <cstdint>
#include <vector>

class CharacterBase;
class HealthComponent;

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

namespace CharacterCombatUtils {

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value);
    RTBEngine::Math::Vector3 NormalizePlanarDirection(RTBEngine::Math::Vector3 direction);

    int ResolveCharacterTeam(RTBEngine::Scene::GameObject* gameObject);

    RTBEngine::Scene::GameObject* ResolveHealthRoot(RTBEngine::Scene::GameObject* gameObject);

    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld(RTBEngine::Scene::GameObject* gameObject);

    std::uint32_t GetPhysicsLayerBit(const char* layerName);

    struct PlanarEnvironmentClipQuery {
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Scene::GameObject* instigator = nullptr;
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f);
        RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3(0.0f, 0.0f, 1.0f);
        float maxLength = 0.0f;
        float castRadius = 0.45f;
        std::uint32_t layerMask = 0u;
    };

    float ResolvePlanarEnvironmentClipLength(const PlanarEnvironmentClipQuery& query);

    struct HostileOverlapHit {
        RTBEngine::Scene::GameObject* targetRoot = nullptr;
        HealthComponent* health = nullptr;
        RTBEngine::Math::Vector3 hitPoint;
        RTBEngine::Math::Vector3 hitNormal;
    };

    struct HostileOverlapQuery {
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Scene::GameObject* instigator = nullptr;
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f);
        RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3(0.0f, 0.0f, 1.0f);
        float distance = 0.0f;
        float radius = 0.0f;
        bool ignoreSameTeam = true;
        std::uint32_t layerMask = 0xFFFFFFFFu;
    };

    struct HostileSphereOverlapQuery {
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Scene::GameObject* instigator = nullptr;
        RTBEngine::Math::Vector3 center = RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f);
        float radius = 0.0f;
        bool ignoreSameTeam = true;
        std::uint32_t layerMask = 0xFFFFFFFFu;
    };

    std::vector<HostileOverlapHit> OverlapHostileTargets(const HostileOverlapQuery& query);
    std::vector<HostileOverlapHit> OverlapHostileTargetsInSphere(const HostileSphereOverlapQuery& query);

}
