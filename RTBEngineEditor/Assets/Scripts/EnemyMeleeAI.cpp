#include "EnemyMeleeAI.h"

#include "HealthComponent.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Animation/AnimationClip.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/BoxColliderComponent.h>
#include <RTBEngine/ECS/CapsuleColliderComponent.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/SphereColliderComponent.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Physics/PhysicsUtils.h>

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
    constexpr float kAttackCompletionEpsilon = 0.0001f;

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
}

RTB_REGISTER_COMPONENT(EnemyMeleeAI)
    RTB_PROPERTY_GAMEOBJECT(targetObject)
    RTB_PROPERTY_GAMEOBJECT(attackOriginObject)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 0.0f, 1440.0f)
    RTB_PROPERTY_RANGE(attackRange, 0.1f, 5.0f)
    RTB_PROPERTY_RANGE(attackCooldown, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(attackDamage, 0.0f, 100.0f)
    RTB_PROPERTY_RANGE(attackHitDelay, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(attackSphereRadius, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(attackSphereDistance, 0.05f, 5.0f)
    RTB_PROPERTY_FBX(walkAnimationFbx)
    RTB_PROPERTY_FBX(attackAnimationFbx)
RTB_END_REGISTER(EnemyMeleeAI)

void EnemyMeleeAI::OnStart()
{
    ClampSettings();
    CaptureTargetIdentity();
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
    CaptureTargetIdentity();
    ResolveTarget();
    ResolveAnimator();
    RegisterAnimationSlots();
    ConfigurePhysicsBody();
    UpdateState(deltaTime);
    UpdateAttack(deltaTime);
}

void EnemyMeleeAI::OnFixedUpdate(float fixedDeltaTime)
{
    if (!owner) {
        return;
    }

    ClampSettings();
    CaptureTargetIdentity();
    ResolveTarget();
    ConfigurePhysicsBody();
    UpdateMovement(fixedDeltaTime);
}

void EnemyMeleeAI::OnLateUpdate(float /*deltaTime*/)
{
    if (state != State::Attacking) {
        return;
    }

    const bool hasAttackAnimation =
        animator && attackSlotState.ready && animator->GetClip(kAttackAlias);

    if (!hasAttackAnimation) {
        if (attackHitExecuted || attackElapsed + kAttackCompletionEpsilon >= attackHitDelay) {
            FinishAttack();
        }
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
    CaptureTargetIdentity();
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
    attackHitDelay = std::max(0.0f, attackHitDelay);
    attackSphereRadius = std::max(0.05f, attackSphereRadius);
    attackSphereDistance = std::max(0.05f, attackSphereDistance);
}

void EnemyMeleeAI::CaptureTargetIdentity()
{
    if (!targetObject || targetObject == owner) {
        lastCapturedTarget = nullptr;
        targetObjectUuid.clear();
        return;
    }

    if (targetObject == lastCapturedTarget) {
        return;
    }

    targetObjectUuid = targetObject->GetUUID();
    lastCapturedTarget = targetObject;
}

void EnemyMeleeAI::ResolveTarget()
{
    targetObject = nullptr;

    auto* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        lastCapturedTarget = nullptr;
        return;
    }

    if (!targetObjectUuid.empty()) {
        RTBEngine::ECS::GameObject* candidate = scene->FindGameObjectByUUID(targetObjectUuid);
        if (candidate && candidate != owner) {
            targetObject = candidate;
            lastCapturedTarget = candidate;
            return;
        }
    }

    lastCapturedTarget = nullptr;
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
    RTBEngine::Physics::PhysicsUtils::ConfigurePlanarDynamicBody(rigidBody);
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

    if (!animator->LoadClipFromFbx(alias, sourceFbx)) {
        RTB_WARN(std::string("[EnemyMeleeAI] ") + slotLabel +
                 " slot FBX has no usable animation clip: " + sourceFbx);
        return;
    }

    slotState.ready = true;
}

void EnemyMeleeAI::UpdateState(float deltaTime)
{
    if (!HasValidTarget() || !IsTargetAlive()) {
        state = State::Idle;
        cooldownRemaining = 0.0f;
        attackElapsed = 0.0f;
        attackHitExecuted = false;
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

void EnemyMeleeAI::UpdateAttack(float deltaTime)
{
    if (state != State::Attacking) {
        return;
    }

    attackElapsed = std::max(0.0f, attackElapsed + deltaTime);
    if (attackHitExecuted || attackElapsed + kAttackCompletionEpsilon < attackHitDelay) {
        return;
    }

    attackHitExecuted = true;

    RTBEngine::Math::Vector3 hitPoint;
    if (PerformAttackSphereCast(&hitPoint)) {
        if (HealthComponent* health = ResolveTargetHealth()) {
            health->TakeDamage(attackDamage);
        }
    }
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
    attackElapsed = 0.0f;
    attackHitExecuted = false;

    if (animator && attackSlotState.ready && animator->GetClip(kAttackAlias)) {
        animator->Play(kAttackAlias, false);
        return;
    }

    if (attackAnimationFbx.empty()) {
        return;
    }

    RTB_WARN("[EnemyMeleeAI] Attack clip is missing; canceling the melee hit.");
    FinishAttack();
}

void EnemyMeleeAI::FinishAttack()
{
    attackElapsed = 0.0f;
    attackHitExecuted = false;
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

RTBEngine::Physics::PhysicsWorld* EnemyMeleeAI::ResolvePhysicsWorld() const
{
    auto resolveFromRigidBody = [](RTBEngine::ECS::GameObject* candidate) -> RTBEngine::Physics::PhysicsWorld* {
        if (!candidate) {
            return nullptr;
        }

        if (auto* rbComp = candidate->GetComponent<RTBEngine::ECS::RigidBodyComponent>()) {
            if (rbComp->HasRigidBody() && rbComp->GetRigidBody()) {
                return rbComp->GetRigidBody()->GetPhysicsWorld();
            }
        }

        if (auto* rbComp = candidate->GetComponentInChildren<RTBEngine::ECS::RigidBodyComponent>()) {
            if (rbComp->HasRigidBody() && rbComp->GetRigidBody()) {
                return rbComp->GetRigidBody()->GetPhysicsWorld();
            }
        }

        return nullptr;
    };

    auto resolveFromColliders = [](RTBEngine::ECS::GameObject* candidate) -> RTBEngine::Physics::PhysicsWorld* {
        if (!candidate) {
            return nullptr;
        }

        if (auto* box = candidate->GetComponent<RTBEngine::ECS::BoxColliderComponent>()) {
            if (box->GetPhysicsWorld()) {
                return box->GetPhysicsWorld();
            }
        }
        if (auto* sphere = candidate->GetComponent<RTBEngine::ECS::SphereColliderComponent>()) {
            if (sphere->GetPhysicsWorld()) {
                return sphere->GetPhysicsWorld();
            }
        }
        if (auto* capsule = candidate->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>()) {
            if (capsule->GetPhysicsWorld()) {
                return capsule->GetPhysicsWorld();
            }
        }

        if (auto* box = candidate->GetComponentInChildren<RTBEngine::ECS::BoxColliderComponent>()) {
            if (box->GetPhysicsWorld()) {
                return box->GetPhysicsWorld();
            }
        }
        if (auto* sphere = candidate->GetComponentInChildren<RTBEngine::ECS::SphereColliderComponent>()) {
            if (sphere->GetPhysicsWorld()) {
                return sphere->GetPhysicsWorld();
            }
        }
        if (auto* capsule = candidate->GetComponentInChildren<RTBEngine::ECS::CapsuleColliderComponent>()) {
            if (capsule->GetPhysicsWorld()) {
                return capsule->GetPhysicsWorld();
            }
        }

        return nullptr;
    };

    RTBEngine::ECS::GameObject* candidates[] = { owner, targetObject };
    for (RTBEngine::ECS::GameObject* candidate : candidates) {
        if (RTBEngine::Physics::PhysicsWorld* physicsWorld = resolveFromRigidBody(candidate)) {
            return physicsWorld;
        }

        if (RTBEngine::Physics::PhysicsWorld* physicsWorld = resolveFromColliders(candidate)) {
            return physicsWorld;
        }
    }

    return nullptr;
}

bool EnemyMeleeAI::IsWithinTargetHierarchy(RTBEngine::ECS::GameObject* candidate) const
{
    if (!candidate || !HasValidTarget()) {
        return false;
    }

    for (RTBEngine::ECS::GameObject* current = candidate; current; current = current->GetParent()) {
        if (current == targetObject) {
            return true;
        }
    }

    return false;
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

RTBEngine::Math::Vector3 EnemyMeleeAI::GetAttackOriginWorldPosition() const
{
    if (!attackOriginObject) {
        return RTBEngine::Math::Vector3::Zero();
    }

    return attackOriginObject->GetWorldPosition();
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

bool EnemyMeleeAI::PerformAttackSphereCast(RTBEngine::Math::Vector3* outHitPoint)
{
    if (!owner || !HasValidTarget() || !attackOriginObject) {
        return false;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld = ResolvePhysicsWorld();
    if (!physicsWorld) {
        return false;
    }

    RTBEngine::Math::Vector3 castStart = GetAttackOriginWorldPosition();
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
    RTBEngine::Physics::PhysicsQueryHit hit;
    RTBEngine::Physics::PhysicsQueryOptions queryOptions;
    queryOptions.ignoredObject = owner;
    queryOptions.ignoreIgnoredObjectHierarchy = true;
    queryOptions.ignoreTriggers = true;

    if (!physicsWorld->SphereCastClosest(castStart, castEnd, attackSphereRadius, hit, queryOptions)) {
        return false;
    }

    if (!IsWithinTargetHierarchy(hit.gameObject)) {
        return false;
    }

    if (outHitPoint) {
        *outHitPoint = hit.point;
    }

    return true;
}
