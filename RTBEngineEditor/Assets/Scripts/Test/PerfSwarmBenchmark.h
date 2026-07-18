#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

#include <string>

namespace RTBEngine {
    namespace UI {
        class UIText;
    }
    namespace Scene {
        class MeshRenderer;
    }
}

class PerfSwarmBenchmark : public RTBEngine::Scene::Component
{
public:
    bool useEcs = false;
    int agentCount = 1500;
    float spawnRadiusMin = 2.0f;
    float spawnRadiusMax = 14.0f;
    float agentScale = 0.12f;
    float statusRefreshSeconds = 0.35f;
    RTBEngine::Math::Vector3 swarmCenter = RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f);
    std::string statusTextObjectName = "PerfStatusText";

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnValidate() override;

    RTB_COMPONENT(PerfSwarmBenchmark)

private:
    RTBEngine::UI::UIText* statusText = nullptr;
    RTBEngine::Scene::MeshRenderer* instanceMeshRenderer = nullptr;
    float statusTimer = 0.0f;
    float smoothedFps = 0.0f;
    int spawnedCount = 0;

    void SpawnSwarm();
    void ResolveStatusText();
    void RefreshStatus(float deltaTime);
    void EnsureInstanceMeshRenderer();
    void PublishEcsInstances();
};
