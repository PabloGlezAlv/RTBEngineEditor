#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

class EnemyTargetTracker : public RTBEngine::Scene::Component
{
public:
    EnemyTargetTracker() = default;
    ~EnemyTargetTracker() override = default;

    void OnStart() override;
    void OnValidate() override;

    RTBEngine::Scene::GameObject* targetObject = nullptr;

    RTB_COMPONENT(EnemyTargetTracker)

public:
    bool HasValidTarget(const RTBEngine::Scene::GameObject* requester) const;
    bool IsTargetAlive(const RTBEngine::Scene::GameObject* requester) const;
    HealthComponent* ResolveTargetHealth() const;
    float GetPlanarDistanceTo(const RTBEngine::Scene::GameObject* requester) const;
    RTBEngine::Math::Vector3 GetPlanarDirectionTo(const RTBEngine::Scene::GameObject* requester) const;
    bool IsWithinTargetHierarchy(const RTBEngine::Scene::GameObject* requester,
                                 RTBEngine::Scene::GameObject* candidate) const;
    void SetTarget(RTBEngine::Scene::GameObject* target);

private:
    void SanitizeTarget();
};
