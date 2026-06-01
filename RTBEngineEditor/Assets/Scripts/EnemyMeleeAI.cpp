#include "EnemyMeleeAI.h"

#include "EnemyAnimationDriver.h"
#include "EnemyLocomotionController.h"
#include "EnemyMeleeAIShared.h"
#include "EnemyTargetTracker.h"
#include "MeleeSphereAttackAbility.h"
#include "OnlineGameNetMessages.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/NetworkTransform.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>

using ThisClass = EnemyMeleeAI;

RTB_REGISTER_COMPONENT(EnemyMeleeAI)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_RANGE(team, 0, 8)
    RTB_PROPERTY_COMPONENT(targetTracker, EnemyTargetTracker)
    RTB_PROPERTY_COMPONENT(animationDriver, EnemyAnimationDriver)
    RTB_PROPERTY_COMPONENT(locomotion, EnemyLocomotionController)
    RTB_PROPERTY_COMPONENT(meleeAttack, MeleeSphereAttackAbility)
    RTB_PROPERTY_RANGE(attackRange, 0.1f, 5.0f)
    RTB_PROPERTY_RANGE(preferredAttackDistance, 0.1f, 5.0f)
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
        lastReplicatedPosition = owner->GetWorldPosition();
        hasReplicatedPosition = true;
    }

    if (health && health->IsDead()) {
        HandleDeath(health->GetLastDeathEvent());
        return;
    }

    if (!HasSimulationAuthority()) {
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

    if (!HasSimulationAuthority()) {
        switch (state) {
        case State::Dying:
            UpdateDying(deltaTime);
            break;
        case State::Shrinking:
            UpdateShrinking(deltaTime);
            break;
        default:
            break;
        }
        return;
    }

    switch (state) {
    case State::Attacking:
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

    if (!HasSimulationAuthority()) {
        locomotion->StopPlanarMotion();
        return;
    }

    switch (state) {
    case State::Chasing:
        if (targetTracker) {
            locomotion->MoveTowards(targetTracker->GetPlanarDirectionTo(owner), fixedDeltaTime);
        }
        break;
    case State::Repositioning:
        if (targetTracker) {
            locomotion->MoveTowards(targetTracker->GetPlanarDirectionTo(owner) * -1.0f, fixedDeltaTime);
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

void EnemyMeleeAI::OnLateUpdate(float deltaTime)
{
    ResolveDependencies();

    if (!HasSimulationAuthority()) {
        UpdateReplicatedLocomotionAnimation(deltaTime);
        return;
    }

    if (state == State::Attacking) {
        const bool abilityActive = meleeAttack && meleeAttack->IsAbilityActive();

        if (!animationDriver || !animationDriver->HasAttackAnimation()) {
            if (!abilityActive) {
                FinishAttack();
            }
            return;
        }

        if (animationDriver->IsAttackPlaying() || abilityActive) {
            return;
        }

        FinishAttack();
        return;
    }

    if (!animationDriver) {
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
    if (meleeAttack) {
        meleeAttack->CancelAbility();
        meleeAttack->ClearTargetContext();
    }
    UnsubscribeFromHealth();
}

void EnemyMeleeAI::ClampSettings()
{
    team = std::max(0, team);
    attackRange = std::max(0.1f, attackRange);
    preferredAttackDistance = std::clamp(preferredAttackDistance, 0.1f, attackRange);
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
    ResolveMeleeAttack();
}

void EnemyMeleeAI::ResolveMeleeAttack()
{
    if (meleeAttack || !owner) {
        return;
    }

    meleeAttack = owner->GetComponent<MeleeSphereAttackAbility>();
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
    constexpr float kAttackDistanceTolerance = 0.15f;

    if (!HasValidCombatSetup()) {
        EnterIdle();
        return;
    }

    if (!targetTracker->HasValidTarget(owner) || !targetTracker->IsTargetAlive(owner)) {
        EnterIdle();
        return;
    }

    const float targetDistance = targetTracker->GetPlanarDistanceTo(owner);
    const float minAttackDistance = std::max(0.1f, preferredAttackDistance - kAttackDistanceTolerance);

    if (targetDistance < minAttackDistance) {
        EnterRepositioning();
        return;
    }

    if (targetDistance <= attackRange) {
        if (!meleeAttack->IsCoolingDown()) {
            StartAttack();
        } else {
            EnterIdle();
        }
        return;
    }

    EnterChasing();
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

    HealthComponent* targetHealth = targetTracker->ResolveTargetHealth();
    RTBEngine::ECS::GameObject* targetRoot = targetTracker->targetObject;
    RTBEngine::Physics::PhysicsWorld* physicsWorld = ResolvePhysicsWorld();
    RTBEngine::Math::Vector3 attackDirection = targetTracker->GetPlanarDirectionTo(owner);

    meleeAttack->SetTargetContext(targetRoot, targetHealth, physicsWorld);
    if (!meleeAttack->TryActivate(owner, attackDirection)) {
        meleeAttack->ClearTargetContext();
        EnterIdle();
        return;
    }

    state = State::Attacking;
    if (locomotion) {
        locomotion->StopPlanarMotion();
    }
    if (animationDriver) {
        animationDriver->PlayAttack();
    }
}

void EnemyMeleeAI::FinishAttack()
{
    UpdateState();
}

void EnemyMeleeAI::EnterIdle()
{
    state = State::Idle;
}

void EnemyMeleeAI::EnterChasing()
{
    state = State::Chasing;
    if (animationDriver) {
        animationDriver->PlayWalkLoop();
    }
}

void EnemyMeleeAI::EnterRepositioning()
{
    state = State::Repositioning;
    if (animationDriver) {
        animationDriver->PlayWalkLoop();
    }
}

bool EnemyMeleeAI::HasValidCombatSetup() const
{
    return owner && targetTracker && locomotion && meleeAttack;
}

RTBEngine::Physics::PhysicsWorld* EnemyMeleeAI::ResolvePhysicsWorld() const
{
    if (!owner) {
        return nullptr;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (rbComp && rbComp->HasRigidBody() && rbComp->GetRigidBody()) {
        if (RTBEngine::Physics::PhysicsWorld* world = rbComp->GetRigidBody()->GetPhysicsWorld()) {
            return world;
        }
    }

    if (targetTracker && targetTracker->targetObject) {
        auto* targetRigidBody = targetTracker->targetObject->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
        if (targetRigidBody && targetRigidBody->HasRigidBody() && targetRigidBody->GetRigidBody()) {
            return targetRigidBody->GetRigidBody()->GetPhysicsWorld();
        }
    }

    return nullptr;
}

void EnemyMeleeAI::UpdateReplicatedLocomotionAnimation(float deltaTime)
{
    if (!owner || !animationDriver || deltaTime <= 0.0f) {
        return;
    }

    if (state == State::Attacking) {
        return;
    }

    const RTBEngine::Math::Vector3 currentPosition = owner->GetWorldPosition();
    if (!hasReplicatedPosition) {
        lastReplicatedPosition = currentPosition;
        hasReplicatedPosition = true;
        return;
    }

    RTBEngine::Math::Vector3 displacement = currentPosition - lastReplicatedPosition;
    displacement.y = 0.0f;
    lastReplicatedPosition = currentPosition;

    constexpr float kMinWalkSpeed = 0.15f;
    const float planarSpeed = displacement.Length() / deltaTime;
    if (planarSpeed >= kMinWalkSpeed &&
        (state == State::Chasing || state == State::Repositioning || state == State::Idle)) {
        animationDriver->PlayWalkLoop();
    }
}

void EnemyMeleeAI::HandleDamageTaken(const HealthComponent::DamageTakenEvent& /*eventData*/)
{
}

void EnemyMeleeAI::HandleDeath(const HealthComponent::DeathEvent& /*eventData*/)
{
    if (!owner || state == State::Dying || state == State::Shrinking || state == State::Dead) {
        return;
    }

    state = State::Dying;
    hitReactRemaining = 0.0f;
    deathHoldRemaining = deathHoldDuration;
    shrinkElapsed = 0.0f;
    deathPoseLocked = false;

    if (meleeAttack) {
        meleeAttack->CancelAbility();
        meleeAttack->ClearTargetContext();
    }

    if (locomotion) {
        locomotion->StopPlanarMotion();
    }

    if (!animationDriver || !animationDriver->PlayDeath()) {
        deathPoseLocked = true;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        if (RTBEngine::ECS::NetworkTransform* networkTransform =
                owner->GetComponent<RTBEngine::ECS::NetworkTransform>()) {
            if (!networkTransform->objectKey.empty()) {
                GameNet::OnlineGameNetSubsystem::BroadcastEnemyDeath(networkTransform->objectKey);
            }
        }
    }
}

void EnemyMeleeAI::HandleCharacterDeath(const HealthComponent::DeathEvent& eventData)
{
    HandleDeath(eventData);
}

HealthComponent*& EnemyMeleeAI::AccessHealthSlot()
{
    return health;
}

HealthComponent* EnemyMeleeAI::PeekHealthSlot() const
{
    return health;
}

int EnemyMeleeAI::GetCharacterTeam() const
{
    return team;
}
