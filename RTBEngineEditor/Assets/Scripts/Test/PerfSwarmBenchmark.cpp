#include "PerfSwarmBenchmark.h"
#include "PerfSwarmAgent.h"
#include "SwarmComponents.h"
#include "SwarmSimulation.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/ECS/Components/LocalTransform.h>
#include <RTBEngine/ECS/World.h>
#include <RTBEngine/Math/Matrix/Matrix4.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/MeshRenderer.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIText.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

using ThisClass = PerfSwarmBenchmark;

namespace {
    constexpr float kTwoPi = 6.28318530718f;

    std::vector<RTBEngine::Math::Matrix4> g_instanceMatrices;
    std::vector<RTBEngine::Math::Vector4> g_instanceColors;
}

RTB_REGISTER_COMPONENT(PerfSwarmBenchmark)
    RTB_PROPERTY(useEcs)
    RTB_PROPERTY_RANGE(agentCount, 1, 20000)
    RTB_PROPERTY_RANGE(spawnRadiusMin, 0.1f, 50.0f)
    RTB_PROPERTY_RANGE(spawnRadiusMax, 0.1f, 80.0f)
    RTB_PROPERTY_RANGE(agentScale, 0.01f, 2.0f)
    RTB_PROPERTY_RANGE(statusRefreshSeconds, 0.05f, 2.0f)
    RTB_PROPERTY(statusTextObjectName)
RTB_END_REGISTER(PerfSwarmBenchmark)

void PerfSwarmBenchmark::OnValidate()
{
    agentCount = std::max(1, agentCount);
    spawnRadiusMin = std::max(0.1f, spawnRadiusMin);
    spawnRadiusMax = std::max(spawnRadiusMin, spawnRadiusMax);
    agentScale = std::max(0.01f, agentScale);
    statusRefreshSeconds = std::max(0.05f, statusRefreshSeconds);
}

void PerfSwarmBenchmark::OnStart()
{
    OnValidate();
    ResolveStatusText();

    if (useEcs) {
        if (RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive()) {
            RTBEngine::ECS::RegisterSwarmSystems(*world);
        }
    }

    SpawnSwarm();

    std::ostringstream message;
    message << "[PerfSwarm] Spawned " << spawnedCount << " agents ("
            << (useEcs ? "ECS+MeshRenderer(instanced)" : "OOP+MeshRenderer") << ")";
    RTB_INFO(message.str());
}

void PerfSwarmBenchmark::OnUpdate(float deltaTime)
{
    RefreshStatus(deltaTime);
}

void PerfSwarmBenchmark::OnLateUpdate(float /*deltaTime*/)
{
    // After ECS Simulation: publish dense transforms into the generic instanced drawer.
    PublishEcsInstances();
}

void PerfSwarmBenchmark::ResolveStatusText()
{
    statusText = nullptr;
    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (!scene || statusTextObjectName.empty()) {
        return;
    }

    for (const auto& goPtr : scene->GetGameObjects()) {
        RTBEngine::Scene::GameObject* go = goPtr.get();
        if (!go || go->GetName() != statusTextObjectName) {
            continue;
        }
        statusText = go->GetComponent<RTBEngine::UI::UIText>();
        return;
    }
}

