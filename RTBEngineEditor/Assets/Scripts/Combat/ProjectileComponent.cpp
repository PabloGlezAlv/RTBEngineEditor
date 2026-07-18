#include "ProjectileComponent.h"

#include "CharacterBase.h"
#include "CombatAuthority.h"
#include "FloatingDamageNumberSpawner.h"
#include "HitFlashComponent.h"

#include <RTBEngine/ECS/ProjectileSimulation.h>
#include <RTBEngine/ECS/World.h>
#include <RTBEngine/ECS/Components/ProjectileComponents.h>
#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include "ProjectileTrailFadeLifetime.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>
#include <cmath>

using ThisClass = ProjectileComponent;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;
    constexpr float kDistanceEpsilon = 0.0001f;
    constexpr float kMinTrailPointDistance = 0.04f;
    constexpr float kMinTrailPointDistanceSq = kMinTrailPointDistance * kMinTrailPointDistance;
    constexpr std::size_t kMaxTrailPoints = 48;
    constexpr float kProjectileTrailWidth = 0.10f;
    constexpr float kPi = 3.14159265358979323846f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    RTBEngine::Math::Quaternion BuildSplashRotation(const RTBEngine::Math::Vector3& flightDirection)
    {
        RTBEngine::Math::Vector3 splashAxis = flightDirection;
        splashAxis.y = 0.0f;
        if (!HasPlanarDirection(splashAxis)) {
            return RTBEngine::Math::Quaternion::Identity();
        }

        splashAxis.Normalize();
        splashAxis = splashAxis * -1.0f;

        const RTBEngine::Math::Vector3 localUp(0.0f, 1.0f, 0.0f);
        const float dot = std::clamp(localUp.Dot(splashAxis), -1.0f, 1.0f);
        if (dot > 1.0f - 1e-5f) {
            return RTBEngine::Math::Quaternion::Identity();
        }

        if (dot < -1.0f + 1e-5f) {
            return RTBEngine::Math::Quaternion(RTBEngine::Math::Vector3(0.0f, 0.0f, 1.0f), kPi);
        }

        RTBEngine::Math::Vector3 axis = localUp.Cross(splashAxis);
        axis.Normalize();
        return RTBEngine::Math::Quaternion(axis, std::acos(dot));
    }

    bool IsSameOrDescendant(const RTBEngine::Scene::GameObject* candidate,
                            const RTBEngine::Scene::GameObject* root)
    {
        for (const RTBEngine::Scene::GameObject* current = candidate; current; current = current->GetParent()) {
            if (current == root) {
                return true;
            }
        }

        return false;
    }

    int ResolveCharacterTeam(RTBEngine::Scene::GameObject* gameObject)
    {
        if (!gameObject) {
            return static_cast<int>(CharacterTeam::Neutral);
        }

        for (RTBEngine::Scene::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam();
            }
        }

        return static_cast<int>(CharacterTeam::Neutral);
    }

    bool IsOtherPlayer(RTBEngine::Scene::GameObject* hitObject,
                       const RTBEngine::Scene::GameObject* instigator)
    {
        if (!hitObject || !instigator) {
            return false;
        }

        if (IsSameOrDescendant(hitObject, instigator)) {
            return false;
        }

        for (RTBEngine::Scene::GameObject* current = hitObject; current; current = current->GetParent()) {
            if (current == instigator) {
                return false;
            }

            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam() == static_cast<int>(CharacterTeam::Player);
            }
        }

        return false;
    }

    void TryTriggerHitFlash(RTBEngine::Scene::GameObject* hitObject)
    {
        for (RTBEngine::Scene::GameObject* current = hitObject; current; current = current->GetParent()) {
            if (auto* hitFlash = current->GetComponent<HitFlashComponent>()) {
                hitFlash->TriggerFlash();
                return;
            }
        }
    }

    void TryTriggerDamageNumber(RTBEngine::Scene::GameObject* hitObject,
                                float amount,
                                const RTBEngine::Math::Vector3& hitPoint)
    {
        if (amount <= 0.0f) {
            return;
        }

        for (RTBEngine::Scene::GameObject* current = hitObject; current; current = current->GetParent()) {
            if (auto* spawner = current->GetComponent<FloatingDamageNumberSpawner>()) {
                spawner->SpawnDamageNumber(amount, hitPoint);
                return;
            }
        }
    }
}

