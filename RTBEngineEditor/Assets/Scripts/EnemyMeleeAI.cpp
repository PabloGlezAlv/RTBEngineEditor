#include "EnemyMeleeAI.h"

#include "EnemyAnimationDriver.h"
#include "EnemyLocomotionController.h"
#include "EnemyMeleeAIShared.h"
#include "EnemyTargetTracker.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>

using ThisClass = EnemyMeleeAI;

RTB_REGISTER_COMPONENT(EnemyMeleeAI)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_COMPONENT(targetTracker, EnemyTargetTracker)
    RTB_PROPERTY_COMPONENT(animationDriver, EnemyAnimationDriver)
    RTB_PROPERTY_COMPONENT(locomotion, EnemyLocomotionController)
    RTB_PROPERTY_GAMEOBJECT(attackOriginObject)
    RTB_PROPERTY_RANGE(attackRange, 0.1f, 5.0f)
    RTB_PROPERTY_RANGE(attackCooldown, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(attackDamage, 0.0f, 100.0f)
    RTB_PROPERTY_RANGE(attackHitDelay, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(attackSphereRadius, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(attackSphereDistance, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(hitReactDuration, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(deathHoldDuration, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(shrinkDuration, 0.01f, 10.0f)
RTB_END_REGISTER(EnemyMeleeAI)

void EnemyMeleeAI::OnStart()
{
    ClampSettings();
    ResolveDependencies();
    RebindHealthSubscriptions();

    if (owner) {
        initialScale = owner->GetTransform().GetScale();
    }

    if (health && health->IsDead()) {
        HandleDeath(health->GetLastDeathEvent());
        return;
    }

    UpdateState();
}

void EnemyMeleeAI::OnUpdate(float deltaTime)
{
    if (!owner) {
        return;
    }

    ClampSettings();
    ResolveDependencies();
    RebindHealthSubscriptions();
    cooldownRemaining = std::max(0.0f, cooldownRemaining - deltaTime);

    switch (state) {
    case State::Attacking:
        UpdateAttack(deltaTime);
        break;
    case State::HitReact:
        UpdateHitReact(deltaTime);
        break;
    case State::Dying:
        UpdateDying(deltaTime);
        break;
    case State::Shrinking:
        UpdateShrinking(deltaTime);
        break;
    case State::Dead:
        break;
    default:
        UpdateState();
        break;
    }
}

void EnemyMeleeAI::OnFixedUpdate(float fixedDeltaTime)
{
    if (!owner) {
        return;
    }

    ResolveDependencies();
    if (!locomotion) {
        return;
    }

    switch (state) {
    case State::Chasing:
        if (targetTracker) {
            locomotion->MoveTowards(targetTracker->GetPlanarDirectionTo(owner), fixedDeltaTime);
        }
        break;
    case State::Attacking:
    case State::Dying:
    case State::Shrinking:
    case State::Dead:
        locomotion->StopPlanarMotion();
        break;
    case State::HitReact:
    case State::Idle:
    default:
        break;
    }
}

void EnemyMeleeAI::OnLateUpdate(float /*deltaTime*/)
{
    ResolveDependencies();
    if (!animationDriver) {
        return;
    }

    if (state == State::Attacking) {
        if (!animationDriver->HasAttackAnimation()) {
            if (attackHitExecuted &&
                attackElapsed + EnemyMeleeAIDetail::kAttackCompletionEpsilon >=
                    attackHitDelay + EnemyMeleeAIDetail::kFallbackAttackDuration) {
                FinishAttack();
            }
            return;
        }

        if (animationDriver->IsAttackPlaying()) {
            return;
        }

        FinishAttack();
        return;
    }

    if (state != State::Dying || deathPoseLocked) {
        return;
    }

    if (!animationDriver->HasDeathAnimation()) {
        deathPoseLocked = true;
        return;
    }

    if (animationDriver->IsDeathPlaying()) {
        return;
    }

    deathPoseLocked = true;
}

void EnemyMeleeAI::OnValidate()
{
    ClampSettings();
    ResolveDependencies();

    if (owner) {
        initialScale = owner->GetTransform().GetScale();
    }
}

void EnemyMeleeAI::OnDestroy()
{
    UnsubscribeFromHealth();
}

void EnemyMeleeAI::ClampSettings()
{
    attackRange = std::max(0.1f, attackRange);
    attackCooldown = std::max(0.0f, attackCooldown);
    attackDamage = std::max(0.0f, attackDamage);
    attackHitDelay = std::max(0.0f, attackHitDelay);
    attackSphereRadius = std::max(0.05f, attackSphereRadius);
    attackSphereDistance = std::max(0.05f, attackSphereDistance);
    hitReactDuration = std::max(0.0f, hitReactDuration);
    deathHoldDuration = std::max(0.0f, deathHoldDuration);
    shrinkDuration = std::max(0.01f, shrinkDuration);
}

void EnemyMeleeAI::ResolveDependencies()
{
    if (!owner) {
        return;
    }

    if (!health) {
        health = owner->GetComponent<HealthComponent>();
    }
    if (!targetTracker) {
        targetTracker = owner->GetComponent<EnemyTargetTracker>();
    }
    if (!animationDriver) {
        animationDriver = owner->GetComponent<EnemyAnimationDriver>();
    }
    if (!locomotion) {
        locomotion = owner->GetComponent<EnemyLocomotionController>();
    }
}

void EnemyMeleeAI::RebindHealthSubscriptions()
{
    if (subscribedHealth == health &&
        damageTakenSubscription.IsValid() &&
        deathSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromHealth();
    if (!health) {
        return;
    }

    subscribedHealth = health;
    damageTakenSubscription = health->SubscribeToDamageTaken(
        [this](const HealthComponent::DamageTakenEvent& eventData) {
            HandleDamageTaken(eventData);
        });
    deathSubscription = health->SubscribeToDeath(
        [this](const HealthComponent::DeathEvent& eventData) {
            HandleDeath(eventData);
        });
}

void EnemyMeleeAI::UnsubscribeFromHealth()
{
    damageTakenSubscription.Reset();
    deathSubscription.Reset();
    subscribedHealth = nullptr;
}

void EnemyMeleeAI::UpdateState()
{
    if (!HasValidCombatSetup()) {
        EnterIdle();
        return;
    }

    if (!targetTracker->HasValidTarget(owner) || !targetTracker->IsTargetAlive(owner)) {
        EnterIdle();
        return;
    }

    if (targetTracker->GetPlanarDistanceTo(owner) <= attackRange && cooldownRemaining <= 0.0f) {
        StartAttack();
        return;
    }

    EnterChasing();
}

void EnemyMeleeAI::UpdateAttack(float deltaTime)
{
    attackElapsed = std::max(0.0f, attackElapsed + deltaTime);
    if (attackHitExecuted ||
        attackElapsed + EnemyMeleeAIDetail::kAttackCompletionEpsilon < attackHitDelay) {
        return;
    }

    attackHitExecuted = true;

    RTBEngine::Math::Vector3 hitPoint;
    RTBEngine::Math::Vector3 hitDirection;
    if (!PerformAttackSphereCast(&hitPoint, &hitDirection)) {
        return;
    }

    if (HealthComponent* targetHealth = targetTracker ? targetTracker->ResolveTargetHealth() : nullptr) {
        HealthComponent::DamageContext damageContext;
        damageContext.amount = attackDamage;
        damageContext.instigator = owner;
        damageContext.hitPoint = hitPoint;
        damageContext.hitDirection = hitDirection;
        targetHealth->TakeDamage(attackDamage, damageContext);
    }
}

void EnemyMeleeAI::UpdateHitReact(float deltaTime)
{
    hitReactRemaining = std::max(0.0f, hitReactRemaining - deltaTime);
    if (hitReactRemaining > 0.0f) {
        return;
    }

    UpdateState();
}

void EnemyMeleeAI::UpdateDying(float deltaTime)
{
    if (!deathPoseLocked) {
        return;
    }

    deathHoldRemaining = std::max(0.0f, deathHoldRemaining - deltaTime);
    if (deathHoldRemaining > 0.0f) {
        return;
    }

    state = State::Shrinking;
    shrinkElapsed = 0.0f;
}

void EnemyMeleeAI::UpdateShrinking(float deltaTime)
{
    shrinkElapsed = std::max(0.0f, shrinkElapsed + deltaTime);
    const float normalized = RTBEngine::Math::Clamp01(shrinkElapsed / shrinkDuration);
    const float eased = RTBEngine::Math::EaseInCubic(normalized);
    owner->GetTransform().SetScale(
        RTBEngine::Math::Lerp(initialScale, RTBEngine::Math::Vector3::Zero(), eased));

    if (normalized < 1.0f) {
        return;
    }

    if (locomotion) {
        locomotion->StopPlanarMotion();
    }
    owner->SetActive(false);
    state = State::Dead;
}

void EnemyMeleeAI::StartAttack()
{
    if (!HasValidCombatSetup() ||
        !targetTracker->HasValidTarget(owner) ||
        !targetTracker->IsTargetAlive(owner)) {
        EnterIdle();
        return;
    }

    state = State::Attacking;
    attackElapsed = 0.0f;
    attackHitExecuted = false;
    if (locomotion) {
        locomotion->StopPlanarMotion();
    }
    if (animationDriver) {
        animationDriver->PlayAttack();
    }
}

void EnemyMeleeAI::FinishAttack()
{
    attackElapsed = 0.0f;
    attackHitExecuted = false;
    cooldownRemaining = attackCooldown;
    UpdateState();
}

void EnemyMeleeAI::EnterIdle()
{
    state = State::Idle;
    attackElapsed = 0.0f;
    attackHitExecuted = false;
}

void EnemyMeleeAI::EnterChasing()
{
    state = State::Chasing;
    if (animationDriver) {
        animationDriver->PlayWalkLoop();
    }
}

bool EnemyMeleeAI::HasValidCombatSetup() const
{
    return owner && targetTracker && locomotion;
}

bool EnemyMeleeAI::PerformAttackSphereCast(RTBEngine::Math::Vector3* outHitPoint,
                                           RTBEngine::Math::Vector3* outHitDirection)
{
    if (!HasValidCombatSetup() || !targetTracker->HasValidTarget(owner)) {
        return false;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld = ResolvePhysicsWorld();
    if (!physicsWorld) {
        return false;
    }

    RTBEngine::Math::Vector3 castStart =
        attackOriginObject ? attackOriginObject->GetWorldPosition() : owner->GetWorldPosition();
    RTBEngine::Math::Vector3 castDirection = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
    castDirection.y = 0.0f;
    if (!EnemyMeleeAIDetail::HasPlanarDirection(castDirection)) {
        castDirection = targetTracker->GetPlanarDirectionTo(owner);
    }
    if (!EnemyMeleeAIDetail::HasPlanarDirection(castDirection)) {
        castDirection = RTBEngine::Math::Vector3::Forward();
    } else {
        castDirection.Normalize();
    }

    RTBEngine::Physics::PhysicsQueryHit hit;
    RTBEngine::Physics::PhysicsQueryOptions queryOptions;
    queryOptions.ignoredObject = owner;
    queryOptions.ignoreIgnoredObjectHierarchy = true;
    queryOptions.ignoreTriggers = true;

    if (!physicsWorld->SphereCastClosest(
            castStart,
            castStart + castDirection * attackSphereDistance,
            attackSphereRadius,
            hit,
            queryOptions)) {
        return false;
    }

    if (!targetTracker->IsWithinTargetHierarchy(owner, hit.gameObject)) {
        return false;
    }

    if (outHitPoint) {
        *outHitPoint = hit.point;
    }
    if (outHitDirection) {
        *outHitDirection = castDirection;
    }
    return true;
}

RTBEngine::Physics::PhysicsWorld* EnemyMeleeAI::ResolvePhysicsWorld() const
{
    if (!owner) {
        return nullptr;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (rbComp && rbComp->HasRigidBody() && rbComp->GetRigidBody()) {
        return rbComp->GetRigidBody()->GetPhysicsWorld();
    }

    return nullptr;
}

void EnemyMeleeAI::HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData)
{
    if (!owner || state == State::Dying || state == State::Shrinking || state == State::Dead) {
        return;
    }

    if (eventData.currentHealth <= 0.0f) {
        return;
    }

    state = State::HitReact;
    attackElapsed = 0.0f;
    attackHitExecuted = false;
    hitReactRemaining = hitReactDuration;
    cooldownRemaining = std::max(cooldownRemaining, attackCooldown * 0.5f);

    if (locomotion && targetTracker) {
        locomotion->ApplyKnockback(
            eventData.damage.hitDirection,
            targetTracker->GetPlanarDirectionTo(owner) * -1.0f);
    }
}

void EnemyMeleeAI::HandleDeath(const HealthComponent::DeathEvent& /*eventData*/)
{
    if (!owner || state == State::Dying || state == State::Shrinking || state == State::Dead) {
        return;
    }

    state = State::Dying;
    cooldownRemaining = 0.0f;
    attackElapsed = 0.0f;
    attackHitExecuted = false;
    hitReactRemaining = 0.0f;
    deathHoldRemaining = deathHoldDuration;
    shrinkElapsed = 0.0f;
    deathPoseLocked = false;

    if (locomotion) {
        locomotion->StopPlanarMotion();
    }

    if (!animationDriver || !animationDriver->PlayDeath()) {
        deathPoseLocked = true;
    }
}
