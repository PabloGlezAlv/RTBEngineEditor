#pragma once

#include "IPlayerSpecialAttack.h"

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

IPlayerSpecialAttack* ResolvePlayerSpecialAttack(RTBEngine::Scene::GameObject* owner);
