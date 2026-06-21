#include "RoundManager.h"

#include "OnlineGameNetMessages.h"
#include "OnlinePlayerManager.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/NetworkTransform.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>

#include "EnemyAnimationDriver.h"
#include "EnemyLocomotionController.h"
#include "EnemyMeleeAI.h"
#include "EnemyTargetTracker.h"
#include "GameSession.h"
#include "HealthComponent.h"
#include "MeleeSphereAttackAbility.h"
#include "RoundUIHandler.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NavAgentComponent.h>
#include <RTBEngine/Scene/NavGridComponent.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Navigation/NavPathService.h>

#include <algorithm>
#include <cmath>
#include <limits>

using ThisClass = RoundManager;

RTB_REGISTER_COMPONENT(RoundManager)
    RTB_PROPERTY_GAMEOBJECT(playerObject)
    RTB_PROPERTY_COMPONENT(onlinePlayerManager, OnlinePlayerManager)
    RTB_PROPERTY_COMPONENT(uiHandler, RoundUIHandler)
    RTB_PROPERTY_RANGE(roundCountdownDuration, 0.0f, 30.0f)
    RTB_PROPERTY_RANGE(baseEnemiesPerRound, 1.0f, 100.0f)
    RTB_PROPERTY_RANGE(additionalEnemiesPerRound, 0.0f, 50.0f)
    RTB_PROPERTY_RANGE(winningRound, 1.0f, 100.0f)
    RTB_PROPERTY_RANGE(playerRespawnDelay, 0.0f, 120.0f)
    RTB_PROPERTY_RANGE(teamWipeSceneDelay, 0.0f, 30.0f)
    RTB_PROPERTY(finalScenePath)
    RTB_PROPERTY_ASSET_PATH(enemyPrefabRef, "prefab")
    RTB_PROPERTY_GAMEOBJECT_LIST(spawnPoints)
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
    InitializeRuntime();

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        GameNet::OnlineGameNetSubsystem::Init();
    }

    if (!enemySpawnPrefab) {
        RTB_WARN("[RoundManager] Assign an enemy prefab asset in the Inspector to spawn rounds.");
        state = State::Stopped;
        return;
    }

    if (!HasAnySpawnPoint()) {
        RTB_WARN("[RoundManager] Assign at least one spawn point in the Spawn Points list.");
        state = State::Stopped;
        return;
    }

    BeginCountdownForRound(1);
}

void RoundManager::OnUpdate(float deltaTime)
{
    if (hasRequestedEndScene) {
        UpdateFinalSceneTransition(deltaTime);
        return;
    }

    UpdateLocalRespawnCountdown(deltaTime);
    CleanupSpawnedEnemies();

    switch (state) {
    case State::Countdown:
        UpdateCountdown(deltaTime);
        break;
    case State::RoundActive:
        if (spawnedEnemies.empty() &&
            (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
             RTBEngine::Online::OnlineGameplayNet::IsLobbyHost())) {
            BeginCountdownForRound(currentRound + 1);
        }
        break;
    case State::Stopped:
    default:
        break;
    }
}

void RoundManager::OnFixedUpdate(float /*fixedDeltaTime*/)
{
}

void RoundManager::ApplyNetworkRoundStart(int roundNumber, int enemyCount)
{
    if (hasRequestedEndScene || roundNumber < 1 || enemyCount < 1) {
        return;
    }

    if (state == State::RoundActive &&
        currentRound == roundNumber &&
        static_cast<int>(spawnedEnemies.size()) >= enemyCount) {
        return;
    }

    if (roundNumber >= winningRound) {
        return;
    }

    if (!enemySpawnPrefab || !HasAnySpawnPoint()) {
        RTB_WARN("[RoundManager] Cannot apply network round start: spawn setup is incomplete.");
        return;
    }

    currentRound = roundNumber;
    nextRound = roundNumber;
    state = State::RoundActive;
    countdownRemaining = 0.0f;
    displayedCountdownSeconds = -1;

    if (uiHandler) {
        uiHandler->ShowRound(currentRound);
        uiHandler->HideCountdown();
    }
}

void RoundManager::ApplyNetworkEnemySpawn(
    int roundNumber,
    int spawnPointIndex,
    int spawnIndex,
    std::uint32_t networkId)
{
    if (hasRequestedEndScene ||
        roundNumber < 1 ||
        spawnPointIndex < 0 ||
        spawnIndex < 0 ||
        networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
        return;
    }

    if (!enemySpawnPrefab ||
        static_cast<size_t>(spawnPointIndex) >= spawnPoints.size()) {
        return;
    }

    RTBEngine::ECS::GameObject* spawnPoint = spawnPoints[static_cast<size_t>(spawnPointIndex)];
    if (!spawnPoint) {
        return;
    }
    RTBEngine::ECS::GameObject* spawnedEnemy =
        SpawnEnemyAt(spawnPoint, roundNumber, spawnIndex, networkId);
    if (!spawnedEnemy) {
        return;
    }

    spawnedEnemies.push_back(spawnedEnemy);

    if (state != State::RoundActive || currentRound < roundNumber) {
        currentRound = roundNumber;
        nextRound = roundNumber;
        state = State::RoundActive;
        countdownRemaining = 0.0f;
        displayedCountdownSeconds = -1;
        if (uiHandler) {
            uiHandler->ShowRound(currentRound);
            uiHandler->HideCountdown();
        }
    }
}