void PerfSwarmBenchmark::EnsureInstanceMeshRenderer()
{
    if (!useEcs || !owner) {
        return;
    }

    instanceMeshRenderer = owner->GetComponent<RTBEngine::Scene::MeshRenderer>();
    if (!instanceMeshRenderer) {
        instanceMeshRenderer = new RTBEngine::Scene::MeshRenderer();
        owner->AddComponent(instanceMeshRenderer);
    }

    RTBEngine::Core::ResourceManager& resources = RTBEngine::Core::ResourceManager::GetInstance();
    RTBEngine::Rendering::Mesh* cubeMesh =
        resources.LoadModel(RTBEngine::Core::ResourceManager::DEFAULT_CUBE_PATH);
    instanceMeshRenderer->SetMesh(cubeMesh);
    instanceMeshRenderer->shaderRef = "basic";
    instanceMeshRenderer->colorRef = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void PerfSwarmBenchmark::PublishEcsInstances()
{
    if (!useEcs) {
        return;
    }

    if (!instanceMeshRenderer && owner) {
        EnsureInstanceMeshRenderer();
    }
    if (!instanceMeshRenderer) {
        return;
    }

    RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
    if (!world) {
        instanceMeshRenderer->ClearInstances();
        return;
    }

    g_instanceMatrices.clear();
    g_instanceColors.clear();
    const RTBEngine::Math::Vector3 scaleVec(agentScale, agentScale, agentScale);

    world->ForEach<RTBEngine::ECS::LocalTransform, RTBEngine::ECS::SwarmMotion, RTBEngine::ECS::SwarmColor>(
        [&](RTBEngine::ECS::Entity /*entity*/,
            RTBEngine::ECS::LocalTransform& transform,
            RTBEngine::ECS::SwarmMotion& /*motion*/,
            RTBEngine::ECS::SwarmColor& swarmColor) {
            g_instanceMatrices.push_back(
                RTBEngine::Math::Matrix4::Translate(transform.position)
                * RTBEngine::Math::Matrix4::Scale(scaleVec));
            g_instanceColors.push_back(swarmColor.rgba);
        });

    instanceMeshRenderer->SetInstances(g_instanceMatrices.data(), g_instanceMatrices.size());
    if (!g_instanceColors.empty()) {
        instanceMeshRenderer->SetInstanceColors(g_instanceColors.data(), g_instanceColors.size());
    }
}

void PerfSwarmBenchmark::SpawnSwarm()
{
    spawnedCount = 0;
    RTBEngine::Scene::SceneManager& sceneManager = RTBEngine::Scene::SceneManager::GetInstance();
    RTBEngine::Core::ResourceManager& resources = RTBEngine::Core::ResourceManager::GetInstance();

    if (useEcs) {
        RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
        if (!world) {
            RTB_ERROR("[PerfSwarm] No active ECS World");
            return;
        }

        EnsureInstanceMeshRenderer();

        const float radiusSpan = spawnRadiusMax - spawnRadiusMin;
        for (int i = 0; i < agentCount; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(std::max(1, agentCount));
            const float phase = t * kTwoPi;
            const float radius = spawnRadiusMin + radiusSpan * (0.15f + 0.85f * t);

            RTBEngine::ECS::LocalTransform transform;
            transform.position = RTBEngine::Math::Vector3(
                swarmCenter.x + std::cos(phase) * radius,
                swarmCenter.y + 1.2f,
                swarmCenter.z + std::sin(phase) * radius);
            transform.fixedHeight = transform.position.y;

            RTBEngine::ECS::SwarmMotion motion;
            motion.center = swarmCenter;
            motion.phase = phase;
            motion.angularSpeed = 0.55f + 1.75f * std::fmod(t * 7.3f, 1.0f);
            motion.orbitRadius = radius;
            motion.bobAmplitude = 0.35f + 0.90f * std::fmod(t * 3.7f, 1.0f);
            motion.bobSpeed = 1.2f + 2.4f * std::fmod(t * 5.1f, 1.0f);
            motion.height = swarmCenter.y + 1.2f;

            RTBEngine::ECS::SwarmColor swarmColor;
            swarmColor.rgba = RTBEngine::Math::Vector4(
                0.25f + 0.75f * t,
                0.35f + 0.40f * (1.0f - t),
                0.90f - 0.55f * t,
                1.0f);

            RTBEngine::ECS::CreateSwarmEntity(*world, nullptr, transform, motion, swarmColor);
            ++spawnedCount;
        }

        PublishEcsInstances();
        return;
    }

    RTBEngine::Rendering::Mesh* cubeMesh =
        resources.LoadModel(RTBEngine::Core::ResourceManager::DEFAULT_CUBE_PATH);
    if (!cubeMesh) {
        RTB_ERROR("[PerfSwarm] Failed to load default cube mesh");
        return;
    }

    const float radiusSpan = spawnRadiusMax - spawnRadiusMin;
    for (int i = 0; i < agentCount; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(std::max(1, agentCount));
        const float phase = t * kTwoPi;
        const float radius = spawnRadiusMin + radiusSpan * (0.15f + 0.85f * t);

        RTBEngine::Scene::GameObject* agentObject = sceneManager.Instantiate("SwarmAgent");
        if (!agentObject) {
            continue;
        }

        agentObject->GetTransform().SetScale(
            RTBEngine::Math::Vector3(agentScale, agentScale, agentScale));

        auto* meshRenderer = new RTBEngine::Scene::MeshRenderer();
        meshRenderer->SetMesh(cubeMesh);
        meshRenderer->colorRef = RTBEngine::Math::Vector4(
            0.25f + 0.75f * t,
            0.35f + 0.40f * (1.0f - t),
            0.90f - 0.55f * t,
            1.0f);
        agentObject->AddComponent(meshRenderer);
        meshRenderer->SetUpdateTickEnabled(false);

        auto* agent = new PerfSwarmAgent();
        agent->useEcs = false;
        agent->phase = phase;
        agent->angularSpeed = 0.55f + 1.75f * std::fmod(t * 7.3f, 1.0f);
        agent->orbitRadius = radius;
        agent->bobAmplitude = 0.35f + 0.90f * std::fmod(t * 3.7f, 1.0f);
        agent->bobSpeed = 1.2f + 2.4f * std::fmod(t * 5.1f, 1.0f);
        agent->height = swarmCenter.y + 1.2f;
        agent->center = swarmCenter;
        agentObject->AddComponent(agent);

        ++spawnedCount;
    }
}

void PerfSwarmBenchmark::RefreshStatus(float deltaTime)
{
    if (deltaTime > 0.0f) {
        const float instantFps = 1.0f / deltaTime;
        smoothedFps = (smoothedFps <= 0.0f)
            ? instantFps
            : (smoothedFps * 0.90f + instantFps * 0.10f);
    }

    statusTimer += deltaTime;
    if (statusTimer < statusRefreshSeconds) {
        return;
    }
    statusTimer = 0.0f;

    double ecsSimMs = 0.0;
    std::uint32_t ecsEntityCount = 0;
    std::size_t instanceCount = 0;
    if (RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive()) {
        const RTBEngine::ECS::EcsSimulationStats& stats = world->GetSimulationStats();
        ecsSimMs = stats.lastSimulationMilliseconds;
        ecsEntityCount = stats.aliveEntityCount;
    }
    if (instanceMeshRenderer) {
        instanceCount = instanceMeshRenderer->GetInstanceCount();
    }

    const uint32_t drawCalls = RTBEngine::Scene::MeshRenderer::GetDrawCallCount();

    std::ostringstream stream;
    stream << (useEcs ? "MODE: ECS+MeshRenderer(inst)" : "MODE: OOP+MeshRenderer")
           << "\nAgents: " << spawnedCount
           << "\nFPS: " << static_cast<int>(smoothedFps + 0.5f)
           << "\nFrame: " << (deltaTime * 1000.0f) << " ms"
           << "\nDrawCalls: " << drawCalls;
    if (useEcs) {
        stream << "\nECS entities: " << ecsEntityCount
               << "\nInstances: " << instanceCount
               << "\nECS sim: " << ecsSimMs << " ms";
    }
    stream << "\nCompare with the other Test scene";

    if (statusText) {
        statusText->SetText(stream.str());
    }
}
