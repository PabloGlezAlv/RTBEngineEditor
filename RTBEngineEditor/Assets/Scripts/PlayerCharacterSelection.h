#pragma once

#include <string>

class CharacterDefinition;
class CharacterCatalog;

class PlayerCharacterSelection {
public:
    static PlayerCharacterSelection& GetInstance();

    void SetSelectedCharacterId(const std::string& characterId);
    const std::string& GetSelectedCharacterId() const { return selectedCharacterId; }

    CharacterDefinition* GetSelectedDefinition() const;

private:
    PlayerCharacterSelection() = default;

    std::string selectedCharacterId = "ranger";
};
