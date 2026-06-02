#pragma once

#include <RTBEngine/ECS/NetworkIdentity.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/Elements/UIText.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class HealthBarUI;
class HealthComponent;

class PartyHealthHud : public RTBEngine::ECS::Component
{
public:
    PartyHealthHud() = default;
    ~PartyHealthHud() override = default;

    RTBEngine::ECS::GameObject* entriesRoot = nullptr;
    RTBEngine::ECS::GameObject* entryTemplate = nullptr;
    float refreshInterval = 0.35f;

    RTB_COMPONENT(PartyHealthHud)

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

private:
    struct PartyEntry {
        RTBEngine::ECS::GameObject* root = nullptr;
        RTBEngine::UI::UIText* nameLabel = nullptr;
        HealthBarUI* healthBar = nullptr;
        std::string ownerKey;
    };

    std::unique_ptr<RTBEngine::ECS::Prefab> entryPrefab;
    std::unordered_map<std::string, PartyEntry> activeEntries;
    float refreshTimer = 0.0f;

    void RefreshEntries();
    void ClearSpawnedEntries();
    std::string BuildOwnerKey(RTBEngine::ECS::GameObject* pawn, RTBEngine::ECS::NetworkIdentity* identity) const;
    PartyEntry* FindOrCreateEntry(const std::string& ownerKey);
    void BindEntry(PartyEntry& entry, RTBEngine::ECS::GameObject* pawn, RTBEngine::ECS::NetworkIdentity* identity);
    void SetEntryVisible(PartyEntry& entry, bool visible);
};
