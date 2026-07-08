#include "CharacterStatsApplier.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "ICharacterStatReceiver.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/GameObject.h>

#include "PlayerCharacterSelection.h"

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
    const std::size_t componentCount = pawn->GetComponentCount();
    for (std::size_t index = 0; index < componentCount; ++index) {
        RTBEngine::ECS::Component* component = pawn->GetComponentAt(index);
        if (!component) {
            continue;
        }

        if (auto* receiver = dynamic_cast<ICharacterStatReceiver*>(component)) {
            receiver->ApplyCharacterStats(definition);
            appliedAny = true;
        }
    }

    return appliedAny;
}
