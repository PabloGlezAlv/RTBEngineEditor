#include "ProjectileComponent.h"

#include "CharacterBase.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>
#include <cmath>

using ThisClass = ProjectileComponent;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;
    constexpr float kDistanceEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    bool IsSameOrDescendant(const RTBEngine::ECS::GameObject* candidate,
                            const RTBEngine::ECS::GameObject* root)
    {
        for (const RTBEngine::ECS::GameObject* current = candidate; current; current = current->GetParent()) {
            if (current == root) {
                return true;
            }
        }

        return false;
    }

    int ResolveCharacterTeam(RTBEngine::ECS::GameObject* gameObject)
    {
        if (!gameObject) {
            return static_cast<int>(CharacterTeam::Neutral);
        }

        for (RTBEngine::ECS::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam();
            }
        }

        return static_cast<int>(CharacterTeam::Neutral);
    }

    bool IsOtherPlayer(RTBEngine::ECS::GameObject* hitObject,
                       const RTBEngine::ECS::GameObject* instigator)
    {
        if (!hitObject || !instigator) {
            return false;
        }

        if (IsSameOrDescendant(hitObject, instigator)) {
            return false;
        }

        for (RTBEngine::ECS::GameObject* current = hitObject; current; current = current->GetParent()) {
            if (current == instigator) {
                return false;
            }

            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam() == static_cast<int>(CharacterTeam::Player);
            }
        }

        return false;
    }
}

