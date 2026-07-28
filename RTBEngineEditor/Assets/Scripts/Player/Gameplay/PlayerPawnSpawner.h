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

    OnlinePlayerManager* onlinePlayerManager = nullptr;
    RoundManager* roundManager = nullptr;
    RTBEngine::UI::UIJoystick* attackJoystick = nullptr;
    RTBEngine::UI::UIJoystick* specialAttackJoystick = nullptr;
    RTBEngine::UI::UIImage* specialAttackReadyIcon = nullptr;

    RTB_COMPONENT(PlayerPawnSpawner)

    void OnAwake() override;

private:
    RTBEngine::Scene::GameObject* spawnedPawn = nullptr;
};
