#include "EnemyLocomotionController.h"

#include "EnemyMeleeAIShared.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsUtils.h>

#include <algorithm>
#include <cmath>

using ThisClass = EnemyLocomotionController;

RTB_REGISTER_COMPONENT(EnemyLocomotionController)
    RTB_PROPERTY_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 0.0f, 1440.0f)
    RTB_PROPERTY_RANGE(knockbackImpulse, 0.0f, 20.0f)
RTB_END_REGISTER(EnemyLocomotionController)

void EnemyLocomotionController::OnStart()
{
    ClampSettings();
}

void EnemyLocomotionController::OnValidate()
{
    ClampSettings();
}

void EnemyLocomotionController::MoveTowards(const RTBEngine::Math::Vector3& targetDirection, float deltaTime)
{
    if (!owner) {
        return;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    RTBEngine::Physics::RigidBody* rigidBody =
        (rbComp && rbComp->HasRigidBody()) ? rbComp->GetRigidBody() : nullptr;
    const bool useDynamicRigidBody =
        rigidBody &&
        rigidBody->GetType() == RTBEngine::Physics::RigidBodyType::Dynamic &&
        rigidBody->GetBulletRigidBody() != nullptr;

    const bool canFaceTarget = EnemyMeleeAIDetail::HasPlanarDirection(targetDirection);
    const RTBEngine::Math::Vector3 desiredMove =
        canFaceTarget ? targetDirection : RTBEngine::Math::Vector3::Zero();

    if (useDynamicRigidBody) {
        RTBEngine::Physics::PhysicsUtils::ApplyPlanarDynamicBodyMotion(
            rigidBody,
            desiredMove,
            canFaceTarget ? targetDirection : RTBEngine::Math::Vector3::Zero(),
            moveSpeed,
            turnSpeed,
            deltaTime,
            owner->GetTransform().GetRotation());
        return;
    }

    if (!canFaceTarget) {
        return;
    }

    const float targetYaw = -std::atan2(targetDirection.x, targetDirection.z) * EnemyMeleeAIDetail::kRadToDeg;
    const float currentYaw =
        owner->GetTransform().GetRotation().ToEulerAngles().y * EnemyMeleeAIDetail::kRadToDeg;
    const float nextYaw =
        EnemyMeleeAIDetail::MoveTowardsAngleDegrees(currentYaw, targetYaw, turnSpeed * deltaTime);

    owner->GetTransform().SetPosition(
        owner->GetTransform().GetPosition() + desiredMove * moveSpeed * deltaTime);
    owner->GetTransform().SetRotation(
        RTBEngine::Math::Quaternion::FromEulerAngles(
            0.0f,
            nextYaw * EnemyMeleeAIDetail::kDegToRad,
            0.0f));
}

void EnemyLocomotionController::StopPlanarMotion() const
{
    if (!owner) {
        return;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    if (!rbComp || !rbComp->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rbComp->GetRigidBody();
    if (!rigidBody) {
        return;
    }

    btVector3 velocity = rigidBody->GetLinearVelocity();
    velocity.setX(0.0f);
    velocity.setZ(0.0f);
    rigidBody->SetLinearVelocity(velocity);
    rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

void EnemyLocomotionController::ApplyKnockback(const RTBEngine::Math::Vector3& hitDirection,
                                               const RTBEngine::Math::Vector3& fallbackDirection,
                                               float strength)
{
    if (strength <= 0.0f || !owner) {
        return;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    if (!rbComp || !rbComp->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rbComp->GetRigidBody();
    if (!rigidBody || rigidBody->GetType() != RTBEngine::Physics::RigidBodyType::Dynamic) {
        return;
    }

    RTBEngine::Math::Vector3 planarDirection = hitDirection;
    planarDirection.y = 0.0f;
    if (!EnemyMeleeAIDetail::HasPlanarDirection(planarDirection)) {
        planarDirection = fallbackDirection;
        planarDirection.y = 0.0f;
    }

    if (!EnemyMeleeAIDetail::HasPlanarDirection(planarDirection)) {
        return;
    }

    planarDirection.Normalize();
    rigidBody->ApplyCentralImpulse(btVector3(
        planarDirection.x * strength,
        0.0f,
        planarDirection.z * strength));
}

void EnemyLocomotionController::ClampSettings()
{
    moveSpeed = std::max(0.0f, moveSpeed);
    turnSpeed = std::max(0.0f, turnSpeed);
    knockbackImpulse = std::max(0.0f, knockbackImpulse);
}
