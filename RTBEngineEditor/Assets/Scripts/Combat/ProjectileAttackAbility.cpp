#include "ProjectileAttackAbility.h"

#include "CharacterDefinition.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "PlayerMeleeSweepAttackAbility.h"
#include "ThirdPersonCharacterController.h"
#include "CharacterBase.h"
#include "CharacterCombatOrigins.h"
#include "PlayerAmmoSystem.h"
#include "PlayerRegistry.h"
#include "ProjectileComponent.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include "OnlineGameNetMessages.h"

#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>

#include <algorithm>
#include <cmath>

using ThisClass = ProjectileAttackAbility;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;
    constexpr float kDegToRad = kPi / 180.0f;
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    int ResolveCharacterTeam(RTBEngine::Scene::GameObject* gameObject)
    {
        return CharacterCombatUtils::ResolveCharacterTeam(gameObject);
    }
}

RTB_REGISTER_COMPONENT(ProjectileAttackAbility)
    RTB_PROPERTY_ASSET_PATH(projectilePrefabRef, "prefab")
    RTB_PROPERTY_RANGE(attackOriginHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_RANGE(launchForwardOffset, -2.0f, 2.0f)
    RTB_PROPERTY_RANGE(hitDelay, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(recoveryDuration, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(tickInterval, 0.0f, 2.0f)
    RTB_PROPERTY_RANGE(tickCount, 1, 8)
    RTB_PROPERTY_COMPONENT(fireAudio, AudioSourceComponent)
    RTB_PROPERTY_COMPONENT(hitAudio, AudioSourceComponent)
RTB_END_REGISTER(ProjectileAttackAbility)

void ProjectileAttackAbility::OnStart()
{
    ResolveProjectilePrefab();
    if (!projectilePoolKey.empty()) {
        RTBEngine::Scene::ObjectPool::GetInstance().SetMaxPoolSize(projectilePoolKey, 48);
    }
    RefreshCachedProjectileStats();
}

void ProjectileAttackAbility::ApplyCharacterStats(const CharacterDefinition& definition)
{
    RTBEngine::Scene::GameObject* owner = GetOwner();
    if (owner && owner->GetComponent<PlayerMeleeSweepAttackAbility>()) {
        return;
    }

    if (!definition.projectilePrefabRef.empty()) {
        SetProjectilePrefabRef(definition.projectilePrefabRef);
    }

    SetProjectileCombatOverrides(
        definition.projectileDamage,
        definition.projectileSpeed,
        definition.projectileKnockback,
        definition.projectileBurstCount,
        definition.projectileBurstInterval);
}

void ProjectileAttackAbility::SetProjectileCombatOverrides(
    float damage,
    float speed,
    float knockback,
    int burstCount,
    float burstInterval)
{
    totalAttackDamage = std::max(0.0f, damage);
    burstTickCount = std::max(1, burstCount);
    float resolvedBurstInterval = burstInterval;
    if (burstTickCount <= 1 && tickCount > 1) {
        burstTickCount = tickCount;
        if (resolvedBurstInterval <= 0.0f) {
            resolvedBurstInterval = tickInterval;
        }
    }
    tickCount = burstTickCount;

    projectileSpeedOverride = std::max(0.0f, speed);
    projectileKnockbackOverride = std::max(0.0f, knockback);

    if (burstTickCount <= 1 || resolvedBurstInterval <= 0.0f) {
        damagePerProjectile = totalAttackDamage;
        projectileDamageOverride = totalAttackDamage;
        tickInterval = 0.0f;
        recoveryDuration = 0.0f;
    } else {
        damagePerProjectile = totalAttackDamage / static_cast<float>(burstTickCount);
        projectileDamageOverride = damagePerProjectile;
        tickInterval = std::max(0.1f, resolvedBurstInterval);
        recoveryDuration = static_cast<float>(burstTickCount - 1) * tickInterval;
    }

    RefreshCachedProjectileStats();
}

int ProjectileAttackAbility::GetTickCount() const
{
    if (tickInterval <= 0.0f) {
        return 0;
    }

    const int ticks = burstTickCount > 1 ? burstTickCount : tickCount;
    return ticks > 1 ? ticks : 0;
}

float ProjectileAttackAbility::GetDamagePerProjectile() const
{
    if (damagePerProjectile > 0.0f) {
        return damagePerProjectile;
    }

    if (totalAttackDamage > 0.0f && GetTickCount() > 1) {
        return totalAttackDamage / static_cast<float>(GetTickCount());
    }

    return totalAttackDamage > 0.0f ? totalAttackDamage : cachedDamage;
}

void ProjectileAttackAbility::SetProjectilePrefabRef(const std::string& prefabRef)
{
    if (prefabRef.empty()) {
        return;
    }

    projectilePrefabRef = prefabRef;
    ResolveProjectilePrefab();
    RefreshCachedProjectileStats();
}

void ProjectileAttackAbility::OnValidate()
{
    ClampSettings();
    ResolveProjectilePrefab();
    RefreshCachedProjectileStats();
}

bool ProjectileAttackAbility::FireNow(RTBEngine::Scene::GameObject* instigator,
                                      const RTBEngine::Math::Vector3& attackDirection,
                                      RTBEngine::Physics::PhysicsWorld* physicsWorld)
{
    const bool spawned = SpawnProjectile(instigator, attackDirection, physicsWorld, true);
    if (spawned) {
        PlayerAmmoSystem::TryConsumeAttackAmmo(instigator);
    }

    return spawned;
}

bool ProjectileAttackAbility::SpawnFromNetworkSnapshot(
    const GameNet::ProjectileSpawnSnapshot& snapshot)
{
    RTBEngine::Scene::GameObject* instigator = FindPawnByPlayerSlot(snapshot.ownerPlayerSlot);
    if (!instigator) {
        return false;
    }

    RTBEngine::Math::Vector3 direction = snapshot.direction;
    direction.y = 0.0f;
    if (!HasPlanarDirection(direction)) {
        return false;
    }

    ProjectileAttackAbility* ability = instigator->GetComponent<ProjectileAttackAbility>();
    if (!ability) {
        return false;
    }

    RTBEngine::Math::Vector3 origin = snapshot.origin;
    const bool spawned = ability->SpawnProjectile(
        instigator,
        direction,
        ability->ResolvePhysicsWorld(instigator),
        false,
        &origin,
        &snapshot);

    if (spawned) {
        if (ThirdPersonCharacterController* controller =
                instigator->GetComponent<ThirdPersonCharacterController>()) {
            controller->PlayReplicatedAttackVisual(direction);
        }
    }

    return spawned;
}

bool ProjectileAttackAbility::SpawnProjectile(RTBEngine::Scene::GameObject* instigator,
                                              const RTBEngine::Math::Vector3& attackDirection,
                                              RTBEngine::Physics::PhysicsWorld* physicsWorld,
                                              bool broadcastOnlineSpawn,
                                              const RTBEngine::Math::Vector3* spawnOriginOverride,
                                              const GameNet::ProjectileSpawnSnapshot* networkSnapshot)
{
    ClampSettings();
    ResolveProjectilePrefab();

    if (!instigator || !projectileSpawnPrefab || cachedDamage <= 0.0f) {
        return false;
    }

    RTBEngine::Math::Vector3 planarDirection = attackDirection;
    planarDirection.y = 0.0f;
    if (!HasPlanarDirection(planarDirection)) {
        return false;
    }
    planarDirection.Normalize();

    const RTBEngine::Math::Vector3 spawnPosition = spawnOriginOverride
        ? *spawnOriginOverride
        : GetLaunchOrigin(instigator, planarDirection);
    const float projectileYaw = -std::atan2(planarDirection.x, planarDirection.z) * kRadToDeg;
    const RTBEngine::Math::Quaternion projectileRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, projectileYaw * kDegToRad, 0.0f);

    RTBEngine::Scene::GameObject* projectileObject =
        RTBEngine::Scene::ObjectPool::GetInstance().Acquire(
            projectilePoolKey,
            spawnPosition,
            projectileRotation);
    if (!projectileObject) {
        return false;
    }

    ProjectileComponent* projectile = projectileObject->GetComponent<ProjectileComponent>();
    if (!projectile) {
        RTBEngine::Scene::ObjectPool::GetInstance().Release(projectileObject);
        return false;
    }

    const bool applyDamage = CombatAuthority::ShouldProjectileApplyDamage();

    if (projectileSpeedOverride > 0.0f) {
        projectile->speed = projectileSpeedOverride;
    }

    const float spawnDamage = networkSnapshot
        ? networkSnapshot->damage
        : GetDamagePerProjectile();
    if (spawnDamage > 0.0f) {
        projectile->damage = spawnDamage;
    }
    projectile->knockbackStrength = projectileKnockbackOverride;

    if (networkSnapshot) {
        ProjectileComponent::ProjectileConfig config;
        config.instigator = instigator;
        config.hitAudio = hitAudio;
        config.instigatorTeam = networkSnapshot->instigatorTeam;
        config.physicsWorld = physicsWorld ? physicsWorld : ResolvePhysicsWorld(instigator);
        config.origin = spawnPosition;
        config.direction = planarDirection;
        config.speed = networkSnapshot->speed;
        config.maxDistance = networkSnapshot->maxDistance;
        config.radius = networkSnapshot->radius;
        config.damage = networkSnapshot->damage;
        config.ignoreSameTeam = networkSnapshot->ignoreSameTeam;
        config.destroyOnHit = networkSnapshot->destroyOnHit;
        config.maxHits = networkSnapshot->maxHits;
        config.applyDamage = applyDamage;
        config.impactParticlePrefabRef = projectile->impactParticlePrefabRef;
        config.trailFadePrefabRef = projectile->trailFadePrefabRef;
        projectile->Initialize(config);
    } else {
        ProjectileComponent::ProjectileRuntimeContext context;
        context.instigator = instigator;
        context.hitAudio = hitAudio;
        context.instigatorTeam = ResolveCharacterTeam(instigator);
        context.physicsWorld = physicsWorld ? physicsWorld : ResolvePhysicsWorld(instigator);
        context.origin = spawnPosition;
        context.direction = planarDirection;
        context.applyDamage = applyDamage;
        projectile->BeginFlight(context);
    }

    if (CombatAuthority::IsLocallyControlled(instigator) && fireAudio) {
        fireAudio->PlayOneShot();
    }

    if (broadcastOnlineSpawn && CombatAuthority::ShouldBroadcastSpawn(instigator)) {
        RTBEngine::Scene::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::Scene::NetworkIdentity>();
        static std::uint32_t nextProjectileSpawnId = 1;

        GameNet::ProjectileSpawnSnapshot snapshot;
        snapshot.spawnId = nextProjectileSpawnId++;
        snapshot.ownerPlayerSlot = identity->networkPlayerSlot;
        snapshot.origin = spawnPosition;
        snapshot.direction = planarDirection;
        snapshot.speed = projectile->speed;
        snapshot.maxDistance = projectile->GetTravelDistance();
        snapshot.radius = projectile->radius;
        snapshot.damage = projectile->damage;
        snapshot.instigatorTeam = ResolveCharacterTeam(instigator);
        snapshot.ignoreSameTeam = projectile->ignoreSameTeam;
        snapshot.destroyOnHit = projectile->destroyOnHit;
        snapshot.maxHits = projectile->maxHits;
        GameNet::OnlineGameNetSubsystem::BroadcastProjectileSpawn(snapshot);
    }

    return true;
}

RTBEngine::Scene::GameObject* ProjectileAttackAbility::FindPawnByPlayerSlot(int playerSlot)
{
    return PlayerRegistry::GetInstance().FindBySlot(playerSlot);
}

RTBEngine::Math::Vector3 ProjectileAttackAbility::GetLaunchOrigin(
    RTBEngine::Scene::GameObject* instigator,
    const RTBEngine::Math::Vector3& attackDirection) const
{
    if (!instigator) {
        return RTBEngine::Math::Vector3::Zero();
    }

    const float minimumCenterHeight = instigator->GetWorldPosition().y + cachedProjectileRadius;

    RTBEngine::Math::Vector3 planarDirection = attackDirection;
    planarDirection.y = 0.0f;
    if (!HasPlanarDirection(planarDirection)) {
        planarDirection = instigator->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
        planarDirection.y = 0.0f;
    }
    if (!HasPlanarDirection(planarDirection)) {
        planarDirection = RTBEngine::Math::Vector3::Forward();
    } else {
        planarDirection.Normalize();
    }

    const RTBEngine::Math::Quaternion rotation = instigator->GetWorldRotation();
    const RTBEngine::Math::Vector3 heightOffset =
        rotation * RTBEngine::Math::Vector3(0.0f, attackOriginHeightOffset, 0.0f);

    RTBEngine::Math::Vector3 origin =
        CharacterCombatOrigins::GetCapsuleCenterWorld(instigator) + heightOffset;
    origin = CharacterCombatOrigins::ApplyPlanarDirectionOffset(
        origin,
        planarDirection,
        launchForwardOffset);
    origin.y = std::max(origin.y, minimumCenterHeight);
    return origin;
}

float ProjectileAttackAbility::GetTravelDistance() const
{
    return std::max(0.05f, cachedTravelDistance);
}

bool ProjectileAttackAbility::CanActivateAbility(
    RTBEngine::Scene::GameObject* instigator,
    const RTBEngine::Math::Vector3& direction) const
{
    if (!instigator || !HasValidProjectilePrefab() || !HasPlanarDirection(direction)) {
        return false;
    }

    if (!PlayerAmmoSystem::HasAmmoAvailable(instigator)) {
        return false;
    }

    return true;
}

void ProjectileAttackAbility::OnAbilityStarted()
{
    RTBEngine::Scene::GameObject* instigator = GetActiveInstigator();
    if (!instigator) {
        return;
    }

    PlayerAmmoSystem::TryConsumeAttackAmmo(instigator);
}

void ProjectileAttackAbility::ExecuteAbilityHit()
{
    RTBEngine::Scene::GameObject* instigator = GetActiveInstigator();
    if (!instigator) {
        return;
    }

    if (!CombatAuthority::CanApplyDamage(instigator)) {
        return;
    }

    SpawnProjectile(
        instigator,
        GetActiveDirection(),
        ResolvePhysicsWorld(instigator),
        true);
}

void ProjectileAttackAbility::ClampSettings()
{
    launchForwardOffset = std::clamp(launchForwardOffset, -2.0f, 2.0f);
    hitDelay = std::max(0.0f, hitDelay);
    recoveryDuration = std::max(0.0f, recoveryDuration);
    tickInterval = std::max(0.0f, tickInterval);
    tickCount = std::max(1, tickCount);
}

void ProjectileAttackAbility::ResolveProjectilePrefab()
{
    projectileSpawnPrefab = nullptr;
    projectilePoolKey.clear();

    if (projectilePrefabRef.empty()) {
        return;
    }

    projectilePoolKey =
        RTBEngine::Scene::ObjectPool::ResolvePoolKey(projectilePrefabRef);
    projectileSpawnPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(projectilePoolKey);
    if (!projectileSpawnPrefab) {
        projectileSpawnPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().Get(projectilePoolKey);
    }
}

void ProjectileAttackAbility::RefreshCachedProjectileStats()
{
    cachedTravelDistance = 0.0f;
    cachedProjectileRadius = 0.05f;
    cachedDamage = 0.0f;

    if (!projectileSpawnPrefab) {
        return;
    }

    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    RTBEngine::Scene::GameObject* templateObject =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(*projectileSpawnPrefab);
    if (!templateObject) {
        return;
    }

    if (auto* projectile = templateObject->GetComponent<ProjectileComponent>()) {
        cachedTravelDistance = projectile->GetTravelDistance();
        cachedProjectileRadius = projectile->radius;
        cachedDamage = projectile->damage;
    }

    scene->RemoveGameObject(templateObject);

    if (projectileDamageOverride > 0.0f) {
        cachedDamage = projectileDamageOverride;
    } else if (totalAttackDamage > 0.0f) {
        cachedDamage = totalAttackDamage;
    }
}

RTBEngine::Physics::PhysicsWorld* ProjectileAttackAbility::ResolvePhysicsWorld(
    RTBEngine::Scene::GameObject* instigator) const
{
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
