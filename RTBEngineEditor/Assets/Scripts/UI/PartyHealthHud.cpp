#include "PartyHealthHud.h"

#include "HealthBarUI.h"
#include "HealthComponent.h"
#include "OnlineDisplayNameHelper.h"
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
    T* FindComponentInChildren(RTBEngine::ECS::GameObject* root)
    {
        if (!root) {
            return nullptr;
        }

        if (T* component = root->GetComponent<T>()) {
            return component;
        }

        for (RTBEngine::ECS::GameObject* child : root->GetChildren()) {
            if (T* found = FindComponentInChildren<T>(child)) {
                return found;
            }
        }

        return nullptr;
    }

    void SetUiVisibleRecursive(RTBEngine::ECS::GameObject* root, bool visible)
    {
        if (!root) {
            return;
        }

        if (RTBEngine::UI::UIElement* uiElement = root->GetComponent<RTBEngine::UI::UIElement>()) {
            uiElement->SetVisible(visible);
        }

        for (RTBEngine::ECS::GameObject* child : root->GetChildren()) {
            SetUiVisibleRecursive(child, visible);
        }
    }

    struct PlayerPawnInfo {
        RTBEngine::ECS::GameObject* pawn = nullptr;
        RTBEngine::ECS::NetworkIdentity* identity = nullptr;
        HealthComponent* health = nullptr;
        int slot = -1;
    };

    std::vector<PlayerPawnInfo> CollectPlayerPawns()
    {
        std::vector<PlayerPawnInfo> pawns;
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) {
            return pawns;
        }

        for (const auto& gameObject : scene->GetGameObjects()) {
            if (!gameObject || !gameObject->IsActive()) {
                continue;
            }

            HealthComponent* health = gameObject->GetComponent<HealthComponent>();
            if (!health) {
                continue;
            }

            ThirdPersonCharacterController* controller =
                gameObject->GetComponent<ThirdPersonCharacterController>();
            RTBEngine::ECS::NetworkIdentity* identity =
                gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
            if (!controller && !identity) {
                continue;
            }

            PlayerPawnInfo info;
            info.pawn = gameObject.get();
            info.health = health;
            info.identity = identity;
            info.slot = identity ? identity->networkPlayerSlot : -1;
            pawns.push_back(info);
        }

        std::sort(pawns.begin(), pawns.end(), [](const PlayerPawnInfo& left, const PlayerPawnInfo& right) {
            if (left.slot >= 0 && right.slot >= 0) {
                return left.slot < right.slot;
            }

            if (left.slot >= 0) {
                return true;
            }

            if (right.slot >= 0) {
                return false;
            }

            return left.pawn < right.pawn;
        });

        return pawns;
    }

}

void PartyHealthHud::OnStart()
{
    refreshTimer = 0.0f;
    if (entryTemplate) {
        SetUiVisibleRecursive(entryTemplate, false);
    }

    RefreshEntries();
}

void PartyHealthHud::OnUpdate(float deltaTime)
{
    refreshTimer += deltaTime;
    if (refreshTimer < refreshInterval) {
        return;
    }

    refreshTimer = 0.0f;
    RefreshEntries();
}

void PartyHealthHud::OnDestroy()
{
    ClearSpawnedEntries();
}

void PartyHealthHud::RefreshEntries()
{
    if (!entriesRoot || !entryTemplate) {
        return;
    }

    const std::vector<PlayerPawnInfo> pawns = CollectPlayerPawns();
    const bool showPartyHud =
        RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() && pawns.size() >= 2;

    if (!showPartyHud) {
        ClearSpawnedEntries();
        return;
    }

    std::unordered_map<std::string, bool> desiredKeys;
    for (const PlayerPawnInfo& pawnInfo : pawns) {
        const std::string ownerKey = BuildOwnerKey(pawnInfo.pawn, pawnInfo.identity);
        desiredKeys[ownerKey] = true;

        PartyEntry* entry = FindOrCreateEntry(ownerKey);
        if (!entry) {
            continue;
        }

        BindEntry(*entry, pawnInfo.pawn, pawnInfo.identity);
        SetEntryVisible(*entry, true);
    }

    for (auto iterator = activeEntries.begin(); iterator != activeEntries.end();) {
        if (desiredKeys.find(iterator->first) != desiredKeys.end()) {
            ++iterator;
            continue;
        }

        RTBEngine::ECS::GameObject* entryRoot = iterator->second.root;
        iterator = activeEntries.erase(iterator);

        if (!entryRoot || entryRoot == entryTemplate) {
            continue;
        }

        if (RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene()) {
            scene->RemoveGameObject(entryRoot);
        }
    }
}

void PartyHealthHud::ClearSpawnedEntries()
{
    if (RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene()) {
        for (auto& pair : activeEntries) {
            RTBEngine::ECS::GameObject* entryRoot = pair.second.root;
            if (!entryRoot || entryRoot == entryTemplate) {
                continue;
            }

            scene->RemoveGameObject(entryRoot);
        }
    }

    activeEntries.clear();
}

std::string PartyHealthHud::BuildOwnerKey(
    RTBEngine::ECS::GameObject* pawn,
    RTBEngine::ECS::NetworkIdentity* identity) const
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
        entryPrefab = RTBEngine::ECS::Prefab::CreateFromGameObject(entryTemplate);
        if (!entryPrefab) {
            return nullptr;
        }
    }

    RTBEngine::ECS::GameObject* spawnedEntry =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate(*entryPrefab, entriesRoot);
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
    RTBEngine::ECS::GameObject* pawn,
    RTBEngine::ECS::NetworkIdentity* identity)
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
