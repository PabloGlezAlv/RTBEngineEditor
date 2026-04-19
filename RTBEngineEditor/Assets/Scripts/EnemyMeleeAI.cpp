#include "EnemyMeleeAI.h"

#include "HealthComponent.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/CapsuleColliderComponent.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/SphereColliderComponent.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsUtils.h>
#include <RTBEngine/Rendering/ModelLoader.h>

#include <algorithm>
#include <cmath>

using ThisClass = EnemyMeleeAI;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;
    constexpr float kDegToRad = kPi / 180.0f;
    constexpr float kDirectionEpsilon = 0.0001f;
    constexpr const char* kWalkAlias = "EnemyMelee.Walk";
    constexpr const char* kAttackAlias = "EnemyMelee.Attack";

    float ClampAngleDegrees(float angle)
    {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }

    float MoveTowardsAngleDegrees(float current, float target, float maxDelta)
    {
        const float delta = ClampAngleDegrees(target - current);
        if (std::abs(delta) <= maxDelta) {
            return target;
        }

        return current + (delta > 0.0f ? maxDelta : -maxDelta);
    }

    RTBEngine::Math::Vector3 GetPlanarForwardFromRotation(const RTBEngine::Math::Quaternion& rotation)
    {
        RTBEngine::Math::Vector3 forward = rotation * RTBEngine::Math::Vector3::Forward();
        forward.y = 0.0f;

        if (forward.LengthSquared() <= kDirectionEpsilon) {
            return RTBEngine::Math::Vector3::Forward();
        }

        forward.Normalize();
        return forward;
    }

    RTBEngine::Math::Vector3 GetRigidBodyPlanarForward(
        const RTBEngine::Physics::RigidBody* rigidBody,
        const RTBEngine::Math::Quaternion& fallbackRotation)
    {
        if (!rigidBody || !rigidBody->GetBulletRigidBody()) {
            return GetPlanarForwardFromRotation(fallbackRotation);
        }

        return GetPlanarForwardFromRotation(
            RTBEngine::Physics::PhysicsUtils::FromBullet(
                rigidBody->GetBulletRigidBody()->getWorldTransform().getRotation()));
    }

    void ReleaseLoadedModelMeshes(RTBEngine::Rendering::ModelData& data)
    {
        for (RTBEngine::Rendering::Mesh* mesh : data.meshes) {
            delete mesh;
        }
        data.meshes.clear();
    }
}

