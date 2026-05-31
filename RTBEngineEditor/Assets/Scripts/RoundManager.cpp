#include "RoundManager.h"

#include "OnlineGameNetMessages.h"
#include "OnlinePlayerManager.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/ECS/NetworkIdentity.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>

#include "EnemyAnimationDriver.h"
#include "EnemyLocomotionController.h"
#include "EnemyMeleeAI.h"
#include "EnemySpawnPoint.h"
#include "EnemyTargetTracker.h"
#include "GameSession.h"
#include "HealthComponent.h"
#include "MeleeSphereAttackAbility.h"
#include "RoundUIHandler.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>

#include <algorithm>
#include <cmath>

using ThisClass = RoundManager;

RTB_REGISTER_COMPONENT(RoundManager)
    RTB_PROPERTY_GAMEOBJECT(enemyTemplate)
    RTB_PROPERTY_GAMEOBJECT(playerObject)
    RTB_PROPERTY_COMPONENT(uiHandler, RoundUIHandler)
    RTB_PROPERTY_RANGE(roundCountdownDuration, 0.0f, 30.0f)
    RTB_PROPERTY_RANGE(baseEnemiesPerRound, 1.0f, 100.0f)
    RTB_PROPERTY_RANGE(additionalEnemiesPerRound, 0.0f, 50.0f)
    RTB_PROPERTY_RANGE(winningRound, 1.0f, 100.0f)
    RTB_PROPERTY_RANGE(playerRespawnDelay, 0.0f, 120.0f)
    RTB_PROPERTY_RANGE(teamWipeSceneDelay, 0.0f, 30.0f)
    RTB_PROPERTY(finalScenePath)
RTB_END_REGISTER(RoundManager)

void RoundManager::OnStart()
{
    GameSession::GetInstance().Reset();
    hasRequestedEndScene = false;
    finalSceneLoadRequested = false;
    finalSceneDelayRemaining = 0.0f;
    localRespawnPending = false;
    localRespawnRemaining = 0.0f;
    ClampSettings();
    ResolveDependencies();
    RefreshTrackedPlayers();
    RefreshSpawnPoints();
    CreateEnemyPrefabFromTemplate();

    if (!enemyPrefab) {
        RTB_WARN("[RoundManager] Assign an enemyTemplate GameObject to spawn rounds.");
        state = State::Stopped;
        return;
    }

    if (spawnPoints.empty()) {
        RTB_WARN("[RoundManager] Add at least one EnemySpawnPoint to the scene.");
        state = State::Stopped;
        return;
    }

    BeginCountdownForRound(1);
}

void RoundManager::OnUpdate(float deltaTime)
{
    ClampSettings();
    ResolveDependencies();
    RefreshTrackedPlayers();

    if (hasRequestedEndScene) {
        UpdateFinalSceneTransition(deltaTime);
        return;
    }

    UpdateLocalRespawnCountdown(deltaTime);
    RefreshSpawnPoints();
    CleanupSpawnedEnemies();

    switch (state) {
    case State::Countdown:
        UpdateCountdown(deltaTime);
        break;
    case State::RoundActive:
        if (spawnedEnemies.empty()) {
            BeginCountdownForRound(currentRound + 1);
        }
        break;
    case State::Stopped:
    default:
        break;
    }
}

void RoundManager::OnValidate()
{
    ClampSettings();
    ResolveDependencies();
    RefreshSpawnPoints();
}

void RoundManager::OnDestroy()
{
    trackedPlayers.clear();
}

void RoundManager::ClampSettings()
{
    roundCountdownDuration = std::max(0.0f, roundCountdownDuration);
    baseEnemiesPerRound = std::max(1, baseEnemiesPerRound);
    additionalEnemiesPerRound = std::max(0, additionalEnemiesPerRound);
    winningRound = std::max(1, winningRound);
    playerRespawnDelay = std::max(0.0f, playerRespawnDelay);
    teamWipeSceneDelay = std::max(0.0f, teamWipeSceneDelay);
}

void RoundManager::ResolveDependencies()
{
    if (!uiHandler && owner) {
        uiHandler = owner->GetComponent<RoundUIHandler>();
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (scene) {
            for (const auto& gameObject : scene->GetGameObjects()) {
                if (!gameObject) {
                    continue;
                }

                OnlinePlayerManager* onlinePlayers = gameObject->GetComponent<OnlinePlayerManager>();
                if (onlinePlayers && onlinePlayers->localPlayerObject) {
                    playerObject = onlinePlayers->localPlayerObject;
                    break;
                }
            }
        }
    }

    if (!playerObject) {
        return;
    }

    HealthComponent* resolvedHealth = playerObject->GetComponent<HealthComponent>();
    if (!resolvedHealth) {
        resolvedHealth = playerObject->GetComponentInChildren<HealthComponent>();
    }

    playerHealth = resolvedHealth;
}

