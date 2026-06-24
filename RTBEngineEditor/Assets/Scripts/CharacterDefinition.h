#pragma once

#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class CharacterDefinition : public RTBEngine::Data::DataAsset {
public:
    std::string characterId = "ranger";
    std::string displayName = "Ranger";
    std::string description;

    float maxHealth = 100.0f;

    float moveSpeed = 4.0f;
    float sprintMultiplier = 1.75f;
    float turnSpeed = 720.0f;

    int maxShots = 3;
    float fullReloadDuration = 10.0f;

    float projectileDamage = 25.0f;
    float projectileSpeed = 14.0f;

    std::string modelRef =
        "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx";

    RTB_DATA_ASSET(CharacterDefinition)
};
