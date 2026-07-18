#include "PartyHealthHud.h"

#include "HealthBarUI.h"
#include "HealthComponent.h"
#include "OnlineDisplayNameHelper.h"
#include "PlayerRegistry.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/UI/UIElement.h>

#include <algorithm>

using ThisClass = PartyHealthHud;

RTB_REGISTER_COMPONENT(PartyHealthHud)
    RTB_PROPERTY_GAMEOBJECT(entriesRoot)
    RTB_PROPERTY_GAMEOBJECT(entryTemplate)
    RTB_PROPERTY(refreshInterval)
RTB_END_REGISTER(PartyHealthHud)

namespace {

    template<typename T>
    T* FindComponentInChildren(RTBEngine::Scene::GameObject* root)
    {
        if (!root) {
            return nullptr;
        }

        if (T* component = root->GetComponent<T>()) {
            return component;
        }

        for (RTBEngine::Scene::GameObject* child : root->GetChildren()) {
            if (T* found = FindComponentInChildren<T>(child)) {
                return found;
            }
        }

        return nullptr;
    }

    void SetUiVisibleRecursive(RTBEngine::Scene::GameObject* root, bool visible)
    {
        if (!root) {
            return;
        }

        if (RTBEngine::UI::UIElement* uiElement = root->GetComponent<RTBEngine::UI::UIElement>()) {
            uiElement->SetVisible(visible);
        }

        for (RTBEngine::Scene::GameObject* child : root->GetChildren()) {
            SetUiVisibleRecursive(child, visible);
        }
    }

}

void PartyHealthHud::OnStart()
{
    if (entryTemplate) {
        SetUiVisibleRecursive(entryTemplate, false);
    }

    if (RTBEngine::Scene::Scene* scene =
            RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
        PlayerRegistry::GetInstance().PruneInvalidPawns(scene);
    }

    pawnSpawnedSubscription = PlayerRegistry::GetInstance().SubscribePawnSpawned(
        [this](const PawnInfo& info) {
            RefreshEntries();
            if (!info.pawn) {
                return;
            }

            const std::string ownerKey = BuildOwnerKey(
                info.pawn,
                info.pawn->GetComponent<RTBEngine::Scene::NetworkIdentity>());
            if (PartyEntry* entry = FindOrCreateEntry(ownerKey)) {
                BindEntry(*entry, info.pawn, info.pawn->GetComponent<RTBEngine::Scene::NetworkIdentity>());
                SetEntryVisible(*entry, true);
            }
        });

    pawnDestroyedSubscription = PlayerRegistry::GetInstance().SubscribePawnDestroyed(
        [this](RTBEngine::Scene::GameObject* pawn) {
            RemoveEntryForPawn(pawn);
            RefreshEntries();
        });

    RefreshEntries();
}

void PartyHealthHud::OnDestroy()
{
    pawnSpawnedSubscription.Reset();
    pawnDestroyedSubscription.Reset();
    ClearSpawnedEntries();
}

void PartyHealthHud::RefreshEntries()
{
    if (!entriesRoot || !entryTemplate) {
        return;
    }

    const std::vector<PawnInfo>& pawns = PlayerRegistry::GetInstance().GetAll();
    const bool showPartyHud =
        RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() && pawns.size() >= 2;

    if (!showPartyHud) {
        ClearSpawnedEntries();
        return;
    }

    std::unordered_map<std::string, bool> desiredKeys;
    for (const PawnInfo& pawnInfo : pawns) {
        if (!pawnInfo.pawn) {
            continue;
        }

        RTBEngine::Scene::NetworkIdentity* identity =
            pawnInfo.pawn->GetComponent<RTBEngine::Scene::NetworkIdentity>();
        const std::string ownerKey = BuildOwnerKey(pawnInfo.pawn, identity);
        desiredKeys[ownerKey] = true;

        PartyEntry* entry = FindOrCreateEntry(ownerKey);
        if (!entry) {
            continue;
        }

        BindEntry(*entry, pawnInfo.pawn, identity);
        SetEntryVisible(*entry, true);
    }

    for (auto iterator = activeEntries.begin(); iterator != activeEntries.end();) {
        if (desiredKeys.find(iterator->first) != desiredKeys.end()) {
            ++iterator;
            continue;
        }

        RTBEngine::Scene::GameObject* entryRoot = iterator->second.root;
        iterator = activeEntries.erase(iterator);

        if (!entryRoot || entryRoot == entryTemplate) {
            continue;
        }

        if (RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
            scene->RemoveGameObject(entryRoot);
        }
    }
}

