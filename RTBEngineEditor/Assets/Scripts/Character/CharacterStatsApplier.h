#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class CharacterDefinition;

class CharacterStatsApplier : public RTBEngine::Scene::Component {
public:
    bool usePlayerSelection = true;
    std::string characterDefinitionRef;
    std::string fallbackCharacterId;

    void OnAwake() override;

    static bool ApplyDefinition(RTBEngine::Scene::GameObject* pawn, const CharacterDefinition& definition);

    RTB_COMPONENT(CharacterStatsApplier)

private:
    CharacterDefinition* ResolveDefinition() const;
};