RTB_REGISTER_COMPONENT(EnemyMeleeAI)
    RTB_PROPERTY_GAMEOBJECT(targetObject)
    RTB_PROPERTY(targetName)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 0.0f, 1440.0f)
    RTB_PROPERTY_RANGE(attackRange, 0.1f, 5.0f)
    RTB_PROPERTY_RANGE(attackCooldown, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(attackDamage, 0.0f, 100.0f)
    RTB_PROPERTY_RANGE(attackSphereRadius, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(attackSphereDistance, 0.05f, 5.0f)
    RTB_PROPERTY(attackHandBoneName)
    RTB_PROPERTY_FBX(walkAnimationFbx)
    RTB_PROPERTY_FBX(attackAnimationFbx)
RTB_END_REGISTER(EnemyMeleeAI)

void EnemyMeleeAI::OnStart()
{
    ClampSettings();
    ResolveTarget();
    ResolveAnimator();
    RegisterAnimationSlots();
    ConfigurePhysicsBody();

    if (HasValidTarget() && IsTargetAlive()) {
        state = State::Chasing;
        PlayWalkLoop();
    } else {
        state = State::Idle;
    }
}

void EnemyMeleeAI::OnUpdate(float deltaTime)
{
    if (!owner) {
        return;
    }

    ClampSettings();
    ResolveTarget();
    ResolveAnimator();
    RegisterAnimationSlots();
    ConfigurePhysicsBody();
    UpdateState(deltaTime);
}

void EnemyMeleeAI::OnFixedUpdate(float fixedDeltaTime)
{
    if (!owner) {
        return;
    }

    ClampSettings();
    ResolveTarget();
    ConfigurePhysicsBody();
    UpdateMovement(fixedDeltaTime);
}

void EnemyMeleeAI::OnLateUpdate(float /*deltaTime*/)
{
    if (state != State::Attacking) {
        return;
    }

    if (!animator) {
        FinishAttack();
        return;
    }

    if (animator->GetCurrentClipName() == kAttackAlias && animator->IsPlaying()) {
        return;
    }

    FinishAttack();
}

void EnemyMeleeAI::OnValidate()
{
    ClampSettings();
    ResolveTarget();
    ResolveAnimator();
    RegisterAnimationSlots();
    ConfigurePhysicsBody();
}

void EnemyMeleeAI::ClampSettings()
{
    moveSpeed = std::max(0.0f, moveSpeed);
    turnSpeed = std::max(0.0f, turnSpeed);
    attackRange = std::max(0.1f, attackRange);
    attackCooldown = std::max(0.0f, attackCooldown);
    attackDamage = std::max(0.0f, attackDamage);
    attackSphereRadius = std::max(0.05f, attackSphereRadius);
    attackSphereDistance = std::max(0.05f, attackSphereDistance);
}

void EnemyMeleeAI::ResolveTarget()
{
    if (targetObject && targetObject != owner) {
        return;
    }

    targetObject = nullptr;

    if (targetName.empty()) {
        return;
    }

    auto& sceneManager = RTBEngine::ECS::SceneManager::GetInstance();
    auto* scene = sceneManager.GetActiveScene();
    if (!scene) {
        return;
    }

    RTBEngine::ECS::GameObject* candidate = scene->FindGameObject(targetName);
    if (candidate != owner) {
        targetObject = candidate;
    }
}

void EnemyMeleeAI::ResolveAnimator()
{
    if (animator) {
        return;
    }

    if (!owner) {
        return;
    }

    animator = owner->GetComponentInChildren<RTBEngine::Animation::Animator>();
}

void EnemyMeleeAI::ConfigurePhysicsBody() const
{
    if (!owner) {
        return;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (!rbComp || !rbComp->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rbComp->GetRigidBody();
    if (!rigidBody || rigidBody->GetType() != RTBEngine::Physics::RigidBodyType::Dynamic) {
        return;
    }

    rigidBody->SetAngularFactor(btVector3(0.0f, 1.0f, 0.0f));

    btVector3 angularVelocity = rigidBody->GetAngularVelocity();
    angularVelocity.setX(0.0f);
    angularVelocity.setZ(0.0f);
    rigidBody->SetAngularVelocity(angularVelocity);
}

void EnemyMeleeAI::RegisterAnimationSlots()
{
    const bool animatorChanged = (registeredAnimator != animator);
    if (animatorChanged) {
        registeredAnimator = animator;
        walkSlotState = {};
        attackSlotState = {};
    }

    if (!animator) {
        if (!missingAnimatorWarningShown && (!walkAnimationFbx.empty() || !attackAnimationFbx.empty())) {
            RTB_WARN("[EnemyMeleeAI] Assign an Animator component to use FBX animation slots.");
            missingAnimatorWarningShown = true;
        }
        return;
    }

    missingAnimatorWarningShown = false;

    RegisterAnimationSlot("Walk", walkAnimationFbx, kWalkAlias, walkSlotState);
    RegisterAnimationSlot("Attack", attackAnimationFbx, kAttackAlias, attackSlotState);
}

void EnemyMeleeAI::RegisterAnimationSlot(const char* slotLabel,
                                         const std::string& sourceFbx,
                                         const char* alias,
                                         AnimationSlotState& slotState)
{
    if (slotState.sourceFbx == sourceFbx) {
        return;
    }

    slotState.sourceFbx = sourceFbx;
    slotState.ready = false;

    if (!animator || sourceFbx.empty()) {
        return;
    }

    RTBEngine::Rendering::ModelData modelData =
        RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(sourceFbx);

    if (modelData.animations.empty() || !modelData.animations.front()) {
        RTB_WARN(std::string("[EnemyMeleeAI] ") + slotLabel +
                 " slot FBX has no usable animation clip: " + sourceFbx);
        ReleaseLoadedModelMeshes(modelData);
        return;
    }

    animator->AddClip(alias, modelData.animations.front());
    slotState.ready = true;
    ReleaseLoadedModelMeshes(modelData);
}

void EnemyMeleeAI::UpdateState(float deltaTime)
{
    if (!HasValidTarget() || !IsTargetAlive()) {
        state = State::Idle;
        cooldownRemaining = 0.0f;
        return;
    }

    if (state == State::Attacking) {
        return;
    }

    if (state == State::Cooldown) {
        cooldownRemaining = std::max(0.0f, cooldownRemaining - deltaTime);
        if (cooldownRemaining > 0.0f) {
            return;
        }
    }

    if (GetPlanarDistanceToTarget() <= attackRange) {
        StartAttack();
        return;
    }

    state = State::Chasing;
    PlayWalkLoop();
}

void EnemyMeleeAI::UpdateMovement(float deltaTime)
{
    auto* rbComp = owner ? owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>() : nullptr;
    RTBEngine::Physics::RigidBody* rigidBody =
        (rbComp && rbComp->HasRigidBody()) ? rbComp->GetRigidBody() : nullptr;
    const bool useDynamicRigidBody =
        rigidBody && rigidBody->GetType() == RTBEngine::Physics::RigidBodyType::Dynamic;

    const RTBEngine::Math::Vector3 targetDirection = GetPlanarDirectionToTarget();
    const bool canFaceTarget = targetDirection.LengthSquared() > kDirectionEpsilon;
    const bool shouldMove = state == State::Chasing && canFaceTarget;
    const RTBEngine::Math::Vector3 desiredMove = shouldMove ? targetDirection : RTBEngine::Math::Vector3::Zero();

    if (useDynamicRigidBody) {
        btVector3 velocity = rigidBody->GetLinearVelocity();
        velocity.setX(desiredMove.x * moveSpeed);
        velocity.setZ(desiredMove.z * moveSpeed);
        rigidBody->SetLinearVelocity(velocity);

        btVector3 angularVelocity = rigidBody->GetAngularVelocity();
        angularVelocity.setX(0.0f);
        angularVelocity.setZ(0.0f);

        if (!canFaceTarget || deltaTime <= 0.0001f || turnSpeed <= 0.0f) {
            angularVelocity.setY(0.0f);
            rigidBody->SetAngularVelocity(angularVelocity);
            return;
        }

        const RTBEngine::Math::Vector3 currentForward =
            GetRigidBodyPlanarForward(rigidBody, owner->GetTransform().GetRotation());
        const float signedAngleRadians = std::atan2(
            currentForward.Cross(targetDirection).y,
            std::clamp(currentForward.Dot(targetDirection), -1.0f, 1.0f));
        const float signedAngleDegrees = signedAngleRadians * kRadToDeg;

        if (std::abs(signedAngleDegrees) <= 0.1f) {
            angularVelocity.setY(0.0f);
        } else {
            const float yawSpeedDegrees =
                std::clamp(signedAngleDegrees / deltaTime, -turnSpeed, turnSpeed);
            angularVelocity.setY(yawSpeedDegrees * kDegToRad);
        }

        rigidBody->SetAngularVelocity(angularVelocity);
        return;
    }

    if (!canFaceTarget) {
        return;
    }

    const float targetYaw = -std::atan2(targetDirection.x, targetDirection.z) * kRadToDeg;
    RTBEngine::Math::Vector3 currentEuler = owner->GetTransform().GetRotation().ToEulerAngles();
    const float currentYaw = currentEuler.y * kRadToDeg;
    const float nextYaw = MoveTowardsAngleDegrees(currentYaw, targetYaw, turnSpeed * deltaTime);
    const RTBEngine::Math::Quaternion nextRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, nextYaw * kDegToRad, 0.0f);

    if (shouldMove) {
        owner->GetTransform().SetPosition(
            owner->GetTransform().GetPosition() + desiredMove * moveSpeed * deltaTime);
    }

    owner->GetTransform().SetRotation(nextRotation);
}

void EnemyMeleeAI::StartAttack()
{
    if (!HasValidTarget() || !IsTargetAlive()) {
        state = State::Idle;
        return;
    }

    state = State::Attacking;

    if (animator && attackSlotState.ready && animator->GetClip(kAttackAlias)) {
        animator->Play(kAttackAlias, false);
        return;
    }

    RTB_WARN("[EnemyMeleeAI] Attack clip is missing; applying the melee hit without animation.");
    FinishAttack();
}

void EnemyMeleeAI::FinishAttack()
{
    RTBEngine::Math::Vector3 hitPoint;
    if (PerformAttackSphereCast(&hitPoint)) {
        if (HealthComponent* health = ResolveTargetHealth()) {
            health->TakeDamage(attackDamage);
        }
    }

    state = State::Cooldown;
    cooldownRemaining = attackCooldown;
}

bool EnemyMeleeAI::HasValidTarget() const
{
    return targetObject != nullptr && targetObject != owner;
}

bool EnemyMeleeAI::IsTargetAlive() const
{
    if (!HasValidTarget()) {
        return false;
    }

    if (HealthComponent* health = ResolveTargetHealth()) {
        return !health->IsDead();
    }

    return true;
}

HealthComponent* EnemyMeleeAI::ResolveTargetHealth() const
{
    if (!targetObject) {
        return nullptr;
    }

    if (HealthComponent* health = targetObject->GetComponent<HealthComponent>()) {
        return health;
    }

    return targetObject->GetComponentInChildren<HealthComponent>();
}

float EnemyMeleeAI::GetPlanarDistanceToTarget() const
{
    if (!owner || !HasValidTarget()) {
        return 0.0f;
    }

    RTBEngine::Math::Vector3 direction = targetObject->GetWorldPosition() - owner->GetWorldPosition();
    direction.y = 0.0f;
    return direction.Length();
}

RTBEngine::Math::Vector3 EnemyMeleeAI::GetPlanarDirectionToTarget() const
{
    if (!owner || !HasValidTarget()) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 direction = targetObject->GetWorldPosition() - owner->GetWorldPosition();
    direction.y = 0.0f;

    if (direction.LengthSquared() <= kDirectionEpsilon) {
        return RTBEngine::Math::Vector3::Zero();
    }

    direction.Normalize();
    return direction;
}

RTBEngine::Math::Vector3 EnemyMeleeAI::GetAttackHandWorldPosition() const
{
    if (animator) {
        if (RTBEngine::ECS::GameObject* bone = animator->GetBoneGameObject(attackHandBoneName)) {
            return bone->GetWorldPosition();
        }
    }

    return GetFallbackHandWorldPosition();
}

RTBEngine::Math::Vector3 EnemyMeleeAI::GetFallbackHandWorldPosition() const
{
    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 forward = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
    if (forward.LengthSquared() <= kDirectionEpsilon) {
        forward = RTBEngine::Math::Vector3::Forward();
    } else {
        forward.Normalize();
    }

    return owner->GetWorldPosition() + RTBEngine::Math::Vector3(0.0f, 1.1f, 0.0f) + forward * 0.35f;
}

void EnemyMeleeAI::PlayWalkLoop()
{
    if (!animator || !walkSlotState.ready) {
        return;
    }

    if (!animator->GetClip(kWalkAlias)) {
        return;
    }

    if (animator->GetCurrentClipName() == kWalkAlias && animator->IsPlaying()) {
        return;
    }

    animator->Play(kWalkAlias, true);
}

bool EnemyMeleeAI::PerformAttackSphereCast(RTBEngine::Math::Vector3* outHitPoint) const
{
    if (!owner || !HasValidTarget()) {
        return false;
    }

    RTBEngine::Math::Vector3 castStart = GetAttackHandWorldPosition();
    RTBEngine::Math::Vector3 castDirection = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
    if (castDirection.LengthSquared() <= kDirectionEpsilon) {
        castDirection = GetPlanarDirectionToTarget();
    }

    if (castDirection.LengthSquared() <= kDirectionEpsilon) {
        castDirection = RTBEngine::Math::Vector3::Forward();
    } else {
        castDirection.Normalize();
    }

    const RTBEngine::Math::Vector3 castEnd = castStart + castDirection * attackSphereDistance;
    return SphereCastIntersectsGameObject(targetObject, castStart, castEnd, attackSphereRadius, outHitPoint);
}

bool EnemyMeleeAI::SphereCastIntersectsGameObject(RTBEngine::ECS::GameObject* candidate,
                                                  const RTBEngine::Math::Vector3& castStart,
                                                  const RTBEngine::Math::Vector3& castEnd,
                                                  float castRadius,
                                                  RTBEngine::Math::Vector3* outHitPoint) const
{
    if (!candidate) {
        return false;
    }

    if (auto* capsule = candidate->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>()) {
        const RTBEngine::Math::Quaternion candidateRotation = candidate->GetWorldRotation();
        RTBEngine::Math::Vector3 capsuleUp = candidateRotation * RTBEngine::Math::Vector3::Up();
        if (capsuleUp.LengthSquared() <= kDirectionEpsilon) {
            capsuleUp = RTBEngine::Math::Vector3::Up();
        } else {
            capsuleUp.Normalize();
        }

        const RTBEngine::Math::Vector3 capsuleCenter =
            candidate->GetWorldPosition() + (candidateRotation * capsule->GetCenterOffset());
        const float halfCylinderHeight = std::max(0.0f, (capsule->GetHeight() - (capsule->GetRadius() * 2.0f)) * 0.5f);
        const RTBEngine::Math::Vector3 capsuleAxisStart = capsuleCenter + capsuleUp * halfCylinderHeight;
        const RTBEngine::Math::Vector3 capsuleAxisEnd = capsuleCenter - capsuleUp * halfCylinderHeight;
        const float combinedRadius = castRadius + capsule->GetRadius();

        if (DistanceSquaredSegmentToSegment(castStart, castEnd, capsuleAxisStart, capsuleAxisEnd) <=
            (combinedRadius * combinedRadius)) {
            if (outHitPoint) {
                *outHitPoint = capsuleCenter;
            }
            return true;
        }
    }

    if (auto* sphere = candidate->GetComponent<RTBEngine::ECS::SphereColliderComponent>()) {
        const RTBEngine::Math::Vector3 sphereCenter =
            candidate->GetWorldPosition() + (candidate->GetWorldRotation() * sphere->GetCenterOffset());
        const float combinedRadius = castRadius + sphere->GetRadius();
        if (DistanceSquaredPointToSegment(sphereCenter, castStart, castEnd) <=
            (combinedRadius * combinedRadius)) {
            if (outHitPoint) {
                *outHitPoint = sphereCenter;
            }
            return true;
        }
    }

    const RTBEngine::Math::Vector3 fallbackTargetPoint =
        candidate->GetWorldPosition() + RTBEngine::Math::Vector3(0.0f, 0.9f, 0.0f);
    if (DistanceSquaredPointToSegment(fallbackTargetPoint, castStart, castEnd) <= (castRadius * castRadius)) {
        if (outHitPoint) {
            *outHitPoint = fallbackTargetPoint;
        }
        return true;
    }

    return false;
}

float EnemyMeleeAI::DistanceSquaredPointToSegment(const RTBEngine::Math::Vector3& point,
                                                  const RTBEngine::Math::Vector3& segmentStart,
                                                  const RTBEngine::Math::Vector3& segmentEnd)
{
    const RTBEngine::Math::Vector3 segment = segmentEnd - segmentStart;
    const float segmentLengthSq = segment.LengthSquared();
    if (segmentLengthSq <= kDirectionEpsilon) {
        return (point - segmentStart).LengthSquared();
    }

    const float t = std::clamp((point - segmentStart).Dot(segment) / segmentLengthSq, 0.0f, 1.0f);
    const RTBEngine::Math::Vector3 closestPoint = segmentStart + segment * t;
    return (point - closestPoint).LengthSquared();
}

float EnemyMeleeAI::DistanceSquaredSegmentToSegment(const RTBEngine::Math::Vector3& segmentAStart,
                                                    const RTBEngine::Math::Vector3& segmentAEnd,
                                                    const RTBEngine::Math::Vector3& segmentBStart,
                                                    const RTBEngine::Math::Vector3& segmentBEnd)
{
    const RTBEngine::Math::Vector3 segmentA = segmentAEnd - segmentAStart;
    const RTBEngine::Math::Vector3 segmentB = segmentBEnd - segmentBStart;
    const RTBEngine::Math::Vector3 betweenStarts = segmentAStart - segmentBStart;

    const float a = segmentA.Dot(segmentA);
    const float e = segmentB.Dot(segmentB);
    const float f = segmentB.Dot(betweenStarts);

    float s = 0.0f;
    float t = 0.0f;

    if (a <= kDirectionEpsilon && e <= kDirectionEpsilon) {
        return betweenStarts.LengthSquared();
    }

    if (a <= kDirectionEpsilon) {
        t = std::clamp(f / e, 0.0f, 1.0f);
    } else {
        const float c = segmentA.Dot(betweenStarts);

        if (e <= kDirectionEpsilon) {
            s = std::clamp(-c / a, 0.0f, 1.0f);
        } else {
            const float b = segmentA.Dot(segmentB);
            const float denom = a * e - b * b;

            if (std::abs(denom) > kDirectionEpsilon) {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }

            t = (b * s + f) / e;

            if (t < 0.0f) {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    const RTBEngine::Math::Vector3 closestPointA = segmentAStart + segmentA * s;
    const RTBEngine::Math::Vector3 closestPointB = segmentBStart + segmentB * t;
    return (closestPointA - closestPointB).LengthSquared();
}
