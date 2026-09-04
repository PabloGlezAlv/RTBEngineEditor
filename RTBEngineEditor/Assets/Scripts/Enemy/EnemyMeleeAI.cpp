#include "EnemyMeleeAI.h"

#include "EnemyAnimationDriver.h"
#include "EnemyLocomotionController.h"
#include "EnemyMeleeAIShared.h"
#include "EnemyTargetTracker.h"
#include "MeleeSphereAttackAbility.h"
#include "OnlineGameNetMessages.h"
#include "RoundManager.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Scene/ComponentQuery.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NavAgentComponent.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>

using ThisClass = EnemyMeleeAI;

namespace {

    constexpr float kTargetRefreshInterval = 0.5f;

    RoundManager* FindRoundManagerInScene()
    {
        return RTBEngine::Scene::ComponentQuery::FindFirst<RoundManager>();
    }

}

RTB_REGISTER_COMPONENT(EnemyMeleeAI)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_RANGE(team, 0, 8)
    RTB_PROPERTY_COMPONENT(targetTracker, EnemyTargetTracker)
    RTB_PROPERTY_COMPONENT(animationDriver, EnemyAnimationDriver)
    RTB_PROPERTY_COMPONENT(locomotion, EnemyLocomotionController)
    RTB_PROPERTY_COMPONENT(navAgent, RTBEngine::Scene::NavAgentComponent)
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
    roundManager = FindRoundManagerInScene();

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

void EnemyMeleeAI::FinalizeSpawnSetup()
{
    RebindHealthSubscriptions();

    if (!HasSimulationAuthority()) {
        return;
    }

    UpdateState();
}

void EnemyMeleeAI::SetRoundManager(RoundManager* manager)
{
    roundManager = manager;
}

