#include "CharacterCombatUtils.h"

#include "CharacterBase.h"

#include <RTBEngine/Scene/GameObject.h>

namespace CharacterCombatUtils {

    int ResolveCharacterTeam(RTBEngine::ECS::GameObject* gameObject)
    {
        if (!gameObject) {
            return static_cast<int>(CharacterTeam::Neutral);
        }

        for (RTBEngine::ECS::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam();
            }
        }

        return static_cast<int>(CharacterTeam::Neutral);
    }

}