RTB_REGISTER_COMPONENT(ProjectileComponent)
    RTB_PROPERTY_RANGE(speed, 0.01f, 50.0f)
    RTB_PROPERTY_RANGE(maxDistance, 0.05f, 50.0f)
    RTB_PROPERTY_RANGE(radius, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(damage, 0.0f, 1000.0f)
    RTB_PROPERTY_RANGE(instigatorTeam, 0, 8)
    RTB_PROPERTY(ignoreSameTeam)
    RTB_PROPERTY(destroyOnHit)
    RTB_PROPERTY_RANGE(maxHits, 0, 100)
RTB_END_REGISTER(ProjectileComponent)

void ProjectileComponent::OnStart()
{
    if (!initialized) {
        InitializeFromOwnerTransform();
    }
}

void ProjectileComponent::OnUpdate(float deltaTime)
{
    if (!owner || pendingDestroy) {
        return;
    }

    if (!initialized) {
        InitializeFromOwnerTransform();
    }

    if (deltaTime <= 0.0f) {
        return;
    }

    const float remainingDistance = maxDistance - distanceTravelled;
    if (remainingDistance <= kDistanceEpsilon) {
        DestroyProjectile();
        return;
    }

    const float stepDistance = std::min(speed * deltaTime, remainingDistance);
    if (stepDistance <= kDistanceEpsilon) {
        return;
    }

    RTBEngine::Math::Vector3 previousPosition = owner->GetWorldPosition();
    previousPosition.y = fixedHeight;

    RTBEngine::Math::Vector3 nextPosition = previousPosition + direction * stepDistance;
    nextPosition.y = fixedHeight;

    RTBEngine::Math::Vector3 hitPosition = nextPosition;
    const bool shouldStop = HandleSweepHit(previousPosition, nextPosition, hitPosition);
    RTBEngine::Math::Vector3 resolvedPosition = shouldStop ? hitPosition : nextPosition;
    resolvedPosition.y = fixedHeight;
    owner->GetTransform().SetPosition(resolvedPosition);

    distanceTravelled = std::min(maxDistance, distanceTravelled + stepDistance);

    if (shouldStop || distanceTravelled + kDistanceEpsilon >= maxDistance) {
        DestroyProjectile();
    }
}

void ProjectileComponent::OnValidate()
{
    ClampSettings();
}

void ProjectileComponent::OnDestroy()
{
    instigator = nullptr;
    physicsWorld = nullptr;
    hitTargets.clear();
    pendingDestroy = true;
}

void ProjectileComponent::Initialize(const ProjectileConfig& config)
{
    instigator = config.instigator;
    physicsWorld = config.physicsWorld;
    speed = config.speed;
    maxDistance = config.maxDistance;
    radius = config.radius;
    damage = config.damage;
    instigatorTeam = config.instigatorTeam;
    if (instigatorTeam == static_cast<int>(CharacterTeam::Neutral)) {
        instigatorTeam = ResolveCharacterTeam(instigator);
    }
    ignoreSameTeam = config.ignoreSameTeam;
    destroyOnHit = config.destroyOnHit;
    maxHits = config.maxHits;
    applyDamage = config.applyDamage;
    direction = config.direction;
    direction.y = 0.0f;
    if (!HasPlanarDirection(direction)) {
        direction = RTBEngine::Math::Vector3::Forward();
    } else {
        direction.Normalize();
    }

    ClampSettings();

    fixedHeight = config.origin.y;
    distanceTravelled = 0.0f;
    appliedHitCount = 0;
    pendingDestroy = false;
    initialized = true;
    hitTargets.clear();

    if (owner) {
        owner->GetTransform().SetPosition(config.origin);
    }
}

void ProjectileComponent::ClampSettings()
{
    speed = std::max(0.01f, speed);
    maxDistance = std::max(0.05f, maxDistance);
    radius = std::max(0.05f, radius);
    damage = std::max(0.0f, damage);
    maxHits = std::max(0, maxHits);
}

void ProjectileComponent::InitializeFromOwnerTransform()
{
    ProjectileConfig config;
    config.instigator = instigator;
    config.physicsWorld = physicsWorld;
    config.speed = speed;
    config.maxDistance = maxDistance;
    config.radius = radius;
    config.damage = damage;
    config.instigatorTeam = instigatorTeam;
    config.ignoreSameTeam = ignoreSameTeam;
    config.destroyOnHit = destroyOnHit;
    config.maxHits = maxHits;

    if (owner) {
        config.origin = owner->GetWorldPosition();
        config.direction = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
    }

    Initialize(config);
}

RTBEngine::Physics::PhysicsWorld* ProjectileComponent::ResolvePhysicsWorld() const
{
    if (physicsWorld) {
        return physicsWorld;
    }

    auto resolveFromObject = [](RTBEngine::ECS::GameObject* gameObject) -> RTBEngine::Physics::PhysicsWorld* {
        if (!gameObject) {
            return nullptr;
        }

        auto* rbComp = gameObject->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
        if (!rbComp || !rbComp->HasRigidBody() || !rbComp->GetRigidBody()) {
            return nullptr;
        }

        return rbComp->GetRigidBody()->GetPhysicsWorld();
    };

    if (RTBEngine::Physics::PhysicsWorld* world = resolveFromObject(owner)) {
        return world;
    }

    return resolveFromObject(instigator);
}

HealthComponent* ProjectileComponent::ResolveHitHealth(RTBEngine::ECS::GameObject* hitObject) const
{
    if (!hitObject) {
        return nullptr;
    }

    if (instigator && IsSameOrDescendant(hitObject, instigator)) {
        return nullptr;
    }

    for (RTBEngine::ECS::GameObject* current = hitObject; current; current = current->GetParent()) {
        if (instigator && current == instigator) {
            return nullptr;
        }

        HealthComponent* targetHealth = current->GetComponent<HealthComponent>();
        if (!targetHealth) {
            continue;
        }

        if (!targetHealth->IsDead()) {
            const int targetTeam = ResolveCharacterTeam(current);
            if (ignoreSameTeam &&
                instigatorTeam != static_cast<int>(CharacterTeam::Neutral) &&
                targetTeam == instigatorTeam) {
                return nullptr;
            }

            return targetHealth;
        }

        return nullptr;
    }

    return nullptr;
}

bool ProjectileComponent::HasAlreadyHit(HealthComponent* target) const
{
    return target &&
        std::find(hitTargets.begin(), hitTargets.end(), target) != hitTargets.end();
}

bool ProjectileComponent::HandleSweepHit(const RTBEngine::Math::Vector3& previousPosition,
                                         const RTBEngine::Math::Vector3& nextPosition,
                                         RTBEngine::Math::Vector3& outResolvedPosition)
{
    RTBEngine::Physics::PhysicsWorld* world = ResolvePhysicsWorld();
    if (!world) {
        return false;
    }

    RTBEngine::Physics::PhysicsQueryHit hit;
    RTBEngine::Physics::PhysicsQueryOptions queryOptions;
    queryOptions.ignoredObject = instigator;
    queryOptions.ignoreIgnoredObjectHierarchy = true;
    queryOptions.ignoreTriggers = true;

    if (!world->SphereCastClosest(previousPosition, nextPosition, radius, hit, queryOptions)) {
        return false;
    }

    const float hitFraction = std::clamp(hit.fraction, 0.0f, 1.0f);
    outResolvedPosition = previousPosition + (nextPosition - previousPosition) * hitFraction;
    outResolvedPosition.y = fixedHeight;

    if (IsOtherPlayer(hit.gameObject, instigator)) {
        return false;
    }

    if (applyDamage) {
        HealthComponent* targetHealth = ResolveHitHealth(hit.gameObject);
        if (targetHealth && !HasAlreadyHit(targetHealth)) {
            HealthComponent::DamageContext damageContext;
            damageContext.amount = damage;
            damageContext.instigator = instigator;
            damageContext.hitPoint = hit.point;
            damageContext.hitDirection = direction;
            targetHealth->TakeDamage(damage, damageContext);

            hitTargets.push_back(targetHealth);
            ++appliedHitCount;

            if (!destroyOnHit) {
                return maxHits > 0 && appliedHitCount >= maxHits;
            }
        }
    }

    return destroyOnHit;
}

void ProjectileComponent::DestroyProjectile()
{
    if (pendingDestroy) {
        return;
    }

    pendingDestroy = true;
    SetEnabled(false);

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (scene && owner) {
        scene->RemoveGameObject(owner);
    }
}
