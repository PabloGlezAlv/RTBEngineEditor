#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/Online/OnlineUser.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include "OnlineGameNetMessages.h"

#include <memory>
#include <vector>

class OnlinePlayerManager : public RTBEngine::ECS::Component {
public:
    OnlinePlayerManager() = default;
    ~OnlinePlayerManager() override = default;

    RTBEngine::ECS::GameObject* localPlayerObject = nullptr;
    float remoteSpawnOffsetX = 2.5f;

    RTB_COMPONENT(OnlinePlayerManager)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;
    void OnFixedUpdate(float fixedDeltaTime) override;

    void RemovePawnFromTracking(RTBEngine::ECS::GameObject* pawn, int playerSlot);

private:
    std::unique_ptr<RTBEngine::ECS::Prefab> playerPrefab;
    std::vector<RTBEngine::ECS::GameObject*> spawnedRemotePawns;
    std::vector<GameNet::PlayerNetworkBindSnapshot> authoritativePlayerBinds;
    std::vector<GameNet::PlayerSessionSnapshot> authoritativePlayerSessionProfiles;
    float bindRebroadcastTimer = 0.0f;
    float bindRebroadcastElapsed = 0.0f;

    void RegisterPlayerSessionProfiles(
        const std::vector<RTBEngine::Online::OnlineUserId>& members);

    void ConfigureOnlinePlayers();
    void ConfigurePawn(
        RTBEngine::ECS::GameObject* pawn,
        const RTBEngine::Online::OnlineUserId& ownerUserId,
        int playerSlot);
    RTBEngine::ECS::GameObject* SpawnRemotePawn(
        const RTBEngine::Online::OnlineUserId& ownerUserId,
        int playerSlot,
        float spawnOffsetX);
};
