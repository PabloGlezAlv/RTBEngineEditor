#include "CharacterStatsApplier.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "HealthComponent.h"
#include "PlayerAmmoSystem.h"
#include "PlayerCharacterSelection.h"
#include "PlayerMeleeSweepAttackAbility.h"
#include "ProjectileAttackAbility.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/GameObject.h>

using ThisClass = CharacterStatsApplier;

RTB_REGISTER_COMPONENT(CharacterStatsApplier)
    RTB_PROPERTY(usePlayerSelection)
    RTB_PROPERTY_DATA_ASSET(characterDefinitionRef)
    RTB_PROPERTY(fallbackCharacterId)
RTB_END_REGISTER(CharacterStatsApplier)

void CharacterStatsApplier::OnAwake()
{
    RTBEngine::ECS::GameObject* pawn = GetOwner();
    if (!pawn) {
        return;
    }

    CharacterDefinition* definition = ResolveDefinition();
    if (!definition) {
        RTB_WARN("CharacterStatsApplier: No character definition found for pawn '" + pawn->GetName() + "'.");
        return;
    }

    if (!ApplyDefinition(pawn, *definition)) {
        RTB_WARN("CharacterStatsApplier: Failed to apply stats for '" + definition->characterId + "'.");
    }
}

CharacterDefinition* CharacterStatsApplier::ResolveDefinition() const
{
    if (!characterDefinitionRef.empty()) {
        RTBEngine::Data::DataAsset* asset =
            RTBEngine::Core::ResourceManager::GetInstance().LoadDataAsset(characterDefinitionRef);
        if (auto* definition = dynamic_cast<CharacterDefinition*>(asset)) {
            return definition;
        }
    }

    if (usePlayerSelection) {
        PlayerCharacterSelection& selection = PlayerCharacterSelection::GetInstance();
        selection.EnsureSelectionFromCatalog();
        if (CharacterDefinition* selected = selection.GetSelectedDefinition()) {
            return selected;
        }
    }

    if (!fallbackCharacterId.empty()) {
        return CharacterCatalog::GetInstance().GetById(fallbackCharacterId);
    }

    return CharacterCatalog::GetInstance().GetDefault();
}

bool CharacterStatsApplier::ApplyDefinition(
    RTBEngine::ECS::GameObject* pawn,
    const CharacterDefinition& definition)
{
    if (!pawn) {
        return false;
    }

    bool appliedAny = false;

    auto* controller = pawn->GetComponent<ThirdPersonCharacterController>();
    if (controller) {
        controller->moveSpeed = definition.moveSpeed;
        controller->sprintMultiplier = definition.sprintMultiplier;
        controller->turnSpeed = definition.turnSpeed;
        controller->ApplyCombatAnimationOverrides(
            definition.aimDrawAnimationFbx,
            definition.aimLoopAnimationFbx,
            definition.attackAnimationFbx);
        appliedAny = true;
    }

    if (auto* health = pawn->GetComponent<HealthComponent>()) {
        health->SetMaxHealth(definition.maxHealth);
        health->SetCurrentHealth(definition.maxHealth);
        appliedAny = true;
    }

    if (auto* ammo = pawn->GetComponent<PlayerAmmoSystem>()) {
        ammo->maxShots = definition.maxShots;
        ammo->fullReloadDuration = definition.fullReloadDuration;
        appliedAny = true;
    }

    if (auto* meleeAttack = pawn->GetComponent<PlayerMeleeSweepAttackAbility>()) {
        meleeAttack->SetMeleeCombatOverrides(
            definition.projectileDamage,
            definition.meleeRange,
            definition.meleeRadius,
            definition.meleeTickInterval,
            definition.meleeKnockback);
        appliedAny = true;
    }
    else if (auto* rangedAttack = pawn->GetComponent<ProjectileAttackAbility>()) {
        if (!definition.projectilePrefabRef.empty()) {
            rangedAttack->SetProjectilePrefabRef(definition.projectilePrefabRef);
        }
        rangedAttack->SetProjectileCombatOverrides(definition.projectileDamage, definition.projectileSpeed);
        appliedAny = true;
    }

    return appliedAny;
}
