#include "ProjectileAttackAbility.h"

#include "CharacterBase.h"
#include "ProjectileComponent.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/ECS/CapsuleColliderComponent.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/SphereColliderComponent.h>
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
RTB_END_REGISTER(ProjectileAttackAbility)

void ProjectileAttackAbility::OnValidate()
{
    ClampSettings();
}

bool ProjectileAttackAbility::FireNow(RTBEngine::ECS::GameObject* instigator,
                                      const RTBEngine::Math::Vector3& attackDirection,
                                      RTBEngine::Physics::PhysicsWorld* physicsWorld)
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

    const RTBEngine::Math::Vector3 spawnPosition = GetLaunchOrigin(instigator, planarDirection);
    const float projectileDiameter = projectileRadius * 2.0f;
    const float projectileYaw = -std::atan2(planarDirection.x, planarDirection.z) * kRadToDeg;
    const RTBEngine::Math::Quaternion projectileRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, projectileYaw * kDegToRad, 0.0f);

    projectileObject->GetTransform().SetPosition(spawnPosition);
    projectileObject->GetTransform().SetRotation(projectileRotation);
    projectileObject->GetTransform().SetScale(
        RTBEngine::Math::Vector3(projectileDiameter, projectileDiameter, projectileDiameter));

    auto* renderer = new RTBEngine::ECS::MeshRenderer();
    projectileObject->AddComponent(renderer);

    RTBEngine::Core::ResourceManager& resources = RTBEngine::Core::ResourceManager::GetInstance();
    renderer->SetMesh(resources.GetDefaultSphere());
    renderer->SetShader(resources.GetShader("basic"));

    auto* sphereCollider = new RTBEngine::ECS::SphereColliderComponent();
    sphereCollider->SetRadius(projectileRadius);
    sphereCollider->SetCenterOffset(RTBEngine::Math::Vector3::Zero());
    sphereCollider->SetIsTrigger(true);
    projectileObject->AddComponent(sphereCollider);

    auto* projectile = new ProjectileComponent();
    projectileObject->AddComponent(projectile);

    ProjectileComponent::ProjectileConfig config;
    config.instigator = instigator;
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
    projectile->Initialize(config);
    return true;
}

RTBEngine::Math::Vector3 ProjectileAttackAbility::GetLaunchOrigin(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& attackDirection) const
{
    if (!instigator) {
        return RTBEngine::Math::Vector3::Zero();
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
    const float minimumCenterHeight = instigator->GetWorldPosition().y + projectileRadius;
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
    FireNow(GetActiveInstigator(), GetActiveDirection());
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
