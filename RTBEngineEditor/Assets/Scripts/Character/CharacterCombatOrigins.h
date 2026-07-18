#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

namespace CharacterCombatOrigins {

    RTBEngine::Math::Vector3 GetCapsuleCenterWorld(RTBEngine::Scene::GameObject* object);
    RTBEngine::Math::Vector3 GetFeetWorld(RTBEngine::Scene::GameObject* object);

    RTBEngine::Math::Vector3 ApplyPlanarDirectionOffset(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& attackDirection,
        float forwardOffset);

}
