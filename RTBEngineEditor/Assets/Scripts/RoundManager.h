#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
        class Scene;
    }
}

class HealthComponent;
class RoundUIHandler;

class RoundManager : public RTBEngine::ECS::Component
{
public:
    RoundManager() = default;
    ~RoundManager() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    RTBEngine::ECS::GameObject* enemyTemplate = nullptr;
    RTBEngine::ECS::GameObject* playerObject = nullptr;
    RoundUIHandler* uiHandler = nullptr;
    float roundCountdownDuration = 5.0f;
    int baseEnemiesPerRound = 2;
    int additionalEnemiesPerRound = 1;

    RTB_COMPONENT(RoundManager)

private:
    enum class State {
        Countdown,
        RoundActive,
        Stopped
    };

    std::unique_ptr<RTBEngine::ECS::Prefab> enemyPrefab;
    std::vector<RTBEngine::ECS::GameObject*> spawnPoints;
    std::vector<RTBEngine::ECS::GameObject*> spawnedEnemies;
    HealthComponent* playerHealth = nullptr;
    State state = State::Stopped;
    int currentRound = 0;
    int nextRound = 1;
    float countdownRemaining = 0.0f;
    int displayedCountdownSeconds = -1;
    uint32_t cachedHierarchyVersion = 0;

    void ClampSettings();
    void ResolveDependencies();
    void RefreshSpawnPoints();
    void CreateEnemyPrefabFromTemplate();
    void UpdateCountdown(float deltaTime);
    void StartRound();
    void BeginCountdownForRound(int roundNumber);
    void SpawnRoundEnemies(int count);
    RTBEngine::ECS::GameObject* SpawnEnemyAt(RTBEngine::ECS::GameObject* spawnPoint);
    void RebindSpawnedEnemy(RTBEngine::ECS::GameObject* spawnedEnemy);
    void CleanupSpawnedEnemies();
    int GetEnemyCountForRound(int roundNumber) const;
    bool IsPlayerAlive() const;
    void UpdateCountdownText();
    void QueueRemoveHierarchy(RTBEngine::ECS::Scene* scene, RTBEngine::ECS::GameObject* root) const;
    void SetHierarchyActive(RTBEngine::ECS::GameObject* root, bool active) const;
    void CollectHierarchy(RTBEngine::ECS::GameObject* root,
                          std::vector<RTBEngine::ECS::GameObject*>& outHierarchy) const;
};
