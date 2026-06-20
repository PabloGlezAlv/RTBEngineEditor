#include "ProjectileAttackAbility.h"

#include "CharacterBase.h"
#include "ProjectileComponent.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/FbxBinding.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include "OnlineGameNetMessages.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/CapsuleColliderComponent.h>
#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/MeshRenderer.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/SphereColliderComponent.h>
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

    bool IsLocallyControlledInstigator(RTBEngine::ECS::GameObject* instigator)
    {
        if (!instigator) {
            return false;
        }

        const RTBEngine::ECS::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        return identity->IsLocallyControlled();
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
}

RTB_REGISTER_COMPONENT(ProjectileAttackAbility)
    RTB_PROPERTY(attackOriginOffset)
    RTB_PROPERTY_GAMEOBJECT(launchOriginObject)
    RTB_PROPERTY_RANGE(launchForwardOffset, -2.0f, 2.0f)
    RTB_PROPERTY(projectileModel)
    RTB_PROPERTY(projectileTexture)
    RTB_PROPERTY_RANGE(cooldown, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(damage, 0.0f, 1000.0f)
    RTB_PROPERTY_RANGE(hitDelay, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(recoveryDuration, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(projectileSpeed, 0.01f, 100.0f)
    RTB_PROPERTY_RANGE(projectileLifetime, 0.01f, 30.0f)
    RTB_PROPERTY_RANGE(projectileRadius, 0.05f, 5.0f)
    RTB_PROPERTY(destroyOnHit)
    RTB_PROPERTY_RANGE(maxHits, 0, 100)
    RTB_PROPERTY(ignoreSameTeam)
    RTB_PROPERTY_COMPONENT(fireAudio, AudioSourceComponent)
    RTB_PROPERTY_COMPONENT(hitAudio, AudioSourceComponent)
RTB_END_REGISTER(ProjectileAttackAbility)

void ProjectileAttackAbility::OnValidate()
{
    ClampSettings();
}

bool ProjectileAttackAbility::FireNow(RTBEngine::ECS::GameObject* instigator,
                                      const RTBEngine::Math::Vector3& attackDirection,
                                      RTBEngine::Physics::PhysicsWorld* physicsWorld)
{
    return SpawnProjectile(instigator, attackDirection, physicsWorld, true);
}

bool ProjectileAttackAbility::SpawnFromNetworkSnapshot(
    const GameNet::ProjectileSpawnSnapshot& snapshot)
{
    RTBEngine::ECS::GameObject* instigator = FindPawnByPlayerSlot(snapshot.ownerPlayerSlot);
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

    ability->damage = snapshot.damage;
    ability->projectileSpeed = snapshot.speed;
    ability->projectileLifetime = std::max(0.01f, snapshot.maxDistance / std::max(0.01f, snapshot.speed));
    ability->projectileRadius = snapshot.radius;
    ability->ignoreSameTeam = snapshot.ignoreSameTeam;
    ability->destroyOnHit = snapshot.destroyOnHit;
    ability->maxHits = snapshot.maxHits;

    RTBEngine::Math::Vector3 origin = snapshot.origin;
    return ability->SpawnProjectile(
        instigator,
        direction,
        ability->ResolvePhysicsWorld(instigator),
        false,
        &origin);
}

bool ProjectileAttackAbility::SpawnProjectile(RTBEngine::ECS::GameObject* instigator,
                                              const RTBEngine::Math::Vector3& attackDirection,
                                              RTBEngine::Physics::PhysicsWorld* physicsWorld,
                                              bool broadcastOnlineSpawn,
                                              const RTBEngine::Math::Vector3* spawnOriginOverride)
{
    ClampSettings();

    if (!instigator || damage <= 0.0f) {
        return false;
    }

    RTBEngine::Math::Vector3 planarDirection = attackDirection;
    planarDirection.y = 0.0f;
    if (!HasPlanarDirection(planarDirection)) {
        return false;
    }
    planarDirection.Normalize();

    RTBEngine::ECS::GameObject* projectileObject =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate("Projectile Attack");
    if (!projectileObject) {
        return false;
    }

    projectileObject->SetTransient(true);
    projectileObject->SetCollisionLayerByName("Projectiles");

    const RTBEngine::Math::Vector3 spawnPosition = spawnOriginOverride
        ? *spawnOriginOverride
        : GetLaunchOrigin(instigator, planarDirection);
    const float projectileDiameter = projectileRadius * 2.0f;
    const float projectileYaw = -std::atan2(planarDirection.x, planarDirection.z) * kRadToDeg;
    const RTBEngine::Math::Quaternion projectileRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, projectileYaw * kDegToRad, 0.0f);

    projectileObject->GetTransform().SetPosition(spawnPosition);
    projectileObject->GetTransform().SetRotation(projectileRotation);

    auto* renderer = new RTBEngine::ECS::MeshRenderer();
    projectileObject->AddComponent(renderer);

    RTBEngine::Core::ResourceManager& resources = RTBEngine::Core::ResourceManager::GetInstance();
    RTBEngine::Rendering::Shader* basicShader = resources.GetShader("basic");
    if (basicShader) {
        renderer->SetShader(basicShader);
    }

    if (!projectileModel.empty()) {
        RTBEngine::Rendering::ModelData modelData =
            RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(projectileModel);
        if (!modelData.meshes.empty()) {
            resources.RegisterMeshes(projectileModel, modelData.meshes);

            RTBEngine::Rendering::FbxBindingContext ctx{ resources, projectileModel, modelData };
            RTBEngine::Rendering::FbxBindingResult binding =
                RTBEngine::Rendering::BuildMeshesAndMaterials(ctx);

            renderer->SetMesh(modelData.meshes[0]);
            if (!binding.meshMaterials.empty() && binding.meshMaterials[0]) {
                if (basicShader) {
                    binding.meshMaterials[0]->SetShader(basicShader);
                }
                renderer->SetMaterial(binding.meshMaterials[0]);
            }

            if (!projectileTexture.empty()) {
                RTBEngine::Rendering::Texture* tex = resources.LoadModelTexture(projectileTexture);
                if (tex) {
                    renderer->SetTexture(tex);
                }
            }

            projectileObject->GetTransform().SetScale(RTBEngine::Math::Vector3::One());
        }
        else {
            projectileObject->GetTransform().SetScale(
                RTBEngine::Math::Vector3(projectileDiameter, projectileDiameter, projectileDiameter));
            renderer->SetMesh(resources.GetDefaultSphere());
        }
    }
    else {
        projectileObject->GetTransform().SetScale(
            RTBEngine::Math::Vector3(projectileDiameter, projectileDiameter, projectileDiameter));
        renderer->SetMesh(resources.GetDefaultSphere());
    }

    auto* sphereCollider = new RTBEngine::ECS::SphereColliderComponent();
    sphereCollider->SetRadius(projectileRadius);
    sphereCollider->SetCenterOffset(RTBEngine::Math::Vector3::Zero());
    sphereCollider->SetIsTrigger(true);
    projectileObject->AddComponent(sphereCollider);

    auto* projectile = new ProjectileComponent();
    projectileObject->AddComponent(projectile);

    ProjectileComponent::ProjectileConfig config;
    config.instigator = instigator;
    config.hitAudio = hitAudio;
    config.instigatorTeam = ResolveCharacterTeam(instigator);
    config.ignoreSameTeam = ignoreSameTeam;
    config.physicsWorld = physicsWorld ? physicsWorld : ResolvePhysicsWorld(instigator);
    config.origin = spawnPosition;
    config.direction = planarDirection;
    config.speed = projectileSpeed;
    config.maxDistance = GetTravelDistance();
    config.radius = projectileRadius;
    config.damage = damage;
    config.destroyOnHit = destroyOnHit;
    config.maxHits = maxHits;
    config.applyDamage = !RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost();
    projectile->Initialize(config);

    if (IsLocallyControlledInstigator(instigator) && fireAudio) {
        fireAudio->PlayOneShot();
    }

    if (broadcastOnlineSpawn &&
        RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        RTBEngine::ECS::NetworkIdentity* identity = instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (identity && identity->networkPlayerSlot >= 0) {
            static std::uint32_t nextProjectileSpawnId = 1;

            GameNet::ProjectileSpawnSnapshot snapshot;
            snapshot.spawnId = nextProjectileSpawnId++;
            snapshot.ownerPlayerSlot = identity->networkPlayerSlot;
            snapshot.origin = spawnPosition;
            snapshot.direction = planarDirection;
            snapshot.speed = projectileSpeed;
            snapshot.maxDistance = config.maxDistance;
            snapshot.radius = projectileRadius;
            snapshot.damage = damage;
            snapshot.instigatorTeam = config.instigatorTeam;
            snapshot.ignoreSameTeam = ignoreSameTeam;
            snapshot.destroyOnHit = destroyOnHit;
            snapshot.maxHits = maxHits;
            GameNet::OnlineGameNetSubsystem::BroadcastProjectileSpawn(snapshot);
        }
    }

    return true;
}

RTBEngine::ECS::GameObject* ProjectileAttackAbility::FindPawnByPlayerSlot(int playerSlot)
{
    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene || playerSlot < 0) {
        return nullptr;
    }

    for (const auto& gameObject : scene->GetGameObjects()) {
        if (!gameObject) {
            continue;
        }

        RTBEngine::ECS::NetworkIdentity* identity = gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (identity && identity->networkPlayerSlot == playerSlot) {
            return gameObject.get();
        }
    }

    return nullptr;
}

RTBEngine::Math::Vector3 ProjectileAttackAbility::GetLaunchOrigin(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& attackDirection) const
{
    if (!instigator) {
        return RTBEngine::Math::Vector3::Zero();
    }

    const float minimumCenterHeight = instigator->GetWorldPosition().y + projectileRadius;

    if (launchOriginObject) {
        const RTBEngine::Math::Vector3 launchForward =
            launchOriginObject->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
        RTBEngine::Math::Vector3 origin =
            launchOriginObject->GetWorldPosition() + launchForward * launchForwardOffset;
        origin.y = std::max(origin.y, minimumCenterHeight);
        return origin;
    }

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

    RTBEngine::Math::Vector3 origin =
        instigator->GetWorldPosition() + (instigator->GetWorldRotation() * attackOriginOffset);
    origin.y = std::max(origin.y, minimumCenterHeight);
    return origin + planarDirection * GetLaunchClearance(instigator);
}

float ProjectileAttackAbility::GetTravelDistance() const
{
    return std::max(0.05f, projectileSpeed * projectileLifetime);
}

float ProjectileAttackAbility::GetLaunchClearance(RTBEngine::ECS::GameObject* instigator) const
{
    if (!instigator) {
        return projectileRadius;
    }

    float ownerRadius = 0.0f;
    if (auto* capsule = instigator->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>()) {
        ownerRadius = std::max(ownerRadius, capsule->GetRadius());
    }
    if (auto* sphere = instigator->GetComponent<RTBEngine::ECS::SphereColliderComponent>()) {
        ownerRadius = std::max(ownerRadius, sphere->GetRadius());
    }

    constexpr float kProjectileSpawnPadding = 0.05f;
    return ownerRadius + projectileRadius + kProjectileSpawnPadding;
}

bool ProjectileAttackAbility::CanActivateAbility(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& direction) const
{
    return instigator && damage > 0.0f && HasPlanarDirection(direction);
}

void ProjectileAttackAbility::ExecuteAbilityHit()
{
    RTBEngine::ECS::GameObject* instigator = GetActiveInstigator();
    if (!instigator) {
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        const RTBEngine::ECS::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (identity && !identity->IsSimulatedByHost()) {
            return;
        }
    }

    FireNow(instigator, GetActiveDirection());
}

void ProjectileAttackAbility::ClampSettings()
{
    cooldown = std::max(0.0f, cooldown);
    damage = std::max(0.0f, damage);
    hitDelay = std::max(0.0f, hitDelay);
    recoveryDuration = std::max(0.0f, recoveryDuration);
    projectileSpeed = std::max(0.01f, projectileSpeed);
    projectileLifetime = std::max(0.01f, projectileLifetime);
    projectileRadius = std::max(0.05f, projectileRadius);
    maxHits = std::max(0, maxHits);
    launchForwardOffset = std::clamp(launchForwardOffset, -2.0f, 2.0f);
}

RTBEngine::Physics::PhysicsWorld* ProjectileAttackAbility::ResolvePhysicsWorld(
    RTBEngine::ECS::GameObject* instigator) const
{
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
