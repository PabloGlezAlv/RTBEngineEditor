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
    std::string walkAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx|Walking_A";
    std::string attackAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatMelee.fbx|Melee_2H_Attack_Chop";
    std::string deathAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Death_A";

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
