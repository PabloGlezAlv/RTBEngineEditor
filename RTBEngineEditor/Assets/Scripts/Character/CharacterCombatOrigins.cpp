#include "CharacterCombatOrigins.h"

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/CapsuleColliderComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SphereColliderComponent.h>

#include <cmath>

namespace CharacterCombatOrigins {
    namespace {
        constexpr float kDirectionEpsilon = 0.0001f;

        bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
        {
            return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
        }
    }

    ColliderBody ResolveColliderBody(RTBEngine::Scene::GameObject* object)
    {
        ColliderBody body;
        if (!object) {
            return body;
        }

        if (auto* capsule = object->GetComponent<RTBEngine::Scene::CapsuleColliderComponent>()) {
            body.shape = ColliderBody::Shape::Capsule;
            body.centerOffset = capsule->GetCenterOffset();
            body.verticalExtent = capsule->GetHeight() * 0.5f;
            return body;
        }

        if (auto* sphere = object->GetComponent<RTBEngine::Scene::SphereColliderComponent>()) {
            body.shape = ColliderBody::Shape::Sphere;
            body.centerOffset = sphere->GetCenterOffset();
            body.verticalExtent = sphere->GetRadius();
            return body;
        }

        return body;
    }

    RTBEngine::Math::Vector3 GetCapsuleCenterWorld(
        RTBEngine::Scene::GameObject* object,
        const ColliderBody& body)
    {
        if (!object) {
            return RTBEngine::Math::Vector3::Zero();
        }

        if (body.shape == ColliderBody::Shape::None) {
            return object->GetWorldPosition();
        }

        return object->GetWorldPosition() +
            object->GetWorldRotation() * body.centerOffset;
    }

    RTBEngine::Math::Vector3 GetCapsuleCenterWorld(RTBEngine::Scene::GameObject* object)
    {
        return GetCapsuleCenterWorld(object, ResolveColliderBody(object));
    }

    RTBEngine::Math::Vector3 GetFeetWorld(
        RTBEngine::Scene::GameObject* object,
        const ColliderBody& body)
    {
        if (!object) {
            return RTBEngine::Math::Vector3::Zero();
        }

        if (body.shape == ColliderBody::Shape::None) {
            return object->GetWorldPosition();
        }

        const RTBEngine::Math::Quaternion rotation = object->GetWorldRotation();
        const RTBEngine::Math::Vector3 centerWorld =
            object->GetWorldPosition() + rotation * body.centerOffset;
        const RTBEngine::Math::Vector3 down = rotation * RTBEngine::Math::Vector3(0.0f, -1.0f, 0.0f);
        return centerWorld + down * body.verticalExtent;
    }

    RTBEngine::Math::Vector3 GetFeetWorld(RTBEngine::Scene::GameObject* object)
    {
        return GetFeetWorld(object, ResolveColliderBody(object));
    }

    RTBEngine::Math::Vector3 ApplyPlanarDirectionOffset(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& attackDirection,
        float forwardOffset)
    {
        if (forwardOffset <= 0.0f) {
            return origin;
        }

        RTBEngine::Math::Vector3 planarDirection = attackDirection;
        planarDirection.y = 0.0f;
        if (!HasPlanarDirection(planarDirection)) {
            return origin;
        }

        planarDirection.Normalize();
        return origin + planarDirection * forwardOffset;
    }

}
