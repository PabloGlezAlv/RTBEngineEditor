#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/GameObject.h>

#include <cstddef>
#include <unordered_map>
#include <vector>

class HealthComponent;

struct PawnInfo {
    RTBEngine::ECS::GameObject* pawn = nullptr;
    int team = 0;
    int networkPlayerSlot = -1;
    HealthComponent* health = nullptr;
};

class PlayerRegistry {
public:
    static PlayerRegistry& GetInstance();

    void Clear();
    void Register(const PawnInfo& info);
    void RegisterPlayerPawn(RTBEngine::ECS::GameObject* pawn);
    void Unregister(RTBEngine::ECS::GameObject* pawn);

    RTBEngine::ECS::GameObject* FindBySlot(int slot) const;
    int FindSlotByOwnerUserId(const std::string& ownerUserId) const;
    const std::vector<PawnInfo>& GetAll() const { return pawns; }

    using PawnSpawnedCallback = RTBEngine::Core::Event<PawnInfo>::Callback;
    using PawnDestroyedCallback = RTBEngine::Core::Event<RTBEngine::ECS::GameObject*>::Callback;

    RTBEngine::Core::EventSubscription SubscribePawnSpawned(PawnSpawnedCallback callback);
    RTBEngine::Core::EventSubscription SubscribePawnDestroyed(PawnDestroyedCallback callback);

private:
    void RebuildLookupTables();
    void UnregisterAtIndex(std::size_t index, bool notify);

    std::vector<PawnInfo> pawns;
    std::unordered_map<int, std::size_t> slotToIndex;
    std::unordered_map<RTBEngine::ECS::GameObject*, std::size_t> pawnToIndex;

    RTBEngine::Core::Event<PawnInfo> pawnSpawnedEvent;
    RTBEngine::Core::Event<RTBEngine::ECS::GameObject*> pawnDestroyedEvent;
};