void RoundManager::RefreshTrackedPlayers()
{
    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        trackedPlayers.clear();
        cachedPlayerScanVersion = 0;
        return;
    }

    const uint32_t hierarchyVersion = RTBEngine::ECS::GameObject::GetHierarchyVersion();
    if (!trackedPlayers.empty() && hierarchyVersion == cachedPlayerScanVersion) {
        return;
    }

    cachedPlayerScanVersion = hierarchyVersion;
    trackedPlayers.clear();

    for (const auto& gameObject : scene->GetGameObjects()) {
        if (!gameObject) {
            continue;
        }

        ThirdPersonCharacterController* controller = gameObject->GetComponent<ThirdPersonCharacterController>();
        if (!controller || controller->team != static_cast<int>(CharacterTeam::Player)) {
            continue;
        }

        HealthComponent* health = gameObject->GetComponent<HealthComponent>();
        if (!health) {
            health = gameObject->GetComponentInChildren<HealthComponent>();
        }

        if (!health) {
            continue;
        }

        TrackedPlayer trackedPlayer;
        trackedPlayer.pawn = gameObject.get();
        trackedPlayer.health = health;
        trackedPlayer.deathSubscription = health->SubscribeToDeath(
            [this, health](const HealthComponent::DeathEvent&) {
                HandleAnyPlayerDeath(health);
            });

        trackedPlayers.push_back(std::move(trackedPlayer));
    }
}

void RoundManager::RefreshSpawnPoints()
{
    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        spawnPoints.clear();
        cachedHierarchyVersion = 0;
        return;
    }

    const uint32_t hierarchyVersion = RTBEngine::ECS::GameObject::GetHierarchyVersion();
    if (!spawnPoints.empty() && hierarchyVersion == cachedHierarchyVersion) {
        return;
    }

    spawnPoints.clear();
    for (const auto& gameObject : scene->GetGameObjects()) {
        if (!gameObject || !gameObject->GetComponent<EnemySpawnPoint>()) {
            continue;
        }

        spawnPoints.push_back(gameObject.get());
    }

    cachedHierarchyVersion = hierarchyVersion;
}

void RoundManager::CreateEnemyPrefabFromTemplate()
{
    if (enemyPrefab || !enemyTemplate) {
        return;
    }

    auto& sceneManager = RTBEngine::ECS::SceneManager::GetInstance();
    if (!sceneManager.GetActiveScene()) {
        return;
    }

    enemyPrefab = RTBEngine::ECS::Prefab::CreateFromGameObject(enemyTemplate);
    sceneManager.DeactivateHierarchy(enemyTemplate);
    enemyTemplate = nullptr;
}

void RoundManager::UpdateCountdown(float deltaTime)
{
    countdownRemaining = std::max(0.0f, countdownRemaining - deltaTime);

    if (countdownRemaining <= 0.0f) {
        StartRound();
        return;
    }

    UpdateCountdownText();
}

void RoundManager::StartRound()
{
    if (nextRound >= winningRound) {
        EndGame(GameResult::Win);
        return;
    }

    if (!enemyPrefab || spawnPoints.empty()) {
        state = State::Stopped;
        if (uiHandler) {
            uiHandler->HideCountdown();
        }
        return;
    }

    currentRound = std::max(1, nextRound);
    state = State::RoundActive;
    displayedCountdownSeconds = -1;

    if (uiHandler) {
        uiHandler->ShowRound(currentRound);
        uiHandler->HideCountdown();
    }

    SpawnRoundEnemies(GetEnemyCountForRound(currentRound));
}

void RoundManager::BeginCountdownForRound(int roundNumber)
{
    nextRound = std::max(1, roundNumber);
    countdownRemaining = roundCountdownDuration;
    displayedCountdownSeconds = -1;
    state = State::Countdown;

    if (uiHandler) {
        uiHandler->ShowRound(nextRound);
    }

    if (countdownRemaining <= 0.0f) {
        StartRound();
        return;
    }

    UpdateCountdownText();
}

void RoundManager::SpawnRoundEnemies(int count)
{
    if (!enemyPrefab || spawnPoints.empty()) {
        return;
    }

    for (int index = 0; index < count; ++index) {
        RTBEngine::ECS::GameObject* spawnPoint =
            spawnPoints[static_cast<size_t>(index) % spawnPoints.size()];
        RTBEngine::ECS::GameObject* spawnedEnemy = SpawnEnemyAt(spawnPoint);
        if (spawnedEnemy) {
            spawnedEnemies.push_back(spawnedEnemy);
        }
    }
}

