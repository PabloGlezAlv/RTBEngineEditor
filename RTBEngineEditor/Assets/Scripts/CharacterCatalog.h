#pragma once

#include <string>
#include <vector>

class CharacterDefinition;

class CharacterCatalog {
public:
    static CharacterCatalog& GetInstance();

    const std::vector<std::string>& GetCharacterAssetPaths() const { return characterAssetPaths; }

    CharacterDefinition* GetById(const std::string& characterId) const;
    CharacterDefinition* GetDefault() const;
    std::vector<std::string> GetAllCharacterIds() const;
    std::vector<CharacterDefinition*> GetAllDefinitions() const;

private:
    CharacterCatalog() = default;

    std::vector<std::string> characterAssetPaths{
        "Assets/Data/Characters/Ranger.rtbasset",
        "Assets/Data/Characters/Knight.rtbasset",
        "Assets/Data/Characters/Arcanist.rtbasset",
        "Assets/Data/Characters/Rogue.rtbasset",
        "Assets/Data/Characters/Barbarian.rtbasset",
    };
};
