#pragma once

#include "HealthComponent.h"

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

class EnemyTargetTracker : public RTBEngine::ECS::Component
{
public:
    EnemyTargetTracker() = default;
    ~EnemyTargetTracker() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    RTBEngine::ECS::GameObject* targetObject = nullptr;

    RTB_COMPONENT(EnemyTargetTracker)

public:
    bool HasValidTarget(const RTBEngine::ECS::GameObject* requester) const;
    bool IsTargetAlive(const RTBEngine::ECS::GameObject* requester) const;
    HealthComponent* ResolveTargetHealth() const;
    float GetPlanarDistanceTo(const RTBEngine::ECS::GameObject* requester) const;
    RTBEngine::Math::Vector3 GetPlanarDirectionTo(const RTBEngine::ECS::GameObject* requester) const;
    bool IsWithinTargetHierarchy(const RTBEngine::ECS::GameObject* requester,
                                 RTBEngine::ECS::GameObject* candidate) const;
    void SetTarget(RTBEngine::ECS::GameObject* target);

private:
    std::string targetObjectUuid;
    RTBEngine::ECS::GameObject* lastCapturedTarget = nullptr;

    void CaptureTargetIdentity();
    void ResolveTarget();
};
