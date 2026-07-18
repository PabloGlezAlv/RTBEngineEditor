#include "PerfSwarmAgent.h"
#include "SwarmComponents.h"
#include "SwarmSimulation.h"

#include <RTBEngine/ECS/Components/LocalTransform.h>
#include <RTBEngine/ECS/World.h>
#include <RTBEngine/Scene/GameObject.h>

#include <cmath>

using ThisClass = PerfSwarmAgent;

namespace {
    constexpr float kTwoPi = 6.28318530718f;
}

RTB_REGISTER_COMPONENT(PerfSwarmAgent)
    RTB_PROPERTY(useEcs)
    RTB_PROPERTY_RANGE(phase, 0.0f, 6.28318530718f)
    RTB_PROPERTY_RANGE(angularSpeed, 0.05f, 20.0f)
    RTB_PROPERTY_RANGE(orbitRadius, 0.1f, 50.0f)
    RTB_PROPERTY_RANGE(bobAmplitude, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(bobSpeed, 0.05f, 20.0f)
    RTB_PROPERTY_RANGE(height, -10.0f, 30.0f)
RTB_END_REGISTER(PerfSwarmAgent)

void PerfSwarmAgent::OnStart()
{
    if (useEcs) {
        CreateEcsEntity();
    }
}

void PerfSwarmAgent::OnUpdate(float deltaTime)
{
    if (useEcs && ecsEntity.IsValid()) {
        return;
    }

    IntegrateOop(deltaTime);
}

void PerfSwarmAgent::OnDestroy()
{
    DestroyEcsEntity();
}

void PerfSwarmAgent::CreateEcsEntity()
{
    DestroyEcsEntity();

    RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
    if (!world || !owner) {
        return;
    }

    RTBEngine::ECS::LocalTransform transform;
    transform.position = owner->GetWorldPosition();
    transform.fixedHeight = transform.position.y;

    RTBEngine::ECS::SwarmMotion motion;
    motion.center = center;
    motion.phase = phase;
    motion.angularSpeed = angularSpeed;
    motion.orbitRadius = orbitRadius;
    motion.bobAmplitude = bobAmplitude;
    motion.bobSpeed = bobSpeed;
    motion.height = height;

    ecsEntity = RTBEngine::ECS::CreateSwarmEntity(*world, owner, transform, motion);
    SetUpdateTickEnabled(false);
}

void PerfSwarmAgent::DestroyEcsEntity()
{
    if (!ecsEntity.IsValid()) {
        return;
    }

    if (RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive()) {
        RTBEngine::ECS::DestroySwarmEntity(*world, ecsEntity);
    }
    ecsEntity = RTBEngine::ECS::kNullEntity;
}

void PerfSwarmAgent::IntegrateOop(float deltaTime)
{
    if (!owner || deltaTime <= 0.0f) {
        return;
    }

    phase += angularSpeed * deltaTime;
    if (phase > kTwoPi) {
        phase -= kTwoPi;
    }

    const float cosP = std::cos(phase);
    const float sinP = std::sin(phase);
    RTBEngine::Math::Vector3 position;
    position.x = center.x + cosP * orbitRadius;
    position.z = center.z + sinP * orbitRadius;
    position.y = height + std::sin(phase * bobSpeed) * bobAmplitude;
    owner->GetTransform().SetPosition(position);
}
