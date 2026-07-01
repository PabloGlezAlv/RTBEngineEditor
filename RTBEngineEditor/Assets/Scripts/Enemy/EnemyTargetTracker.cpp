#include "EnemyTargetTracker.h"

#include "EnemyMeleeAIShared.h"

#include <RTBEngine/Scene/GameObject.h>

using ThisClass = EnemyTargetTracker;

RTB_REGISTER_COMPONENT(EnemyTargetTracker)
    RTB_PROPERTY_GAMEOBJECT(targetObject)
RTB_END_REGISTER(EnemyTargetTracker)

void EnemyTargetTracker::OnStart()
{
    SanitizeTarget();
}

void EnemyTargetTracker::OnValidate()
{
    SanitizeTarget();
}

bool EnemyTargetTracker::HasValidTarget(const RTBEngine::ECS::GameObject* requester) const
{
    return targetObject != nullptr && targetObject != requester;
}

bool EnemyTargetTracker::IsTargetAlive(const RTBEngine::ECS::GameObject* requester) const
{
    if (!HasValidTarget(requester)) {
        return false;
    }

    if (HealthComponent* targetHealth = ResolveTargetHealth()) {
        return !targetHealth->IsDead();
    }

    return true;
}

HealthComponent* EnemyTargetTracker::ResolveTargetHealth() const
{
    if (!targetObject) {
        return nullptr;
    }

    if (HealthComponent* targetHealth = targetObject->GetComponent<HealthComponent>()) {
        return targetHealth;
    }

    return targetObject->GetComponentInChildren<HealthComponent>();
}

float EnemyTargetTracker::GetPlanarDistanceTo(const RTBEngine::ECS::GameObject* requester) const
{
    if (!requester || !HasValidTarget(requester)) {
        return 0.0f;
    }

    RTBEngine::Math::Vector3 direction = targetObject->GetWorldPosition() - requester->GetWorldPosition();
    direction.y = 0.0f;
    return direction.Length();
}

RTBEngine::Math::Vector3 EnemyTargetTracker::GetPlanarDirectionTo(const RTBEngine::ECS::GameObject* requester) const
{
    if (!requester || !HasValidTarget(requester)) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 direction = targetObject->GetWorldPosition() - requester->GetWorldPosition();
    direction.y = 0.0f;

    if (!EnemyMeleeAIDetail::HasPlanarDirection(direction)) {
        return RTBEngine::Math::Vector3::Zero();
    }

    direction.Normalize();
    return direction;
}

bool EnemyTargetTracker::IsWithinTargetHierarchy(const RTBEngine::ECS::GameObject* requester,
                                                 RTBEngine::ECS::GameObject* candidate) const
{
    if (!candidate || !HasValidTarget(requester)) {
        return false;
    }

    for (RTBEngine::ECS::GameObject* current = candidate; current; current = current->GetParent()) {
        if (current == targetObject) {
            return true;
        }
    }

    return false;
}

void EnemyTargetTracker::SetTarget(RTBEngine::ECS::GameObject* target)
{
    targetObject = (target != owner) ? target : nullptr;
}

void EnemyTargetTracker::SanitizeTarget()
{
    if (targetObject == owner) {
        targetObject = nullptr;
    }
}
