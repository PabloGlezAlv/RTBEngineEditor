#include "CharacterCombatOrigins.h"

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

    RTBEngine::Math::Vector3 GetCapsuleCenterWorld(RTBEngine::ECS::GameObject* object)
    {
        if (!object) {
            return RTBEngine::Math::Vector3::Zero();
        }

        if (auto* capsule = object->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>()) {
            return object->GetWorldPosition() +
                object->GetWorldRotation() * capsule->GetCenterOffset();
        }

        if (auto* sphere = object->GetComponent<RTBEngine::ECS::SphereColliderComponent>()) {
            return object->GetWorldPosition() +
                object->GetWorldRotation() * sphere->GetCenterOffset();
        }

        return object->GetWorldPosition();
    }

    RTBEngine::Math::Vector3 GetFeetWorld(RTBEngine::ECS::GameObject* object)
    {
        if (!object) {
            return RTBEngine::Math::Vector3::Zero();
        }

        if (auto* capsule = object->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>()) {
            const RTBEngine::Math::Quaternion rotation = object->GetWorldRotation();
            const RTBEngine::Math::Vector3 centerWorld =
                object->GetWorldPosition() + rotation * capsule->GetCenterOffset();
            const RTBEngine::Math::Vector3 down = rotation * RTBEngine::Math::Vector3(0.0f, -1.0f, 0.0f);
            return centerWorld + down * (capsule->GetHeight() * 0.5f);
        }

        if (auto* sphere = object->GetComponent<RTBEngine::ECS::SphereColliderComponent>()) {
            const RTBEngine::Math::Quaternion rotation = object->GetWorldRotation();
            const RTBEngine::Math::Vector3 centerWorld =
                object->GetWorldPosition() + rotation * sphere->GetCenterOffset();
            const RTBEngine::Math::Vector3 down = rotation * RTBEngine::Math::Vector3(0.0f, -1.0f, 0.0f);
            return centerWorld + down * (sphere->GetRadius());
        }

        return object->GetWorldPosition();
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
