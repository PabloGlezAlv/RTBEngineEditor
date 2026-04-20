#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

namespace RTBEngine {
    namespace Animation {
        class Animator;
    }
}

class EnemyAnimationDriver : public RTBEngine::ECS::Component
{
public:
    EnemyAnimationDriver() = default;
    ~EnemyAnimationDriver() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    RTBEngine::Animation::Animator* animator = nullptr;
    std::string walkAnimationFbx = "Assets/Models/walking.fbx";
    std::string attackAnimationFbx = "Assets/Models/attack.fbx";
    std::string deathAnimationFbx = "Assets/Models/dying.fbx";

    RTB_COMPONENT(EnemyAnimationDriver)

public:
    void PlayWalkLoop();
    bool PlayAttack();
    bool PlayDeath();
    bool HasAttackAnimation() const;
    bool HasDeathAnimation() const;
    bool IsAttackPlaying() const;
    bool IsDeathPlaying() const;

private:
    struct AnimationSlotState {
        std::string sourceFbx;
        bool ready = false;
    };

    RTBEngine::Animation::Animator* registeredAnimator = nullptr;
    bool missingAnimatorWarningShown = false;
    AnimationSlotState walkSlotState;
    AnimationSlotState attackSlotState;
    AnimationSlotState deathSlotState;

    void ResolveAnimator();
    void RegisterAnimationSlots();
    void RegisterAnimationSlot(const char* slotLabel,
                               const std::string& sourceFbx,
                               const char* alias,
                               AnimationSlotState& slotState);
};
