#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace Animation {
        class Animator;
    }
}

class EnemyAnimationDriver : public RTBEngine::Scene::Component
{
public:
    EnemyAnimationDriver() = default;
    ~EnemyAnimationDriver() override = default;

    void OnStart() override;
    void OnValidate() override;

    RTBEngine::Animation::Animator* animator = nullptr;

    RTB_COMPONENT(EnemyAnimationDriver)

public:
    void PlayWalkLoop();
    bool PlayAttack();
    bool PlayDeath();
    bool HasAttackAnimation() const;
    bool HasDeathAnimation() const;
    bool IsAttackPlaying() const;
    bool IsDeathPlaying() const;
    void HoldDeathPose();

private:
    void ResolveAnimator();
};
