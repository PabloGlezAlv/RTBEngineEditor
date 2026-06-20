#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <cstdint>
#include <string>
#include <vector>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

class HealthComponent;
class OnlinePlayerManager;
class RoundUIHandler;

enum class GameResult;

class RoundManager : public RTBEngine::ECS::Component
{
public:
    RoundManager() = default;
    ~RoundManager() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnValidate() override;
    void OnDestroy() override;

    RTBEngine::ECS::GameObject* playerObject = nullptr;
    OnlinePlayerManager* onlinePlayerManager = nullptr;
    RoundUIHandler* uiHandler = nullptr;
    float roundCountdownDuration = 5.0f;
    int baseEnemiesPerRound = 2;
    int additionalEnemiesPerRound = 1;
    int winningRound = 5;
    float playerRespawnDelay = 30.0f;
    float teamWipeSceneDelay = 5.0f;
    std::string finalScenePath = "Assets/Scenes/FinalScene.lua";
    std::string enemyPrefabRef = "Assets/Prefabs/Enemy Melee.prefab";

    std::vector<RTBEngine::ECS::GameObject*> spawnPoints;

    void ApplyNetworkRoundStart(int roundNumber, int enemyCount);
    void ApplyNetworkEnemySpawn(
        int roundNumber,
        int spawnPointIndex,
        int spawnIndex,
        std::uint32_t networkId);
    bool CanSpawnEnemies() const;

    size_t GetSpawnPointCount() const { return spawnPoints.size(); }

    RTB_COMPONENT(RoundManager)

private:
    enum class State {
        Countdown,
        RoundActive,
        Stopped
    };

    struct TrackedPlayer {
        RTBEngine::ECS::GameObject* pawn = nullptr;
        HealthComponent* health = nullptr;
        RTBEngine::Core::EventSubscription deathSubscription;
    };

    std::vector<RTBEngine::ECS::GameObject*> spawnedEnemies;
    std::vector<TrackedPlayer> trackedPlayers;

    RTBEngine::ECS::Prefab* enemySpawnPrefab = nullptr;
    HealthComponent* playerHealth = nullptr;
    State state = State::Stopped;
    int currentRound = 0;
    int nextRound = 1;
    float countdownRemaining = 0.0f;
    int displayedCountdownSeconds = -1;
    int displayedRespawnSeconds = -1;
    int displayedEndGameSeconds = -1;
    float localRespawnRemaining = 0.0f;
    float finalSceneDelayRemaining = 0.0f;
    bool hasRequestedEndScene = false;
    bool finalSceneLoadRequested = false;
    bool localRespawnPending = false;

    void ClampSettings();
    void InitializeRuntime();
    bool HasAnySpawnPoint() const;
    void UpdateCountdown(float deltaTime);
    void StartRound();
    void BeginCountdownForRound(int roundNumber);
    void SpawnRoundEnemies(int count, bool allowClientSpawn = false);
    RTBEngine::ECS::GameObject* SpawnEnemyAt(RTBEngine::ECS::GameObject* spawnPoint);
    RTBEngine::ECS::GameObject* SpawnEnemyAt(
        RTBEngine::ECS::GameObject* spawnPoint,
        int roundNumber,
        int spawnIndex,
        std::uint32_t networkId = 0);
    void RebindSpawnedEnemy(RTBEngine::ECS::GameObject* spawnedEnemy);
    void ConfigureOnlineEnemy(RTBEngine::ECS::GameObject* spawnedEnemy, std::uint32_t networkId);
    RTBEngine::ECS::GameObject* FindBestEnemyTarget(RTBEngine::ECS::GameObject* requester = nullptr) const;
    void CleanupSpawnedEnemies();
    int GetEnemyCountForRound(int roundNumber) const;
    void UpdateCountdownText();
    void HandleAnyPlayerDeath(HealthComponent* deadHealth);
    void BeginLocalRespawnCountdown();
    void CancelLocalRespawnCountdown();
    void UpdateLocalRespawnCountdown(float deltaTime);
    void ReviveLocalPlayer();
    void RevivePlayerPawn(RTBEngine::ECS::GameObject* pawn);
    bool AreAllPlayersDead() const;
    void BeginTeamWipe();
    bool IsTrackedPlayerLocallyControlled() const;
    void EndGame(GameResult result);
    void UpdateFinalSceneTransition(float deltaTime);
    void RequestFinalScene();
};
