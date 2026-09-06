#pragma once

#include "CharacterCombatOrigins.h"

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Physics {
        class RigidBody;
    }

    namespace Scene {
        class RigidBodyComponent;
    }
}

class PlayerPawnMotor : public RTBEngine::Scene::Component
{
public:
    PlayerPawnMotor() = default;
    ~PlayerPawnMotor() override = default;

    void OnStart() override;
    void OnValidate() override;

    float GetMoveSpeed() const { return moveSpeed; }
    float GetSprintMultiplier() const { return sprintMultiplier; }
    float GetTurnSpeed() const { return turnSpeed; }
    void SetMoveStats(float speed, float sprint, float turn);

    float ComputePlanarSpeed(bool isRunning) const;
    bool UsesDynamicRigidBody() const;
    RTBEngine::Physics::RigidBody* GetDynamicRigidBody() const;

    void ApplyDynamicPlanarMotion(
        const RTBEngine::Math::Vector3& moveDirection,
        const RTBEngine::Math::Vector3& facingDirection,
        float planarMoveSpeed,
        float deltaTime,
        float turnSpeedDegrees = -1.0f);

    void StopPlanarMotion() const;
    void SyncDynamicBodyRotation(const RTBEngine::Math::Quaternion& rotation);
    void AddPlanarKnockback(const RTBEngine::Math::Vector3& direction, float strength);

    RTB_COMPONENT(PlayerPawnMotor)

    RTB_SERIALIZE()
    float moveSpeed = 4.0f;
    RTB_SERIALIZE()
    float sprintMultiplier = 1.75f;
    RTB_SERIALIZE()
    float turnSpeed = 720.0f;

private:
    void ClampSettings();
    void CacheReferences();
    void ApplyExternalKnockbackVelocity(RTBEngine::Physics::RigidBody* rigidBody, float deltaTime);

    RTBEngine::Scene::RigidBodyComponent* rigidBodyComponent = nullptr;
    CharacterCombatOrigins::ColliderBody colliderBody;
    RTBEngine::Math::Vector3 externalPlanarVelocity = RTBEngine::Math::Vector3::Zero();
    float externalPlanarDecay = 10.0f;
};
