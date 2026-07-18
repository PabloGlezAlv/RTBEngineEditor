#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <cstdint>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }

    namespace Physics {
        class PhysicsWorld;
    }

    namespace ECS {

        struct ProjectileFlight {
            Math::Vector3 direction = Math::Vector3::Forward();
            float speed = 8.0f;
            float maxDistance = 1.15f;
            float distanceTravelled = 0.0f;
            float radius = 0.55f;
            bool pendingDestroy = false;
            bool shouldStop = false;
        };

        struct ProjectilePendingHit {
            bool active = false;
            Scene::GameObject* hitObject = nullptr;
            Math::Vector3 hitPoint = Math::Vector3::Zero();
            float hitFraction = 1.0f;
        };

        struct ProjectilePhysicsContext {
            Physics::PhysicsWorld* physicsWorld = nullptr;
            Scene::GameObject* instigator = nullptr;
        };

    }
}
