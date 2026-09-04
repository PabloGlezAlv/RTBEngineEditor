#pragma once

#include "CharacterCombatOrigins.h"

#include <RTBEngine/Math/Vectors/Vector3.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
        class TrailRenderer;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class PlayerAimTrailPresenter
{
public:
    RTBEngine::Scene::TrailRenderer* trail = nullptr;
    float forwardOffset = 0.40f;
    float heightOffset = 0.0f;
    float wallClipRadius = 0.45f;

    void Bind(
        RTBEngine::Scene::TrailRenderer* trailRenderer,
        float trailForwardOffset,
        float trailHeightOffset,
        float clipRadius);
    void CacheOwner(RTBEngine::Scene::GameObject* owner);

    void Hide() const;
    void ShowSegment(
        const RTBEngine::Math::Vector3& start,
        const RTBEngine::Math::Vector3& end) const;

    RTBEngine::Math::Vector3 ResolveVisualOrigin(
        RTBEngine::Scene::GameObject* owner,
        const RTBEngine::Math::Vector3& attackDirection) const;
    RTBEngine::Math::Vector3 ResolveCombatOrigin(
        RTBEngine::Scene::GameObject* owner,
        const RTBEngine::Math::Vector3& attackDirection) const;
    float ResolveClippedLength(
        RTBEngine::Scene::GameObject* owner,
        const RTBEngine::Math::Vector3& attackDirection,
        float maxLength) const;

private:
    CharacterCombatOrigins::ColliderBody colliderBody;
    RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
};
