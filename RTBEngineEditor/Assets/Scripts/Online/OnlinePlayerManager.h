#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Online/OnlineUser.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include "OnlineGameNetMessages.h"

#include <memory>
#include <vector>

class RoundManager;

class OnlinePlayerManager : public RTBEngine::ECS::Component {
public:
    OnlinePlayerManager() = default;
    ~OnlinePlayerManager() override = default;

    RTBEngine::ECS::GameObject* localPlayerObject = nullptr;
    RoundManager* roundManager = nullptr;
    float remoteSpawnOffsetX = 2.5f;

    RTB_COMPONENT(OnlinePlayerManager)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;
    void OnFixedUpdate(float fixedDeltaTime) override;

    void RemovePawnFromTracking(RTBEngine::ECS::GameObject* pawn, int playerSlot);
    void MergeAuthoritativeSessionProfile(const GameNet::PlayerSessionSnapshot& snapshot);

private:
    std::unique_ptr<RTBEngine::ECS::Prefab> playerPrefab;
    std::vector<RTBEngine::ECS::GameObject*> spawnedRemotePawns;
    std::vector<GameNet::PlayerNetworkBindSnapshot> authoritativePlayerBinds;
    std::vector<GameNet::PlayerSessionSnapshot> authoritativePlayerSessionProfiles;

    void RegisterPlayerSessionProfiles(
        const std::vector<RTBEngine::Online::OnlineUserId>& members);
    void SendLocalPlayerSessionProfile();

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
