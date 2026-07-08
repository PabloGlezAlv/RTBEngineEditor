#pragma once

class CharacterDefinition;

class ICharacterStatReceiver {
public:
    virtual ~ICharacterStatReceiver() = default;
    virtual void ApplyCharacterStats(const CharacterDefinition& definition) = 0;
};