void PartyHealthHud::RemoveEntryForPawn(RTBEngine::Scene::GameObject* pawn)
{
    if (!pawn) {
        return;
    }

    RTBEngine::Scene::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    const std::string ownerKey = BuildOwnerKey(pawn, identity);
    const auto iterator = activeEntries.find(ownerKey);
    if (iterator == activeEntries.end()) {
        return;
    }

    RTBEngine::Scene::GameObject* entryRoot = iterator->second.root;
    activeEntries.erase(iterator);

    if (!entryRoot || entryRoot == entryTemplate) {
        return;
    }

    if (RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
        scene->RemoveGameObject(entryRoot);
    }
}

void PartyHealthHud::ClearSpawnedEntries()
{
    if (RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
        for (auto& pair : activeEntries) {
            RTBEngine::Scene::GameObject* entryRoot = pair.second.root;
            if (!entryRoot || entryRoot == entryTemplate) {
                continue;
            }

            scene->RemoveGameObject(entryRoot);
        }
    }

    activeEntries.clear();
}

std::string PartyHealthHud::BuildOwnerKey(
    RTBEngine::Scene::GameObject* pawn,
    RTBEngine::Scene::NetworkIdentity* identity) const
{
    if (identity && !identity->networkOwnerUserId.empty()) {
        return identity->networkOwnerUserId;
    }

    if (identity && identity->networkPlayerSlot >= 0) {
        return "slot:" + std::to_string(identity->networkPlayerSlot);
    }

    return "pawn:" + std::to_string(reinterpret_cast<std::uintptr_t>(pawn));
}

PartyHealthHud::PartyEntry* PartyHealthHud::FindOrCreateEntry(const std::string& ownerKey)
{
    const auto existing = activeEntries.find(ownerKey);
    if (existing != activeEntries.end()) {
        return &existing->second;
    }

    if (!entryPrefab) {
        entryPrefab = RTBEngine::Scene::Prefab::CreateFromGameObject(entryTemplate);
        if (!entryPrefab) {
            return nullptr;
        }
    }

    RTBEngine::Scene::GameObject* spawnedEntry =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(*entryPrefab, entriesRoot);
    if (!spawnedEntry) {
        return nullptr;
    }

    PartyEntry entry;
    entry.root = spawnedEntry;
    entry.ownerKey = ownerKey;
    entry.nameLabel = FindComponentInChildren<RTBEngine::UI::UIText>(spawnedEntry);
    entry.healthBar = FindComponentInChildren<HealthBarUI>(spawnedEntry);

    auto inserted = activeEntries.emplace(ownerKey, entry);
    return inserted.second ? &inserted.first->second : &activeEntries[ownerKey];
}

void PartyHealthHud::BindEntry(
    PartyEntry& entry,
    RTBEngine::Scene::GameObject* pawn,
    RTBEngine::Scene::NetworkIdentity* identity)
{
    if (entry.nameLabel) {
        entry.nameLabel->SetText(GameNet::ResolvePlayerDisplayName(identity));
    }

    if (entry.healthBar) {
        HealthComponent* health = pawn ? pawn->GetComponent<HealthComponent>() : nullptr;
        if (entry.healthBar->health != health) {
            entry.healthBar->health = health;
            entry.healthBar->RefreshBinding();
        }
    }
}

void PartyHealthHud::SetEntryVisible(PartyEntry& entry, bool visible)
{
    if (!entry.root) {
        return;
    }

    entry.root->SetActive(visible);
    SetUiVisibleRecursive(entry.root, visible);
}
