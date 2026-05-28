#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/Online/OnlineUser.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

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

private:
    std::unique_ptr<RTBEngine::ECS::Prefab> playerPrefab;
    std::vector<RTBEngine::ECS::GameObject*> spawnedRemotePawns;

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