bool RoundManager::CanSpawnEnemies() const
{
    return enemySpawnPrefab != nullptr && HasAnySpawnPoint();
}

void RoundManager::OnValidate()
{
    ClampSettings();
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

void RoundManager::InitializeRuntime()
{
    trackedPlayers.clear();
    playerHealth = nullptr;

    if (!uiHandler && owner) {
        uiHandler = owner->GetComponent<RoundUIHandler>();
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        onlinePlayerManager &&
        onlinePlayerManager->localPlayerObject) {
        playerObject = onlinePlayerManager->localPlayerObject;
    }

    if (playerObject) {
        playerHealth = playerObject->GetComponent<HealthComponent>();
        if (!playerHealth) {
            playerHealth = playerObject->GetComponentInChildren<HealthComponent>();
        }
    }

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

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

    enemySpawnPrefab = nullptr;
    if (!enemyPrefabRef.empty()) {
        const std::string resolvedPath =
            RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(enemyPrefabRef);
        enemySpawnPrefab = RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(resolvedPath);
        if (!enemySpawnPrefab) {
            RTB_WARN("[RoundManager] Enemy prefab asset not found: '" + enemyPrefabRef + "'.");
        }
    }
}

bool RoundManager::HasAnySpawnPoint() const
{
    for (RTBEngine::ECS::GameObject* spawnPointRef : spawnPoints) {
        if (spawnPointRef) {
            return true;
        }
    }

    return false;
}

void RoundManager::UpdateCountdown(float deltaTime)
{
    countdownRemaining = std::max(0.0f, countdownRemaining - deltaTime);

    if (countdownRemaining <= 0.0f) {
        if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
            !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            countdownRemaining = 0.0f;
            return;
        }

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

    if (!enemySpawnPrefab || !HasAnySpawnPoint()) {
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

    const int enemyCount = GetEnemyCountForRound(currentRound);

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        GameNet::RoundStartSnapshot roundStart;
        roundStart.roundNumber = currentRound;
        roundStart.enemyCount = enemyCount;
        GameNet::OnlineGameNetSubsystem::BroadcastRoundStart(roundStart);
    }

    SpawnRoundEnemies(enemyCount);
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

void RoundManager::SpawnRoundEnemies(int count, bool allowClientSpawn)
{
    if (!enemySpawnPrefab || spawnPoints.empty() || count < 1) {
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() &&
        !allowClientSpawn) {
        return;
    }

    for (int index = 0; index < count; ++index) {
        const int spawnPointIndex = static_cast<int>(index % spawnPoints.size());
        RTBEngine::ECS::GameObject* spawnPoint = spawnPoints[static_cast<size_t>(spawnPointIndex)];
        if (!spawnPoint) {
            continue;
        }

        RTBEngine::ECS::GameObject* spawnedEnemy = SpawnEnemyAt(spawnPoint, currentRound, index);
        if (!spawnedEnemy) {
            continue;
        }

        spawnedEnemies.push_back(spawnedEnemy);

        if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
            RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            RTBEngine::ECS::NetworkIdentity* identity =
                spawnedEnemy->GetComponent<RTBEngine::ECS::NetworkIdentity>();
            GameNet::EnemySpawnSnapshot snapshot;
            snapshot.networkId = identity ? identity->GetNetworkId() : 0;
            snapshot.roundNumber = currentRound;
            snapshot.spawnPointIndex = spawnPointIndex;
            snapshot.spawnIndex = index;
            GameNet::OnlineGameNetSubsystem::BroadcastEnemySpawn(snapshot);
        }
    }

    if (RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene()) {
        RTBEngine::ECS::NavGridComponent::ActivateAllBakedInScene(scene);
    }
}

RTBEngine::ECS::GameObject* RoundManager::SpawnEnemyAt(RTBEngine::ECS::GameObject* spawnPoint)
{
    return SpawnEnemyAt(spawnPoint, currentRound, static_cast<int>(spawnedEnemies.size()));
}

RTBEngine::ECS::GameObject* RoundManager::SpawnEnemyAt(
    RTBEngine::ECS::GameObject* spawnPoint,
    int roundNumber,
    int spawnIndex,
    std::uint32_t networkId)
{
    RTBEngine::ECS::Prefab* spawnPrefab = enemySpawnPrefab;
    if (!spawnPoint || !spawnPrefab || roundNumber < 1 || spawnIndex < 0) {
        return nullptr;
    }

    const bool isOnline = RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby();
    if (isOnline) {
        if (networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId &&
            RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            networkId = RTBEngine::Online::OnlineGameplayNet::AllocateNetworkObjectId();
        }

        if (networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
            return nullptr;
        }

        if (GameNet::OnlineGameNetSubsystem::HasEnemyWithNetworkId(networkId)) {
            return nullptr;
        }
    }

    RTBEngine::ECS::GameObject* spawnedEnemy =
        RTBEngine::ECS::SceneManager::GetInstance().Instantiate(
            *spawnPrefab,
            spawnPoint->GetWorldPosition(),
            spawnPoint->GetWorldRotation());
    if (!spawnedEnemy) {
        return nullptr;
    }

    ConfigureOnlineEnemy(spawnedEnemy, networkId);
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
    auto* navAgent = spawnedEnemy->GetComponent<RTBEngine::ECS::NavAgentComponent>();
    auto* meleeAI = spawnedEnemy->GetComponent<EnemyMeleeAI>();
    auto* meleeAttack = spawnedEnemy->GetComponent<MeleeSphereAttackAbility>();
    auto* health = spawnedEnemy->GetComponent<HealthComponent>();
    auto* animator = spawnedEnemy->GetComponentInChildren<RTBEngine::Animation::Animator>();

    if (targetTracker) {
        targetTracker->SetTarget(FindBestEnemyTarget(spawnedEnemy));
    }

    if (animationDriver) {
        animationDriver->animator = animator;
        animationDriver->OnValidate();
    }

    if (meleeAI) {
        meleeAI->health = health;
        meleeAI->targetTracker = targetTracker;
        meleeAI->animationDriver = animationDriver;
        meleeAI->locomotion = locomotion;
        meleeAI->navAgent = navAgent;
        meleeAI->meleeAttack = meleeAttack;
        meleeAI->FinalizeSpawnSetup();

        if (!navAgent) {
            RTB_WARN("[RoundManager] Spawned enemy '" + spawnedEnemy->GetName() +
                "' is missing NavAgentComponent; navigation will be disabled.");
        } else if (targetTracker && targetTracker->targetObject) {
            navAgent->SetDestination(targetTracker->targetObject->GetWorldPosition());
            navAgent->EnsurePathReady();
        }
    }
}

void RoundManager::ConfigureOnlineEnemy(
    RTBEngine::ECS::GameObject* spawnedEnemy,
    std::uint32_t networkId)
{
    if (!spawnedEnemy) {
        return;
    }

    if (networkId != RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
        if (RTBEngine::ECS::NetworkIdentity* identity =
                spawnedEnemy->GetComponent<RTBEngine::ECS::NetworkIdentity>()) {
            identity->SetNetworkId(networkId);
        }

        if (RTBEngine::ECS::NetworkTransform* networkTransform =
                spawnedEnemy->GetComponent<RTBEngine::ECS::NetworkTransform>()) {
            networkTransform->OnValidate();
        }
    }

    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    if (RTBEngine::ECS::RigidBodyComponent* rigidBody =
            spawnedEnemy->GetComponent<RTBEngine::ECS::RigidBodyComponent>()) {
        rigidBody->bodyType = RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()
            ? RTBEngine::Physics::RigidBodyType::Dynamic
            : RTBEngine::Physics::RigidBodyType::Kinematic;
        rigidBody->OnValidate();
    }
}

RTBEngine::ECS::GameObject* RoundManager::FindBestEnemyTarget(RTBEngine::ECS::GameObject* requester) const
{
    RTBEngine::ECS::GameObject* bestTarget = nullptr;
    float bestDistanceSquared = std::numeric_limits<float>::max();
    const RTBEngine::Math::Vector3 requesterPosition =
        requester ? requester->GetWorldPosition() : RTBEngine::Math::Vector3::Zero();

    for (const TrackedPlayer& trackedPlayer : trackedPlayers) {
        if (!trackedPlayer.pawn || !trackedPlayer.health || trackedPlayer.health->IsDead()) {
            continue;
        }

        RTBEngine::Math::Vector3 offset = trackedPlayer.pawn->GetWorldPosition() - requesterPosition;
        offset.y = 0.0f;
        const float distanceSquared = offset.LengthSquared();
        if (distanceSquared < bestDistanceSquared) {
            bestDistanceSquared = distanceSquared;
            bestTarget = trackedPlayer.pawn;
        }
    }

    if (bestTarget) {
        return bestTarget;
    }

    return playerObject;
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
