#include "PlayerCharacterSelection.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"

PlayerCharacterSelection& PlayerCharacterSelection::GetInstance()
{
    static PlayerCharacterSelection instance;
    return instance;
}

void PlayerCharacterSelection::SetSelectedCharacterId(const std::string& characterId)
{
    if (characterId.empty()) {
        return;
    }

    selectedCharacterId = characterId;
}

void PlayerCharacterSelection::EnsureSelectionFromCatalog()
{
    CharacterCatalog& catalog = CharacterCatalog::GetInstance();
    if (!selectedCharacterId.empty() && catalog.GetById(selectedCharacterId)) {
        return;
    }

    if (CharacterDefinition* defaultDefinition = catalog.GetDefault()) {
        selectedCharacterId = defaultDefinition->characterId;
    }
}

CharacterDefinition* PlayerCharacterSelection::GetSelectedDefinition() const
{
    CharacterCatalog& catalog = CharacterCatalog::GetInstance();
    if (!selectedCharacterId.empty()) {
        if (CharacterDefinition* selected = catalog.GetById(selectedCharacterId)) {
            return selected;
        }
    }

    return catalog.GetDefault();
}
