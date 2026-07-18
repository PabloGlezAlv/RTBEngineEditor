#include "CharacterGameplaySpawner.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/SceneManager.h>

#include <filesystem>

namespace {

std::string GetPrefabNameFromAssetPath(const std::string& assetPath)
{
    const std::filesystem::path filePath(assetPath);
    const std::string stem = filePath.stem().string();
    return stem.empty() ? assetPath : stem;
}

RTBEngine::ECS::Prefab* ResolveGameplayPrefab(const CharacterDefinition& definition)
{
    if (definition.gameplayPrefabRef.empty()) {
        RTB_WARN("[CharacterGameplaySpawner] Character '" + definition.characterId +
                 "' has no gameplayPrefabRef assigned.");
        return nullptr;
    }

    const std::string resolvedPath =
        RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(
            definition.gameplayPrefabRef);
    RTBEngine::ECS::Prefab* prefab =
        RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(resolvedPath);
    if (!prefab) {
        prefab = RTBEngine::ECS::PrefabRegistry::GetInstance().Get(
            GetPrefabNameFromAssetPath(definition.gameplayPrefabRef));
    }
    if (!prefab) {
        RTB_WARN("[CharacterGameplaySpawner] Gameplay prefab not found: '" +
                 definition.gameplayPrefabRef + "'.");
    }

    return prefab;
}

} // namespace

std::string CharacterGameplaySpawner::SanitizeCharacterId(const std::string& characterId)
{
    if (characterId.empty()) {
        return {};
    }

    if (CharacterCatalog::GetInstance().GetById(characterId)) {
        return characterId;
    }

    RTB_WARN("[CharacterGameplaySpawner] Unknown character id '" + characterId + "'.");
    return {};
}

RTBEngine::ECS::GameObject* CharacterGameplaySpawner::InstantiateFromDefinition(
    const CharacterDefinition& definition,
    const RTBEngine::Math::Vector3& position,
    const RTBEngine::Math::Quaternion& rotation)
{
    RTBEngine::ECS::Prefab* prefab = ResolveGameplayPrefab(definition);
    if (!prefab) {
        return nullptr;
    }

    return RTBEngine::ECS::SceneManager::GetInstance().Instantiate(
        *prefab,
        position,
        rotation,
        nullptr,
        true);
}

RTBEngine::ECS::GameObject* CharacterGameplaySpawner::InstantiateFromCharacterId(
    const std::string& characterId,
    const RTBEngine::Math::Vector3& position,
    const RTBEngine::Math::Quaternion& rotation)
{
    const std::string resolvedId = SanitizeCharacterId(characterId);
    CharacterDefinition* definition = CharacterCatalog::GetInstance().GetById(resolvedId);
    if (!definition) {
        RTB_WARN("[CharacterGameplaySpawner] Unknown character id '" + resolvedId + "'.");
        return nullptr;
    }

    return InstantiateFromDefinition(*definition, position, rotation);
}