RTBEngine::ECS::GameObject* RoundManager::SpawnEnemyAt(RTBEngine::ECS::GameObject* spawnPoint)
{
    if (!spawnPoint || !enemyPrefab) {
        return nullptr;
    }

    RTBEngine::ECS::GameObject* spawnedEnemy =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate(
            *enemyPrefab,
            spawnPoint->GetWorldPosition(),
            spawnPoint->GetWorldRotation());
    if (!spawnedEnemy) {
        return nullptr;
    }

    RebindSpawnedEnemy(spawnedEnemy);
    return spawnedEnemy;
}

void RoundManager::RebindSpawnedEnemy(RTBEngine::ECS::GameObject* spawnedEnemy)
{
    if (!spawnedEnemy) {
        return;
    }

    auto* targetTracker = spawnedEnemy->GetComponent<EnemyTargetTracker>();
    auto* animationDriver = spawnedEnemy->GetComponent<EnemyAnimationDriver>();
    auto* locomotion = spawnedEnemy->GetComponent<EnemyLocomotionController>();
    auto* meleeAI = spawnedEnemy->GetComponent<EnemyMeleeAI>();
    auto* meleeAttack = spawnedEnemy->GetComponent<MeleeSphereAttackAbility>();
    auto* health = spawnedEnemy->GetComponent<HealthComponent>();
    auto* animator = spawnedEnemy->GetComponentInChildren<RTBEngine::Animation::Animator>();

    if (targetTracker) {
        targetTracker->SetTarget(playerObject);
    }

    if (animationDriver) {
        animationDriver->animator = animator;
    }

    if (meleeAI) {
        meleeAI->health = health;
        meleeAI->targetTracker = targetTracker;
        meleeAI->animationDriver = animationDriver;
        meleeAI->locomotion = locomotion;
        meleeAI->meleeAttack = meleeAttack;
    }
}

void RoundManager::CleanupSpawnedEnemies()
{
    auto removeIt = std::remove_if(
        spawnedEnemies.begin(),
        spawnedEnemies.end(),
        [](RTBEngine::ECS::GameObject* spawnedEnemy) {
            if (!spawnedEnemy) {
                return true;
            }

            if (spawnedEnemy->IsActive()) {
                return false;
            }

            RTBEngine::ECS::SceneManager::GetInstance().DeactivateHierarchy(spawnedEnemy);
            return true;
        });

    spawnedEnemies.erase(removeIt, spawnedEnemies.end());
}

int RoundManager::GetEnemyCountForRound(int roundNumber) const
{
    return std::max(1, baseEnemiesPerRound + std::max(0, roundNumber - 1) * additionalEnemiesPerRound);
}

void RoundManager::UpdateCountdownText()
{
    if (!uiHandler) {
        return;
    }

    const int countdownSeconds = static_cast<int>(std::ceil(countdownRemaining));
    if (countdownSeconds == displayedCountdownSeconds) {
        return;
    }

    displayedCountdownSeconds = countdownSeconds;
    uiHandler->ShowCountdown(displayedCountdownSeconds);
}

bool RoundManager::AreAllPlayersDead() const
{
    if (trackedPlayers.empty()) {
        if (playerHealth) {
            return playerHealth->IsDead();
        }

        return false;
    }

    for (const TrackedPlayer& trackedPlayer : trackedPlayers) {
        if (!trackedPlayer.health || !trackedPlayer.health->IsDead()) {
            return false;
        }
    }

    return true;
}

void RoundManager::HandleAnyPlayerDeath(HealthComponent* deadHealth)
{
    if (hasRequestedEndScene || !deadHealth) {
        return;
    }

    if (AreAllPlayersDead()) {
        if (trackedPlayers.size() <= 1) {
            if (playerHealth && deadHealth == playerHealth && IsTrackedPlayerLocallyControlled() &&
                !localRespawnPending) {
                BeginLocalRespawnCountdown();
            }
        } else {
            BeginTeamWipe();
        }

        return;
    }

    if (!playerHealth || deadHealth != playerHealth || !IsTrackedPlayerLocallyControlled()) {
        return;
    }

    if (!localRespawnPending) {
        BeginLocalRespawnCountdown();
    }
}

void RoundManager::BeginLocalRespawnCountdown()
{
    localRespawnPending = true;
    localRespawnRemaining = playerRespawnDelay;
    displayedRespawnSeconds = -1;

    if (uiHandler) {
        if (localRespawnRemaining > 0.0f) {
            uiHandler->ShowRespawnCountdown(static_cast<int>(std::ceil(localRespawnRemaining)));
        } else {
            uiHandler->HideRespawnCountdown();
        }
    }

    if (localRespawnRemaining <= 0.0f) {
        ReviveLocalPlayer();
    }
}

