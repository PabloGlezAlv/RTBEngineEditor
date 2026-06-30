#include "CharacterDefinition.h"

using ThisClass = CharacterDefinition;

RTB_REGISTER_DATA_ASSET(CharacterDefinition)
    RTB_PROPERTY(characterId)
    RTB_PROPERTY(displayName)
    RTB_PROPERTY(description)
    RTB_PROPERTY_RANGE(maxHealth, 1.0f, 500.0f)
    RTB_PROPERTY_RANGE(moveSpeed, 0.5f, 12.0f)
    RTB_PROPERTY_RANGE(sprintMultiplier, 1.0f, 3.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 90.0f, 1080.0f)
    RTB_PROPERTY_RANGE(maxShots, 1, 8)
    RTB_PROPERTY_RANGE(fullReloadDuration, 1.0f, 60.0f)
    RTB_PROPERTY_RANGE(projectileDamage, 1.0f, 100.0f)
    RTB_PROPERTY_RANGE(projectileSpeed, 4.0f, 30.0f)
    RTB_PROPERTY_ASSET_PATH(modelRef, "fbx")
    RTB_PROPERTY_ASSET_PATH(previewPrefabRef, "prefab")
RTB_END_REGISTER_DATA_ASSET(CharacterDefinition)