RTB_REGISTER_COMPONENT(ProjectileComponent)
    RTB_PROPERTY_RANGE(speed, 0.01f, 50.0f)
    RTB_PROPERTY_RANGE(lifetime, 0.01f, 30.0f)
    RTB_PROPERTY_RANGE(maxDistance, 0.05f, 50.0f)
    RTB_PROPERTY_RANGE(radius, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(damage, 0.0f, 1000.0f)
    RTB_PROPERTY_RANGE(knockbackStrength, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(instigatorTeam, 0, 8)
    RTB_PROPERTY(ignoreSameTeam)
    RTB_PROPERTY(destroyOnHit)
    RTB_PROPERTY_RANGE(maxHits, 0, 100)
    RTB_PROPERTY(enableFlightTrail)
    RTB_PROPERTY_ASSET_PATH(impactParticlePrefabRef, "prefab")
RTB_END_REGISTER(ProjectileComponent)

void ProjectileComponent::OnPoolAcquire()
{
    SetEnabled(true);
    SetUpdateTickEnabled(true);
    pendingDestroy = false;
    initialized = false;
    appliedHitCount = 0;
    distanceTravelled = 0.0f;
    hitTargets.clear();
    DestroyEcsEntity();
}

void ProjectileComponent::OnPoolRelease()
{
    SetEnabled(false);
    DestroyEcsEntity();
    instigator = nullptr;
    physicsWorld = nullptr;
    hitAudio = nullptr;
    hitTargets.clear();
    appliedHitCount = 0;
    distanceTravelled = 0.0f;
    pendingDestroy = false;
    initialized = false;

    if (flightTrail) {
        flightTrail->SetVisible(false);
        flightTrail->ClearPoints();
        flightTrail = nullptr;
    }
}

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

    if (ecsEntity.IsValid()) {
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
    if (enableFlightTrail) {
        UpdateFlightTrail(resolvedPosition);
    }

    distanceTravelled = std::min(maxDistance, distanceTravelled + stepDistance);

    if (shouldStop || distanceTravelled + kDistanceEpsilon >= maxDistance) {
        DestroyProjectile();
    }
}

void ProjectileComponent::OnLateUpdate(float deltaTime)
{
    if (!ecsEntity.IsValid() || pendingDestroy) {
        return;
    }

    SyncEcsSimulation(deltaTime);
}

void ProjectileComponent::OnValidate()
{
    ClampSettings();
}

void ProjectileComponent::OnDestroy()
{
    DestroyEcsEntity();
    if (flightTrail) {
        flightTrail->SetVisible(false);
        flightTrail->ClearPoints();
        flightTrail = nullptr;
    }

    instigator = nullptr;
    physicsWorld = nullptr;
    hitTargets.clear();
    pendingDestroy = true;
}

void ProjectileComponent::BeginFlight(const ProjectileRuntimeContext& context)
{
    instigator = context.instigator;
    hitAudio = context.hitAudio;
    physicsWorld = context.physicsWorld;
    instigatorTeam = context.instigatorTeam;
    if (instigatorTeam == static_cast<int>(CharacterTeam::Neutral)) {
        instigatorTeam = ResolveCharacterTeam(instigator);
    }
    applyDamage = context.applyDamage;

    direction = context.direction;
    direction.y = 0.0f;
    if (!HasPlanarDirection(direction)) {
        direction = RTBEngine::Math::Vector3::Forward();
    } else {
        direction.Normalize();
    }

    ClampSettings();

    SetEnabled(true);
    fixedHeight = context.origin.y;
    distanceTravelled = 0.0f;
    appliedHitCount = 0;
    pendingDestroy = false;
    initialized = true;
    hitTargets.clear();

    if (owner) {
        owner->GetTransform().SetPosition(context.origin);
    }

    if (enableFlightTrail) {
        EnsureFlightTrail();
        UpdateFlightTrail(context.origin);
    }

    CreateEcsEntity();
}

void ProjectileComponent::Initialize(const ProjectileConfig& config)
{
    speed = config.speed;
    maxDistance = config.maxDistance;
    radius = config.radius;
    damage = config.damage;
    knockbackStrength = config.knockbackStrength;
    ignoreSameTeam = config.ignoreSameTeam;
    destroyOnHit = config.destroyOnHit;
    maxHits = config.maxHits;
    if (!config.impactParticlePrefabRef.empty()) {
        impactParticlePrefabRef = config.impactParticlePrefabRef;
    }

    ProjectileRuntimeContext context;
    context.instigator = config.instigator;
    context.hitAudio = config.hitAudio;
    context.physicsWorld = config.physicsWorld;
    context.origin = config.origin;
    context.direction = config.direction;
    context.instigatorTeam = config.instigatorTeam;
    context.applyDamage = config.applyDamage;
    BeginFlight(context);
}

void ProjectileComponent::ClampSettings()
{
    speed = std::max(0.01f, speed);
    lifetime = std::max(0.01f, lifetime);
    if (lifetime > 0.0f) {
        maxDistance = std::max(0.05f, speed * lifetime);
    } else {
        maxDistance = std::max(0.05f, maxDistance);
    }
    radius = std::max(0.05f, radius);
    damage = std::max(0.0f, damage);
    knockbackStrength = std::max(0.0f, knockbackStrength);
    maxHits = std::max(0, maxHits);
}

void ProjectileComponent::EnsureFlightTrail()
{
    if (!enableFlightTrail || !owner || flightTrail) {
        return;
    }

    flightTrail = owner->GetComponent<RTBEngine::Scene::TrailRenderer>();
    if (!flightTrail) {
        flightTrail = new RTBEngine::Scene::TrailRenderer();
        owner->AddComponent(flightTrail);
    }

    flightTrail->width = kProjectileTrailWidth;
    flightTrail->color = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    flightTrail->fadeAlphaAlongLength = true;
    flightTrail->SetGlobalAlphaScale(1.0f);
    flightTrail->SetVisible(true);
    flightTrail->ClearPoints();
}

void ProjectileComponent::UpdateFlightTrail(const RTBEngine::Math::Vector3& position)
{
    if (!enableFlightTrail || !flightTrail) {
        return;
    }

    RTBEngine::Math::Vector3 trailPoint = position;
    trailPoint.y = fixedHeight;

    const std::vector<RTBEngine::Math::Vector3>& points = flightTrail->GetPoints();
    if (!points.empty()) {
        RTBEngine::Math::Vector3 delta = trailPoint - points.back();
        delta.y = 0.0f;
        if (delta.LengthSquared() <= kMinTrailPointDistanceSq) {
            return;
        }
    }

    flightTrail->AddPoint(trailPoint);

    while (flightTrail->GetPointCount() > kMaxTrailPoints) {
        const std::vector<RTBEngine::Math::Vector3>& allPoints = flightTrail->GetPoints();
        flightTrail->SetPoints(
            std::vector<RTBEngine::Math::Vector3>(allPoints.begin() + 1, allPoints.end()));
    }
}

void ProjectileComponent::ReleaseTrailForFadeout()
{
    if (!enableFlightTrail || !flightTrail || flightTrail->GetPointCount() < 2) {
        flightTrail = nullptr;
        return;
    }

    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    const std::vector<RTBEngine::Math::Vector3> trailPoints = flightTrail->GetPoints();
    const float trailWidth = flightTrail->width;

    RTBEngine::Scene::GameObject* trailGhost =
        RTBEngine::Scene::ObjectPool::GetInstance().Acquire(
            RTBEngine::Scene::ObjectPool::ResolvePoolKey("Projectile Trail Fade"),
            trailPoints.back(),
            RTBEngine::Math::Quaternion::Identity());
    if (!trailGhost) {
        return;
    }

    RTBEngine::Scene::TrailRenderer* ghostTrail = trailGhost->GetComponent<RTBEngine::Scene::TrailRenderer>();
    if (!ghostTrail) {
        ghostTrail = new RTBEngine::Scene::TrailRenderer();
        trailGhost->AddComponent(ghostTrail);
    }

    ProjectileTrailFadeLifetime* fadeLifetime = trailGhost->GetComponent<ProjectileTrailFadeLifetime>();
    if (!fadeLifetime) {
        fadeLifetime = new ProjectileTrailFadeLifetime();
        trailGhost->AddComponent(fadeLifetime);
    }

    ghostTrail->width = trailWidth;
    ghostTrail->color = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    ghostTrail->fadeAlphaAlongLength = true;
    ghostTrail->SetGlobalAlphaScale(1.0f);
    ghostTrail->SetPoints(trailPoints);
    ghostTrail->SetVisible(true);

    fadeLifetime->BeginFade();

    flightTrail->SetVisible(false);
    flightTrail->ClearPoints();
    flightTrail = nullptr;
}

void ProjectileComponent::SpawnImpactParticles()
{
    if (!owner) {
        return;
    }

    const std::string impactPoolKey = !impactParticlePrefabRef.empty()
        ? RTBEngine::Scene::ObjectPool::ResolvePoolKey(impactParticlePrefabRef)
        : RTBEngine::Scene::ObjectPool::ResolvePoolKey("Arrow Impact Sparks");
    if (impactPoolKey.empty()) {
        return;
    }

    RTBEngine::Math::Vector3 impactPosition = owner->GetWorldPosition();
    impactPosition.y = fixedHeight;

    RTBEngine::Scene::GameObject* impactEffect =
        RTBEngine::Scene::ObjectPool::GetInstance().Acquire(
            impactPoolKey,
            impactPosition,
            BuildSplashRotation(direction));
    if (!impactEffect) {
        return;
    }
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
    config.knockbackStrength = knockbackStrength;
    config.instigatorTeam = instigatorTeam;
    config.ignoreSameTeam = ignoreSameTeam;
    config.destroyOnHit = destroyOnHit;
    config.maxHits = maxHits;
    config.impactParticlePrefabRef = impactParticlePrefabRef;

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

    auto resolveFromObject = [](RTBEngine::Scene::GameObject* gameObject) -> RTBEngine::Physics::PhysicsWorld* {
        if (!gameObject) {
            return nullptr;
        }

        auto* rbComp = gameObject->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
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

HealthComponent* ProjectileComponent::ResolveHitHealth(RTBEngine::Scene::GameObject* hitObject) const
{
    if (!hitObject) {
        return nullptr;
    }

    if (instigator && IsSameOrDescendant(hitObject, instigator)) {
        return nullptr;
    }

    for (RTBEngine::Scene::GameObject* current = hitObject; current; current = current->GetParent()) {
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

    HealthComponent* targetHealth = ResolveHitHealth(hit.gameObject);
    if (targetHealth && !HasAlreadyHit(targetHealth)) {
        TryTriggerHitFlash(hit.gameObject);
        if (!applyDamage && CombatAuthority::IsLocallyControlled(instigator)) {
            TryTriggerDamageNumber(hit.gameObject, damage, hit.point);
        }
    }

    if (applyDamage) {
        if (targetHealth && !HasAlreadyHit(targetHealth)) {
            HealthComponent::DamageContext damageContext;
            damageContext.amount = damage;
            damageContext.instigator = instigator;
            damageContext.hitPoint = hit.point;
            damageContext.hitDirection = direction;
            damageContext.knockbackStrength = knockbackStrength;
            targetHealth->TakeDamage(damage, damageContext);

            if (CombatAuthority::IsLocallyControlled(instigator) && hitAudio) {
                hitAudio->PlayOneShot();
            }

            hitTargets.push_back(targetHealth);
            ++appliedHitCount;

            if (!destroyOnHit) {
                return maxHits > 0 && appliedHitCount >= maxHits;
            }
        }
    }

    return destroyOnHit;
}

void ProjectileComponent::CreateEcsEntity()
{
    DestroyEcsEntity();

    RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
    if (!world || !owner) {
        return;
    }

    RTBEngine::ECS::LocalTransform transform;
    transform.position = owner->GetWorldPosition();
    transform.fixedHeight = fixedHeight;

    RTBEngine::ECS::ProjectileFlight flight;
    flight.direction = direction;
    flight.speed = speed;
    flight.maxDistance = maxDistance;
    flight.radius = radius;
    flight.distanceTravelled = distanceTravelled;

    RTBEngine::ECS::ProjectilePhysicsContext physicsContext;
    physicsContext.physicsWorld = ResolvePhysicsWorld();
    physicsContext.instigator = instigator;

    ecsEntity = RTBEngine::ECS::CreateProjectileEntity(
        *world,
        owner,
        transform,
        flight,
        physicsContext);
}

void ProjectileComponent::DestroyEcsEntity()
{
    if (!ecsEntity.IsValid()) {
        return;
    }

    if (RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive()) {
        RTBEngine::ECS::DestroyProjectileEntity(*world, ecsEntity);
    }

    ecsEntity = RTBEngine::ECS::kNullEntity;
}

bool ProjectileComponent::ProcessEcsHit(RTBEngine::Scene::GameObject* hitObject,
                                      const RTBEngine::Math::Vector3& hitPoint)
{
    if (IsOtherPlayer(hitObject, instigator)) {
        return false;
    }

    HealthComponent* targetHealth = ResolveHitHealth(hitObject);
    if (targetHealth && !HasAlreadyHit(targetHealth)) {
        TryTriggerHitFlash(hitObject);
        if (!applyDamage && CombatAuthority::IsLocallyControlled(instigator)) {
            TryTriggerDamageNumber(hitObject, damage, hitPoint);
        }
    }

    if (applyDamage) {
        if (targetHealth && !HasAlreadyHit(targetHealth)) {
            HealthComponent::DamageContext damageContext;
            damageContext.amount = damage;
            damageContext.instigator = instigator;
            damageContext.hitPoint = hitPoint;
            damageContext.hitDirection = direction;
            damageContext.knockbackStrength = knockbackStrength;
            targetHealth->TakeDamage(damage, damageContext);

            if (CombatAuthority::IsLocallyControlled(instigator) && hitAudio) {
                hitAudio->PlayOneShot();
            }

            hitTargets.push_back(targetHealth);
            ++appliedHitCount;

            if (!destroyOnHit) {
                return maxHits > 0 && appliedHitCount >= maxHits;
            }
        }
    }

    return destroyOnHit;
}

void ProjectileComponent::SyncEcsSimulation(float /*deltaTime*/)
{
    RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
    if (!world || !world->IsAlive(ecsEntity)) {
        return;
    }

    RTBEngine::ECS::ProjectileFlight* flight = world->TryGet<RTBEngine::ECS::ProjectileFlight>(ecsEntity);
    RTBEngine::ECS::LocalTransform* transform = world->TryGet<RTBEngine::ECS::LocalTransform>(ecsEntity);
    RTBEngine::ECS::ProjectilePendingHit* pendingHit =
        world->TryGet<RTBEngine::ECS::ProjectilePendingHit>(ecsEntity);

    if (transform && enableFlightTrail) {
        UpdateFlightTrail(transform->position);
    }

    if (pendingHit && pendingHit->active) {
        const bool shouldStop = ProcessEcsHit(pendingHit->hitObject, pendingHit->hitPoint);
        pendingHit->active = false;
        if (shouldStop && flight) {
            flight->pendingDestroy = true;
        }
    }

    if (flight) {
        distanceTravelled = flight->distanceTravelled;
        if (flight->pendingDestroy) {
            DestroyEcsEntity();
            DestroyProjectile();
        }
    }
}

void ProjectileComponent::DestroyProjectile()
{
    if (pendingDestroy) {
        return;
    }

    pendingDestroy = true;
    DestroyEcsEntity();
    ReleaseTrailForFadeout();
    SpawnImpactParticles();

    if (owner) {
        RTBEngine::Scene::ObjectPool::GetInstance().Release(owner);
    }
}
