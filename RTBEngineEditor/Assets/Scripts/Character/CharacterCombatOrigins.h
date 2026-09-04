#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

namespace CharacterCombatOrigins {

    struct ColliderBody {
        enum class Shape {
            None = 0,
            Capsule,
            Sphere
        };

        Shape shape = Shape::None;
        RTBEngine::Math::Vector3 centerOffset = RTBEngine::Math::Vector3::Zero();
        float verticalExtent = 0.0f;
    };

    ColliderBody ResolveColliderBody(RTBEngine::Scene::GameObject* object);

    RTBEngine::Math::Vector3 GetCapsuleCenterWorld(RTBEngine::Scene::GameObject* object);
    RTBEngine::Math::Vector3 GetCapsuleCenterWorld(
        RTBEngine::Scene::GameObject* object,
        const ColliderBody& body);

    RTBEngine::Math::Vector3 GetFeetWorld(RTBEngine::Scene::GameObject* object);
    RTBEngine::Math::Vector3 GetFeetWorld(
        RTBEngine::Scene::GameObject* object,
        const ColliderBody& body);

    RTBEngine::Math::Vector3 ApplyPlanarDirectionOffset(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& attackDirection,
        float forwardOffset);

}
