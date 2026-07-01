#pragma once

#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class CharacterDefinition : public RTBEngine::Data::DataAsset {
public:
    std::string characterId;
    std::string displayName;
    std::string description;

    float maxHealth = 100.0f;

    float moveSpeed = 4.0f;
    float sprintMultiplier = 1.75f;
    float turnSpeed = 720.0f;

    int maxShots = 3;
    float fullReloadDuration = 10.0f;

    float projectileDamage = 25.0f;
    float projectileSpeed = 14.0f;
    float projectileBurstInterval = 0.0f;
    int projectileBurstCount = 1;

    std::string modelRef;
    std::string previewPrefabRef;
    std::string gameplayPrefabRef;

    // Ranged-only: projectile prefab (Arrow or Sphere).
    std::string projectilePrefabRef;

    // Melee-only: sweep hitbox and tick cadence (projectileDamage = total attack damage).
    float meleeRange = 0.0f;
    float meleeRadius = 0.0f;
    float meleeTickInterval = 0.0f;
    int meleeTickCount = 1;
    float meleeKnockback = 0.0f;
    float projectileKnockback = 0.0f;

    // Combat animation clips (fbx path|clip name), applied to ThirdPersonCharacterController.
    std::string aimDrawAnimationFbx;
    std::string aimLoopAnimationFbx;
    std::string attackAnimationFbx;

    RTB_DATA_ASSET(CharacterDefinition)
};
