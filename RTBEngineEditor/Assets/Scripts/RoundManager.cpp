#include "RoundManager.h"

#include "EnemyAnimationDriver.h"
#include "EnemyLocomotionController.h"
#include "EnemyMeleeAI.h"
#include "EnemySpawnPoint.h"
#include "EnemyTargetTracker.h"
#include "GameSession.h"
#include "HealthComponent.h"
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
    RTB_PROPERTY_RANGE(finalSceneDelay, 0.0f, 30.0f)
    RTB_PROPERTY(finalScenePath)
RTB_END_REGISTER(RoundManager)

void RoundManager::OnStart()
{
    GameSession::GetInstance().Reset();
    hasRequestedEndScene = false;
    finalSceneLoadRequested = false;
    finalSceneDelayRemaining = 0.0f;
    ClampSettings();
    ResolveDependencies();
    RebindPlayerDeathSubscription();
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
    if (hasRequestedEndScene) {
        UpdateFinalSceneTransition(deltaTime);
        return;
    }

    ClampSettings();
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
    UnsubscribeFromPlayerHealth();
}

void RoundManager::ClampSettings()
{
    roundCountdownDuration = std::max(0.0f, roundCountdownDuration);
    baseEnemiesPerRound = std::max(1, baseEnemiesPerRound);
    additionalEnemiesPerRound = std::max(0, additionalEnemiesPerRound);
    winningRound = std::max(1, winningRound);
    finalSceneDelay = std::max(0.0f, finalSceneDelay);
}

void RoundManager::ResolveDependencies()
{
    if (!uiHandler && owner) {
        uiHandler = owner->GetComponent<RoundUIHandler>();
    }

    if (!playerObject) {
        return;
    }

    if (!playerHealth || playerHealth->GetOwner() != playerObject) {
        playerHealth = playerObject->GetComponent<HealthComponent>();
        if (!playerHealth) {
            playerHealth = playerObject->GetComponentInChildren<HealthComponent>();
        }
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

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    enemyPrefab = RTBEngine::ECS::Prefab::CreateFromGameObject(enemyTemplate);
    SetHierarchyActive(enemyTemplate, false);
    QueueRemoveHierarchy(scene, enemyTemplate);
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
    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene || !spawnPoint || !enemyPrefab) {
        return nullptr;
    }

    std::vector<RTBEngine::ECS::GameObject*> spawnedChildren;
    RTBEngine::ECS::GameObject* spawnedEnemy = enemyPrefab->Instantiate(nullptr, spawnedChildren);
    if (!spawnedEnemy) {
        return nullptr;
    }

    scene->AddGameObject(spawnedEnemy);
    for (RTBEngine::ECS::GameObject* child : spawnedChildren) {
        if (child) {
            scene->AddGameObject(child);
        }
    }

    spawnedEnemy->GetTransform().SetPosition(spawnPoint->GetWorldPosition());
    spawnedEnemy->GetTransform().SetRotation(spawnPoint->GetWorldRotation());
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
    }
}

void RoundManager::CleanupSpawnedEnemies()
{
    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        spawnedEnemies.clear();
        return;
    }

    auto removeIt = std::remove_if(
        spawnedEnemies.begin(),
        spawnedEnemies.end(),
        [this, scene](RTBEngine::ECS::GameObject* spawnedEnemy) {
            if (!spawnedEnemy) {
                return true;
            }

            if (spawnedEnemy->IsActive()) {
                return false;
            }

            SetHierarchyActive(spawnedEnemy, false);
            QueueRemoveHierarchy(scene, spawnedEnemy);
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

void RoundManager::RebindPlayerDeathSubscription()
{
    if (subscribedPlayerHealth == playerHealth && playerDeathSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromPlayerHealth();

    if (!playerHealth) {
        return;
    }

    subscribedPlayerHealth = playerHealth;
    playerDeathSubscription = playerHealth->SubscribeToDeath(
        [this](const HealthComponent::DeathEvent&) {
            HandlePlayerDeath();
        });

    if (playerHealth->IsDead()) {
        HandlePlayerDeath();
    }
}

void RoundManager::UnsubscribeFromPlayerHealth()
{
    playerDeathSubscription.Reset();
    subscribedPlayerHealth = nullptr;
}

void RoundManager::HandlePlayerDeath()
{
    EndGame(GameResult::Lose);
}

void RoundManager::EndGame(GameResult result)
{
    if (hasRequestedEndScene) {
        return;
    }

    hasRequestedEndScene = true;
    finalSceneLoadRequested = false;
    finalSceneDelayRemaining = std::max(0.0f, finalSceneDelay);
    state = State::Stopped;
    GameSession::GetInstance().SetResult(result);

    if (uiHandler) {
        uiHandler->HideCountdown();
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

void RoundManager::QueueRemoveHierarchy(RTBEngine::ECS::Scene* scene, RTBEngine::ECS::GameObject* root) const
{
    if (!scene || !root) {
        return;
    }

    std::vector<RTBEngine::ECS::GameObject*> hierarchy;
    CollectHierarchy(root, hierarchy);

    for (auto it = hierarchy.rbegin(); it != hierarchy.rend(); ++it) {
        scene->RemoveGameObject(*it);
    }
}

void RoundManager::SetHierarchyActive(RTBEngine::ECS::GameObject* root, bool active) const
{
    if (!root) {
        return;
    }

    root->SetActive(active);
    for (RTBEngine::ECS::GameObject* child : root->GetChildren()) {
        SetHierarchyActive(child, active);
    }
}

void RoundManager::CollectHierarchy(RTBEngine::ECS::GameObject* root,
                                    std::vector<RTBEngine::ECS::GameObject*>& outHierarchy) const
{
    if (!root) {
        return;
    }

    outHierarchy.push_back(root);
    for (RTBEngine::ECS::GameObject* child : root->GetChildren()) {
        CollectHierarchy(child, outHierarchy);
    }
}