void EnemyMeleeAI::OnUpdate(float deltaTime)
{
    if (IsStunned()) {
        return;
    }

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
    if (!locomotion) {
        return;
    }

    if (IsStunned()) {
        locomotion->StopPlanarMotion();
        return;
    }

    if (!HasSimulationAuthority()) {
        locomotion->StopPlanarMotion();
        return;
    }

    switch (state) {
    case State::Chasing:
        if (navAgent && targetTracker && targetTracker->targetObject) {
            navAgent->SetDestination(targetTracker->targetObject->GetWorldPosition());
            navAgent->EnsurePathReady();
            if (navAgent->HasActivePath()) {
                const RTBEngine::Math::Vector3 moveDirection =
                    navAgent->GetPlanarMoveDirection(owner->GetWorldPosition());
                if (EnemyMeleeAIDetail::HasPlanarDirection(moveDirection)) {
                    locomotion->MoveTowards(moveDirection, fixedDeltaTime);
                } else {
                    locomotion->StopPlanarMotion();
                }
            } else {
                locomotion->StopPlanarMotion();
            }
        } else {
            if (!navAgent) {
                static bool warnedMissingNavAgent = false;
                if (!warnedMissingNavAgent) {
                    warnedMissingNavAgent = true;
                    RTB_WARN("[EnemyMeleeAI] Chasing without NavAgentComponent on '" +
                        owner->GetName() + "'; enemy will stay still.");
                }
            }
            locomotion->StopPlanarMotion();
        }
        break;
    case State::Repositioning:
        if (navAgent && hasRepositionDestination && owner) {
            navAgent->EnsurePathReady();
            if (navAgent->HasActivePath()) {
                const RTBEngine::Math::Vector3 moveDirection =
                    navAgent->GetPlanarMoveDirection(owner->GetWorldPosition());
                if (EnemyMeleeAIDetail::HasPlanarDirection(moveDirection)) {
                    locomotion->MoveTowards(moveDirection, fixedDeltaTime);
                } else {
                    locomotion->StopPlanarMotion();
                }
            } else {
                locomotion->StopPlanarMotion();
            }
        } else {
            locomotion->StopPlanarMotion();
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
    if (!HasSimulationAuthority()) {
        if (state == State::Attacking) {
            if (animationDriver && animationDriver->IsAttackPlaying()) {
                return;
            }

            state = State::Idle;
            return;
        }

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
        LockDeathPose();
        return;
    }

    if (animationDriver->IsDeathPlaying()) {
        return;
    }

    LockDeathPose();
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
    if (!navAgent) {
        navAgent = owner->GetComponent<RTBEngine::Scene::NavAgentComponent>();
    }
    CacheCharacterBaseReferences();
    ResolveMeleeAttack();
}

void EnemyMeleeAI::ResolveMeleeAttack()
{
    if (meleeAttack) {
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

void EnemyMeleeAI::RefreshClosestTarget()
{
    if (!targetTracker || !HasSimulationAuthority()) {
        return;
    }

    targetRefreshRemaining = std::max(0.0f, targetRefreshRemaining - RTBEngine::Core::Time::GetDeltaTime());
    if (targetRefreshRemaining > 0.0f) {
        return;
    }

    targetRefreshRemaining = kTargetRefreshInterval;

    if (!roundManager) {
        return;
    }

    RTBEngine::Scene::GameObject* closestTarget = roundManager->FindClosestPlayerTarget(owner);
    if (!closestTarget) {
        return;
    }

    if (closestTarget == targetTracker->targetObject) {
        return;
    }

    targetTracker->SetTarget(closestTarget);
    if (navAgent) {
        navAgent->SetDestination(closestTarget->GetWorldPosition());
        navAgent->EnsurePathReady();
    }
}

void EnemyMeleeAI::UpdateState()
{
    RefreshClosestTarget();

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
    RTBEngine::Scene::GameObject* targetRoot = targetTracker->targetObject;
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

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        if (RTBEngine::Scene::NetworkIdentity* identity = GetNetworkIdentity()) {
            if (identity->HasNetworkId()) {
                ++networkAttackSequence;
                GameNet::EnemyAttackSnapshot attackSnapshot;
                attackSnapshot.networkId = identity->GetNetworkId();
                attackSnapshot.attackSequence = networkAttackSequence;
                GameNet::OnlineGameNetSubsystem::BroadcastEnemyAttack(attackSnapshot);
            }
        }
    }
}

void EnemyMeleeAI::PlayReplicatedAttack(std::uint32_t attackSequence)
{
    if (HasSimulationAuthority() || attackSequence == 0) {
        return;
    }

    if (attackSequence <= lastProcessedReplicatedAttackSequence) {
        return;
    }

    lastProcessedReplicatedAttackSequence = attackSequence;

    if (state == State::Dying || state == State::Shrinking || state == State::Dead) {
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
    if (navAgent && targetTracker && targetTracker->targetObject) {
        navAgent->SetDestination(targetTracker->targetObject->GetWorldPosition());
    }
    if (animationDriver) {
        animationDriver->PlayWalkLoop();
    }
}

void EnemyMeleeAI::EnterRepositioning()
{
    state = State::Repositioning;
    hasRepositionDestination = false;

    // Retreat destination is chosen here once; FixedUpdate only follows the existing path.
    if (navAgent && targetTracker && targetTracker->targetObject && owner) {
        const RTBEngine::Math::Vector3 awayDirection =
            targetTracker->GetPlanarDirectionTo(owner) * -1.0f;
        if (EnemyMeleeAIDetail::HasPlanarDirection(awayDirection)) {
            constexpr float kRepositionDistance = 2.0f;
            repositionDestination = owner->GetWorldPosition() + awayDirection * kRepositionDistance;
            navAgent->SetDestination(repositionDestination);
            navAgent->EnsurePathReady();
            hasRepositionDestination = true;
        }
    }

    if (animationDriver) {
        animationDriver->PlayWalkLoop();
    }
}

bool EnemyMeleeAI::HasValidCombatSetup() const
{
    return targetTracker && locomotion && meleeAttack;
}

RTBEngine::Physics::PhysicsWorld* EnemyMeleeAI::ResolvePhysicsWorld() const
{
    auto* rbComp = owner->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    if (rbComp && rbComp->HasRigidBody() && rbComp->GetRigidBody()) {
        if (RTBEngine::Physics::PhysicsWorld* world = rbComp->GetRigidBody()->GetPhysicsWorld()) {
            return world;
        }
    }

    if (targetTracker && targetTracker->targetObject) {
        auto* targetRigidBody = targetTracker->targetObject->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
        if (targetRigidBody && targetRigidBody->HasRigidBody() && targetRigidBody->GetRigidBody()) {
            return targetRigidBody->GetRigidBody()->GetPhysicsWorld();
        }
    }

    return nullptr;
}

void EnemyMeleeAI::UpdateReplicatedLocomotionAnimation(float deltaTime)
{
    if (!animationDriver || deltaTime <= 0.0f) {
        return;
    }

    if (state == State::Attacking || state == State::Dying || state == State::Shrinking || state == State::Dead) {
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

void EnemyMeleeAI::HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData)
{
    if (eventData.damage.amount <= 0.0f ||
        state == State::Dead || state == State::Dying || state == State::Shrinking) {
        return;
    }

    if (!HasSimulationAuthority()) {
        return;
    }

    if (meleeAttack && meleeAttack->IsAbilityActive()) {
        meleeAttack->CancelAbility();
        meleeAttack->ClearTargetContext();
    }

    if (locomotion) {
        RTBEngine::Math::Vector3 fallbackDirection = RTBEngine::Math::Vector3::Zero();
        if (eventData.damage.instigator) {
            fallbackDirection =
                owner->GetWorldPosition() - eventData.damage.instigator->GetWorldPosition();
        }

        locomotion->ApplyKnockback(
            eventData.damage.hitDirection,
            fallbackDirection,
            eventData.damage.knockbackStrength);
    }

    state = State::HitReact;
    hitReactRemaining = hitReactDuration;
}

void EnemyMeleeAI::HandleDeath(const HealthComponent::DeathEvent& /*eventData*/)
{
    if (state == State::Dying || state == State::Shrinking || state == State::Dead) {
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
        LockDeathPose();
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        if (RTBEngine::Scene::NetworkIdentity* identity = GetNetworkIdentity()) {
            if (identity->HasNetworkId()) {
                GameNet::OnlineGameNetSubsystem::BroadcastEnemyDeath(identity->GetNetworkId());
            }
        }
    }
}

void EnemyMeleeAI::LockDeathPose()
{
    if (deathPoseLocked) {
        return;
    }

    deathPoseLocked = true;

    if (animationDriver) {
        animationDriver->HoldDeathPose();
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
