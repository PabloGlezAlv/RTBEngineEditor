#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Online/OnlineUser.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Core/Event.h>

#include "OnlineGameNetMessages.h"

#include <string>
#include <unordered_map>
#include <vector>

class RoundManager;

class OnlinePlayerManager : public RTBEngine::Scene::Component {
public:
    OnlinePlayerManager() = default;
    ~OnlinePlayerManager() override = default;

    RTBEngine::Scene::GameObject* localPlayerObject = nullptr;
    RoundManager* roundManager = nullptr;
    float remoteSpawnOffsetX = 2.5f;

    RTB_COMPONENT(OnlinePlayerManager)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;
    void OnFixedUpdate(float fixedDeltaTime) override;

    void RemovePawnFromTracking(RTBEngine::Scene::GameObject* pawn, int playerSlot);
    void SyncAuthoritativeRemotePlayers();
    void RequestRemotePawnSync();

private:
    std::vector<RTBEngine::Scene::GameObject*> spawnedRemotePawns;
    std::vector<GameNet::PlayerNetworkBindSnapshot> authoritativePlayerBinds;
    std::unordered_map<int, std::string> spawnedCharacterIdsBySlot;
    RTBEngine::Core::EventSubscription sessionProfileSubscription;

    void RegisterPlayerSessionProfiles(
        const std::vector<RTBEngine::Online::OnlineUserId>& members);
    void SendLocalPlayerSessionProfile();

    void ConfigureOnlinePlayers();
    void EnsureRemotePawnsSpawned();
    void DespawnRemotePawnForSlot(int playerSlot);

    std::string ResolveCharacterIdForSlot(
        int playerSlot,
        const RTBEngine::Online::OnlineUserId& ownerUserId) const;

    void ConfigurePawn(
        RTBEngine::Scene::GameObject* pawn,
        const RTBEngine::Online::OnlineUserId& ownerUserId,
        int playerSlot);
    RTBEngine::Scene::GameObject* SpawnRemotePawn(
        const RTBEngine::Online::OnlineUserId& ownerUserId,
        int playerSlot,
        const std::string& characterId,
        float spawnOffsetX);
};
