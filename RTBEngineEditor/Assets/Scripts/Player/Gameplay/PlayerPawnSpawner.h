#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class OnlinePlayerManager;
class RoundManager;

namespace RTBEngine {
    namespace UI {
        class UIImage;
        class UIJoystick;
    }
}

class PlayerPawnSpawner : public RTBEngine::Scene::Component {
public:
    PlayerPawnSpawner() = default;
    ~PlayerPawnSpawner() override = default;

    void OnAwake() override;

    RTB_COMPONENT(PlayerPawnSpawner)

    RTB_SERIALIZE()
    OnlinePlayerManager* onlinePlayerManager = nullptr;
    RTB_SERIALIZE()
    RoundManager* roundManager = nullptr;
    RTB_SERIALIZE()
    RTBEngine::UI::UIJoystick* attackJoystick = nullptr;
    RTB_SERIALIZE()
    RTBEngine::UI::UIJoystick* specialAttackJoystick = nullptr;
    RTB_SERIALIZE()
    RTBEngine::UI::UIImage* specialAttackReadyIcon = nullptr;

    RTBEngine::Scene::GameObject* spawnedPawn = nullptr;
};
