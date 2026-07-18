#pragma once

#include <RTBEngine/ECS/Entity.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class PerfSwarmAgent : public RTBEngine::Scene::Component
{
public:
    bool useEcs = false;
    float phase = 0.0f;
    float angularSpeed = 1.2f;
    float orbitRadius = 6.0f;
    float bobAmplitude = 0.75f;
    float bobSpeed = 2.4f;
    float height = 1.5f;
    RTBEngine::Math::Vector3 center = RTBEngine::Math::Vector3::Zero();

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

    RTB_COMPONENT(PerfSwarmAgent)

private:
    RTBEngine::ECS::Entity ecsEntity = RTBEngine::ECS::kNullEntity;

    void CreateEcsEntity();
    void DestroyEcsEntity();
    void IntegrateOop(float deltaTime);
};
