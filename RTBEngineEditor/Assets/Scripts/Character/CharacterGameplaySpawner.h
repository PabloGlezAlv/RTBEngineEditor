#pragma once

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Scene/GameObject.h>

#include <string>

class CharacterDefinition;

class CharacterGameplaySpawner {
public:
    static std::string SanitizeCharacterId(const std::string& characterId);

    static RTBEngine::ECS::GameObject* InstantiateFromDefinition(
        const CharacterDefinition& definition,
        const RTBEngine::Math::Vector3& position,
        const RTBEngine::Math::Quaternion& rotation);
    static RTBEngine::ECS::GameObject* InstantiateFromCharacterId(
        const std::string& characterId,
        const RTBEngine::Math::Vector3& position,
        const RTBEngine::Math::Quaternion& rotation);
};
