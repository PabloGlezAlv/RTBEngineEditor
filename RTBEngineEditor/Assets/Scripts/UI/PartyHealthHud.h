#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/Elements/UIText.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class HealthBarUI;
class HealthComponent;

class PartyHealthHud : public RTBEngine::Scene::Component
{
public:
    PartyHealthHud() = default;
    ~PartyHealthHud() override = default;

    RTBEngine::Scene::GameObject* entriesRoot = nullptr;
    RTBEngine::Scene::GameObject* entryTemplate = nullptr;
    float refreshInterval = 0.35f;

    RTB_COMPONENT(PartyHealthHud)

    void OnStart() override;
    void OnDestroy() override;

private:
    struct PartyEntry {
        RTBEngine::Scene::GameObject* root = nullptr;
        RTBEngine::UI::UIText* nameLabel = nullptr;
        HealthBarUI* healthBar = nullptr;
        std::string ownerKey;
    };

    std::unique_ptr<RTBEngine::Scene::Prefab> entryPrefab;
    std::unordered_map<std::string, PartyEntry> activeEntries;

    RTBEngine::Core::EventSubscription pawnSpawnedSubscription;
    RTBEngine::Core::EventSubscription pawnDestroyedSubscription;

    void RefreshEntries();
    void ClearSpawnedEntries();
    std::string BuildOwnerKey(RTBEngine::Scene::GameObject* pawn, RTBEngine::Scene::NetworkIdentity* identity) const;
    PartyEntry* FindOrCreateEntry(const std::string& ownerKey);
    void BindEntry(PartyEntry& entry, RTBEngine::Scene::GameObject* pawn, RTBEngine::Scene::NetworkIdentity* identity);
    void SetEntryVisible(PartyEntry& entry, bool visible);
    void RemoveEntryForPawn(RTBEngine::Scene::GameObject* pawn);
};
