#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

namespace RTBEngine {
    namespace Animation {
        class Animator;
    }

    namespace Scene {
        class GameObject;
    }
}

class PlayerLocomotionAnimator
{
public:
    void Bind(RTBEngine::Animation::Animator* targetAnimator);
    void ResetReplicatedMotionSample();

    void UpdateLocomotion(bool inLocomotionState, bool hasMovementInput, bool isRunning);
    void SampleReplicatedMotion(
        RTBEngine::Scene::GameObject* owner,
        float deltaTime,
        bool inLocomotionState,
        float baseMoveSpeed,
        float sprintMultiplier);
    void ForceStartLocomotionAnimation(const RTBEngine::Scene::GameObject* owner);

private:
    RTBEngine::Animation::Animator* animator = nullptr;
    RTBEngine::Math::Vector3 lastReplicatedWorldPosition = RTBEngine::Math::Vector3::Zero();
    bool hasReplicatedMotionSample = false;
    float replicatedPlanarSpeed = 0.0f;
};
