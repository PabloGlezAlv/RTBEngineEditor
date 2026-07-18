#include "PlayerRegistry.h"

#include <RTBEngine/Scene/Scene.h>

#include "CharacterBase.h"
#include "HealthComponent.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Scene/NetworkIdentity.h>

#include <algorithm>

PlayerRegistry& PlayerRegistry::GetInstance()
{
    static PlayerRegistry instance;
    return instance;
}

void PlayerRegistry::Clear()
{
    pawns.clear();
    slotToIndex.clear();
    pawnToIndex.clear();
    pawnSpawnedEvent.Clear();
    pawnDestroyedEvent.Clear();
}

void PlayerRegistry::PruneInvalidPawns(RTBEngine::ECS::Scene* scene)
{
    for (std::size_t index = pawns.size(); index-- > 0;) {
        RTBEngine::ECS::GameObject* pawn = pawns[index].pawn;
        if (!pawn) {
            UnregisterAtIndex(index, false);
            continue;
        }

        if (!scene) {
            continue;
        }

        RTBEngine::ECS::GameObject* found = scene->FindGameObjectByUUID(pawn->GetUUID());
        if (found != pawn) {
            UnregisterAtIndex(index, false);
        }
    }
}

void PlayerRegistry::RebuildLookupTables()
{
    slotToIndex.clear();
    pawnToIndex.clear();

    for (std::size_t index = 0; index < pawns.size(); ++index) {
        const PawnInfo& info = pawns[index];
        if (info.pawn) {
            pawnToIndex[info.pawn] = index;
        }

        if (info.networkPlayerSlot >= 0) {
            slotToIndex[info.networkPlayerSlot] = index;
        }
    }
}

void PlayerRegistry::UnregisterAtIndex(std::size_t index, bool notify)
{
    if (index >= pawns.size()) {
        return;
    }

    RTBEngine::ECS::GameObject* removedPawn = pawns[index].pawn;
    pawns.erase(pawns.begin() + static_cast<std::ptrdiff_t>(index));
    RebuildLookupTables();

    if (notify && removedPawn) {
        pawnDestroyedEvent.Invoke(removedPawn);
    }
}

void PlayerRegistry::Register(const PawnInfo& info)
{
    if (!info.pawn) {
        return;
    }

    if (const auto existingPawn = pawnToIndex.find(info.pawn); existingPawn != pawnToIndex.end()) {
        UnregisterAtIndex(existingPawn->second, false);
    }

    if (info.networkPlayerSlot >= 0) {
        if (const auto existingSlot = slotToIndex.find(info.networkPlayerSlot);
            existingSlot != slotToIndex.end()) {
            UnregisterAtIndex(existingSlot->second, false);
        }
    }

    pawns.push_back(info);
    RebuildLookupTables();
    pawnSpawnedEvent.Invoke(pawns.back());
}

void PlayerRegistry::RegisterPlayerPawn(RTBEngine::ECS::GameObject* pawn)
{
    if (!pawn) {
        return;
    }

    ThirdPersonCharacterController* controller = pawn->GetComponent<ThirdPersonCharacterController>();
    if (!controller || controller->team != static_cast<int>(CharacterTeam::Player)) {
        return;
    }

    HealthComponent* health = pawn->GetComponent<HealthComponent>();
    if (!health) {
        health = pawn->GetComponentInChildren<HealthComponent>();
    }

    RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();

    PawnInfo info;
    info.pawn = pawn;
    info.team = controller->team;
    info.networkPlayerSlot = identity ? identity->networkPlayerSlot : -1;
    info.health = health;
    Register(info);
}

void PlayerRegistry::Unregister(RTBEngine::ECS::GameObject* pawn)
{
    if (!pawn) {
        return;
    }

    const auto iterator = pawnToIndex.find(pawn);
    if (iterator == pawnToIndex.end()) {
        return;
    }

    UnregisterAtIndex(iterator->second, true);
}

RTBEngine::ECS::GameObject* PlayerRegistry::FindBySlot(int slot) const
{
    if (slot < 0) {
        return nullptr;
    }

    const auto iterator = slotToIndex.find(slot);
    if (iterator == slotToIndex.end() || iterator->second >= pawns.size()) {
        return nullptr;
    }

    return pawns[iterator->second].pawn;
}

int PlayerRegistry::FindSlotByOwnerUserId(const std::string& ownerUserId) const
{
    if (ownerUserId.empty()) {
        return -1;
    }

    for (const PawnInfo& info : pawns) {
        if (!info.pawn) {
            continue;
        }

        if (const RTBEngine::ECS::NetworkIdentity* identity =
                info.pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>()) {
            if (identity->networkOwnerUserId == ownerUserId) {
                return identity->networkPlayerSlot;
            }
        }
    }

    return -1;
}

RTBEngine::Core::EventSubscription PlayerRegistry::SubscribePawnSpawned(PawnSpawnedCallback callback)
{
    return pawnSpawnedEvent.Subscribe(std::move(callback));
}

RTBEngine::Core::EventSubscription PlayerRegistry::SubscribePawnDestroyed(PawnDestroyedCallback callback)
{
    return pawnDestroyedEvent.Subscribe(std::move(callback));
}
