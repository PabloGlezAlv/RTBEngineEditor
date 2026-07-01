#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class CharacterDefinition;

class CharacterStatsApplier : public RTBEngine::ECS::Component {
public:
    bool usePlayerSelection = true;
    std::string characterDefinitionRef;
    std::string fallbackCharacterId;

    void OnAwake() override;

    static bool ApplyDefinition(RTBEngine::ECS::GameObject* pawn, const CharacterDefinition& definition);

    RTB_COMPONENT(CharacterStatsApplier)

private:
    CharacterDefinition* ResolveDefinition() const;
};