void RoundManager::CancelLocalRespawnCountdown()
{
    localRespawnPending = false;
    localRespawnRemaining = 0.0f;
    displayedRespawnSeconds = -1;

    if (uiHandler) {
        uiHandler->HideRespawnCountdown();
    }
}

void RoundManager::UpdateLocalRespawnCountdown(float deltaTime)
{
    if (!localRespawnPending || hasRequestedEndScene) {
        return;
    }

    localRespawnRemaining = std::max(0.0f, localRespawnRemaining - std::max(0.0f, deltaTime));

    if (uiHandler) {
        const int respawnSeconds = static_cast<int>(std::ceil(localRespawnRemaining));
        if (respawnSeconds != displayedRespawnSeconds) {
            displayedRespawnSeconds = respawnSeconds;
            if (localRespawnRemaining > 0.0f) {
                uiHandler->ShowRespawnCountdown(respawnSeconds);
            } else {
                uiHandler->HideRespawnCountdown();
            }
        }
    }

    if (localRespawnRemaining <= 0.0f) {
        ReviveLocalPlayer();
    }
}

void RoundManager::ReviveLocalPlayer()
{
    CancelLocalRespawnCountdown();

    if (!playerObject) {
        return;
    }

    RevivePlayerPawn(playerObject);
}

void RoundManager::RevivePlayerPawn(RTBEngine::ECS::GameObject* pawn)
{
    if (!pawn) {
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity || identity->networkPlayerSlot < 0) {
            return;
        }

        const int playerSlot = identity->networkPlayerSlot;
        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            GameNet::OnlineGameNetSubsystem::ApplyPlayerReviveForSlot(playerSlot);
            GameNet::OnlineGameNetSubsystem::BroadcastPlayerRevive(playerSlot);
        } else {
            GameNet::OnlineGameNetSubsystem::RequestPlayerRevive(playerSlot);
        }

        return;
    }

    HealthComponent* health = pawn->GetComponent<HealthComponent>();
    if (!health) {
        health = pawn->GetComponentInChildren<HealthComponent>();
    }

    if (health && health->IsDead()) {
        health->Revive();
    }

    if (ThirdPersonCharacterController* controller = pawn->GetComponent<ThirdPersonCharacterController>()) {
        controller->ReviveFromDeath();
    }
}

void RoundManager::BeginTeamWipe()
{
    CancelLocalRespawnCountdown();
    EndGame(GameResult::Lose);
}

bool RoundManager::IsTrackedPlayerLocallyControlled() const
{
    if (!playerObject) {
        return false;
    }

    const RTBEngine::ECS::NetworkIdentity* identity =
        playerObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    return identity->IsLocallyControlled();
}

void RoundManager::EndGame(GameResult result)
{
    if (hasRequestedEndScene) {
        return;
    }

    hasRequestedEndScene = true;
    finalSceneLoadRequested = false;
    finalSceneDelayRemaining = std::max(0.0f, teamWipeSceneDelay);
    displayedEndGameSeconds = -1;
    state = State::Stopped;
    GameSession::GetInstance().SetResult(result);

    if (uiHandler) {
        uiHandler->HideCountdown();
        if (finalSceneDelayRemaining > 0.0f) {
            uiHandler->ShowEndGameCountdown(static_cast<int>(std::ceil(finalSceneDelayRemaining)));
        } else {
            uiHandler->HideEndGameCountdown();
        }
    }

    if (finalSceneDelayRemaining <= 0.0f) {
        RequestFinalScene();
    }
}

void RoundManager::UpdateFinalSceneTransition(float deltaTime)
{
    if (finalSceneLoadRequested) {
        return;
    }

    finalSceneDelayRemaining = std::max(0.0f, finalSceneDelayRemaining - std::max(0.0f, deltaTime));

    if (uiHandler) {
        const int endGameSeconds = static_cast<int>(std::ceil(finalSceneDelayRemaining));
        if (endGameSeconds != displayedEndGameSeconds) {
            displayedEndGameSeconds = endGameSeconds;
            if (finalSceneDelayRemaining > 0.0f) {
                uiHandler->ShowEndGameCountdown(endGameSeconds);
            } else {
                uiHandler->HideEndGameCountdown();
            }
        }
    }

    if (finalSceneDelayRemaining <= 0.0f) {
        RequestFinalScene();
    }
}

void RoundManager::RequestFinalScene()
{
    if (finalSceneLoadRequested) {
        return;
    }

    finalSceneLoadRequested = true;
    if (finalScenePath.empty()) {
        RTB_WARN("[RoundManager] finalScenePath is empty; cannot load final scene.");
        return;
    }

    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(finalScenePath.c_str());
}
