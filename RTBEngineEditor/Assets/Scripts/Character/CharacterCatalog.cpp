#include "CharacterCatalog.h"
#include "CharacterDefinition.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>

CharacterCatalog& CharacterCatalog::GetInstance()
{
    static CharacterCatalog instance;
    return instance;
}

CharacterDefinition* CharacterCatalog::GetById(const std::string& characterId) const
{
    if (characterId.empty()) {
        return nullptr;
    }

    auto& resourceManager = RTBEngine::Core::ResourceManager::GetInstance();
    for (const std::string& assetPath : characterAssetPaths) {
        RTBEngine::Data::DataAsset* asset = resourceManager.LoadDataAsset(assetPath);
        auto* definition = dynamic_cast<CharacterDefinition*>(asset);
        if (definition && definition->characterId == characterId) {
            return definition;
        }
    }

    RTB_WARN("CharacterCatalog: Unknown character id '" + characterId + "'.");
    return nullptr;
}

CharacterDefinition* CharacterCatalog::GetDefault() const
{
    const std::vector<CharacterDefinition*> definitions = GetAllDefinitions();
    return definitions.empty() ? nullptr : definitions.front();
}

std::vector<std::string> CharacterCatalog::GetAllCharacterIds() const
{
    std::vector<std::string> ids;
    ids.reserve(characterAssetPaths.size());

    auto& resourceManager = RTBEngine::Core::ResourceManager::GetInstance();
    for (const std::string& assetPath : characterAssetPaths) {
        RTBEngine::Data::DataAsset* asset = resourceManager.LoadDataAsset(assetPath);
        auto* definition = dynamic_cast<CharacterDefinition*>(asset);
        if (definition && !definition->characterId.empty()) {
            ids.push_back(definition->characterId);
        }
    }

    return ids;
}

std::vector<CharacterDefinition*> CharacterCatalog::GetAllDefinitions() const
{
    std::vector<CharacterDefinition*> definitions;
    definitions.reserve(characterAssetPaths.size());

    auto& resourceManager = RTBEngine::Core::ResourceManager::GetInstance();
    for (const std::string& assetPath : characterAssetPaths) {
        RTBEngine::Data::DataAsset* asset = resourceManager.LoadDataAsset(assetPath);
        if (auto* definition = dynamic_cast<CharacterDefinition*>(asset)) {
            definitions.push_back(definition);
        }
    }

    return definitions;
}
