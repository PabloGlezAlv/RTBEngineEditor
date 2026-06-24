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

CharacterDefinition* PlayerCharacterSelection::GetSelectedDefinition() const
{
    if (CharacterDefinition* selected = CharacterCatalog::GetInstance().GetById(selectedCharacterId)) {
        return selected;
    }

    return CharacterCatalog::GetInstance().GetDefault();
}
