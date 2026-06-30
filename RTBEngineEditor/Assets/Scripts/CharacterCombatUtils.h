#pragma once

class CharacterBase;

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

namespace CharacterCombatUtils {

    int ResolveCharacterTeam(RTBEngine::ECS::GameObject* gameObject);

}
