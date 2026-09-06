#include "PlayerPawnMotor.h"

#include <RTBEngine/Physics/PhysicsUtils.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerPawnMotor;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasMovementInput(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }
}

RTB_REGISTER_COMPONENT(PlayerPawnMotor)
    RTB_PROPERTY_SERIALIZED_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(sprintMultiplier, 1.0f, 4.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(turnSpeed, 0.0f, 1440.0f)
RTB_END_REGISTER(PlayerPawnMotor)

void PlayerPawnMotor::OnStart()
{
    CacheReferences();
}

void PlayerPawnMotor::OnValidate()
{
    ClampSettings();
}

void PlayerPawnMotor::ClampSettings()
{
    moveSpeed = std::max(0.0f, moveSpeed);
    sprintMultiplier = std::max(1.0f, sprintMultiplier);
    turnSpeed = std::max(0.0f, turnSpeed);
}

void PlayerPawnMotor::CacheReferences()
{
    if (!owner) {
        return;
    }

    rigidBodyComponent = owner->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    colliderBody = CharacterCombatOrigins::ResolveColliderBody(owner);
}

void PlayerPawnMotor::SetMoveStats(float speed, float sprint, float turn)
{
    moveSpeed = speed;
    sprintMultiplier = sprint;
    turnSpeed = turn;
    ClampSettings();
}

float PlayerPawnMotor::ComputePlanarSpeed(bool isRunning) const
{
    return moveSpeed * (isRunning ? sprintMultiplier : 1.0f);
}

bool PlayerPawnMotor::UsesDynamicRigidBody() const
{
    RTBEngine::Physics::RigidBody* rigidBody = GetDynamicRigidBody();
    return rigidBody != nullptr;
}

RTBEngine::Physics::RigidBody* PlayerPawnMotor::GetDynamicRigidBody() const
{
    if (!rigidBodyComponent || !rigidBodyComponent->HasRigidBody()) {
        return nullptr;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rigidBodyComponent->GetRigidBody();
    if (!rigidBody || rigidBody->GetType() != RTBEngine::Physics::RigidBodyType::Dynamic) {
        return nullptr;
    }

    return rigidBody;
}

void PlayerPawnMotor::ApplyDynamicPlanarMotion(
    const RTBEngine::Math::Vector3& moveDirection,
    const RTBEngine::Math::Vector3& facingDirection,
    float planarMoveSpeed,
    float deltaTime,
    float turnSpeedDegrees)
{
    RTBEngine::Physics::RigidBody* rigidBody = GetDynamicRigidBody();
    if (!rigidBody || !owner) {
        return;
    }

    const float resolvedTurnSpeed = turnSpeedDegrees >= 0.0f ? turnSpeedDegrees : turnSpeed;

    RTBEngine::Physics::PhysicsUtils::ApplyPlanarDynamicBodyMotion(
        rigidBody,
        moveDirection,
        facingDirection,
        planarMoveSpeed,
        resolvedTurnSpeed,
        deltaTime,
        owner->GetTransform().GetRotation());
    ApplyExternalKnockbackVelocity(rigidBody, deltaTime);
}

void PlayerPawnMotor::ApplyExternalKnockbackVelocity(
    RTBEngine::Physics::RigidBody* rigidBody,
    float deltaTime)
{
    if (!rigidBody || externalPlanarVelocity.LengthSquared() <= kDirectionEpsilon) {
        externalPlanarVelocity = RTBEngine::Math::Vector3::Zero();
        return;
    }

    btVector3 velocity = rigidBody->GetLinearVelocity();
    velocity.setX(velocity.x() + externalPlanarVelocity.x);
    velocity.setZ(velocity.z() + externalPlanarVelocity.z);
    rigidBody->SetLinearVelocity(velocity);

    const float speed = externalPlanarVelocity.Length();
    const float decayAmount = std::max(0.0f, externalPlanarDecay * std::max(0.0f, deltaTime));
    if (speed <= decayAmount) {
        externalPlanarVelocity = RTBEngine::Math::Vector3::Zero();
        return;
    }

    externalPlanarVelocity *= (speed - decayAmount) / speed;
}

void PlayerPawnMotor::SyncDynamicBodyRotation(const RTBEngine::Math::Quaternion& rotation)
{
    RTBEngine::Physics::RigidBody* rigidBody = GetDynamicRigidBody();
    if (!rigidBody || !owner) {
        return;
    }

    rigidBody->SetWorldTransform(
        owner->GetWorldPosition() + (rotation * colliderBody.centerOffset),
        rotation);
    rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

void PlayerPawnMotor::StopPlanarMotion() const
{
    if (!rigidBodyComponent || !rigidBodyComponent->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rigidBodyComponent->GetRigidBody();
    if (!rigidBody) {
        return;
    }

    btVector3 velocity = rigidBody->GetLinearVelocity();
    velocity.setX(0.0f);
    velocity.setZ(0.0f);
    rigidBody->SetLinearVelocity(velocity);
    rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

void PlayerPawnMotor::AddPlanarKnockback(const RTBEngine::Math::Vector3& direction, float strength)
{
    if (strength <= 0.0f) {
        return;
    }

    RTBEngine::Math::Vector3 planarDirection = direction;
    planarDirection.y = 0.0f;
    if (!HasMovementInput(planarDirection)) {
        return;
    }

    planarDirection.Normalize();
    externalPlanarVelocity += planarDirection * strength;
}
