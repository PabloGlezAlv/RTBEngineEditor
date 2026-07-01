#include "ProjectileComponent.h"

#include "CharacterBase.h"
#include "FloatingDamageNumberSpawner.h"
#include "HitFlashComponent.h"

#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include "ProjectileTrailFadeLifetime.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
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

    bool IsLocallyControlledInstigator(RTBEngine::ECS::GameObject* instigatorObject)
    {
        if (!instigatorObject) {
            return false;
        }

        const RTBEngine::ECS::NetworkIdentity* identity =
            instigatorObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        return identity->IsLocallyControlled();
    }

    void TryTriggerHitFlash(RTBEngine::ECS::GameObject* hitObject)
    {
        for (RTBEngine::ECS::GameObject* current = hitObject; current; current = current->GetParent()) {
            if (auto* hitFlash = current->GetComponent<HitFlashComponent>()) {
                hitFlash->TriggerFlash();
                return;
            }
        }
    }

    void TryTriggerDamageNumber(RTBEngine::ECS::GameObject* hitObject,
                                float amount,
                                const RTBEngine::Math::Vector3& hitPoint)
    {
        if (amount <= 0.0f) {
            return;
        }

        for (RTBEngine::ECS::GameObject* current = hitObject; current; current = current->GetParent()) {
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
    RTB_PROPERTY_ASSET_PATH(impactParticlePrefabRef, "prefab")
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
    UpdateFlightTrail(resolvedPosition);

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

    fixedHeight = context.origin.y;
    distanceTravelled = 0.0f;
    appliedHitCount = 0;
    pendingDestroy = false;
    initialized = true;
    hitTargets.clear();

    if (owner) {
        owner->GetTransform().SetPosition(context.origin);
    }

    EnsureFlightTrail();
    UpdateFlightTrail(context.origin);
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
    if (!owner || flightTrail) {
        return;
    }

    flightTrail = owner->GetComponent<RTBEngine::ECS::TrailRenderer>();
    if (!flightTrail) {
        flightTrail = new RTBEngine::ECS::TrailRenderer();
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
    if (!flightTrail) {
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
    if (!flightTrail || flightTrail->GetPointCount() < 2) {
        flightTrail = nullptr;
        return;
    }

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    const std::vector<RTBEngine::Math::Vector3> trailPoints = flightTrail->GetPoints();
    const float trailWidth = flightTrail->width;

    RTBEngine::ECS::GameObject* trailGhost =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate("Projectile Trail Fade");
    if (!trailGhost) {
        return;
    }

    trailGhost->SetTransient(true);

    auto* ghostTrail = new RTBEngine::ECS::TrailRenderer();
    ghostTrail->width = trailWidth;
    ghostTrail->color = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    ghostTrail->fadeAlphaAlongLength = true;
    ghostTrail->SetGlobalAlphaScale(1.0f);
    ghostTrail->SetPoints(trailPoints);
    ghostTrail->SetVisible(true);
    trailGhost->AddComponent(ghostTrail);

    auto* fadeLifetime = new ProjectileTrailFadeLifetime();
    trailGhost->AddComponent(fadeLifetime);

    for (const auto& component : trailGhost->GetComponents()) {
        if (component) {
            component->TryInvokeStart();
        }
    }

    flightTrail->SetVisible(false);
    flightTrail->ClearPoints();
    flightTrail = nullptr;
}

void ProjectileComponent::SpawnImpactParticles()
{
    if (impactParticlePrefabRef.empty() || !owner) {
        return;
    }

    const std::string resolvedPath =
        RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(impactParticlePrefabRef);
    const RTBEngine::ECS::Prefab* prefab =
        RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(resolvedPath);
    if (!prefab) {
        prefab = RTBEngine::ECS::PrefabRegistry::GetInstance().Get("Arrow Impact Sparks");
    }
    if (!prefab) {
        return;
    }

    RTBEngine::Math::Vector3 impactPosition = owner->GetWorldPosition();
    impactPosition.y = fixedHeight;

    RTBEngine::ECS::GameObject* impactEffect =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate(
            *prefab,
            impactPosition,
            BuildSplashRotation(direction));
    if (!impactEffect) {
        return;
    }

    impactEffect->SetTransient(true);

    for (const auto& component : impactEffect->GetComponents()) {
        if (component) {
            component->TryInvokeStart();
        }
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

    HealthComponent* targetHealth = ResolveHitHealth(hit.gameObject);
    if (targetHealth && !HasAlreadyHit(targetHealth)) {
        TryTriggerHitFlash(hit.gameObject);
        if (!applyDamage && IsLocallyControlledInstigator(instigator)) {
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

            if (IsLocallyControlledInstigator(instigator) && hitAudio) {
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

void ProjectileComponent::DestroyProjectile()
{
    if (pendingDestroy) {
        return;
    }

    pendingDestroy = true;
    SetEnabled(false);
    ReleaseTrailForFadeout();
    SpawnImpactParticles();

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (scene && owner) {
        scene->RemoveGameObject(owner);
    }
}
