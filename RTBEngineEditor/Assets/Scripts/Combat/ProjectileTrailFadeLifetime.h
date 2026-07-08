#pragma once

#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/IPoolable.h>

namespace RTBEngine {
    namespace ECS {
        class TrailRenderer;
    }
}

class ProjectileTrailFadeLifetime : public RTBEngine::ECS::Component, public RTBEngine::ECS::IPoolable
{
public:
    ProjectileTrailFadeLifetime() = default;
    ~ProjectileTrailFadeLifetime() override = default;

    float fadeDuration = 0.55f;
    float headTrimInterval = 0.028f;

    RTB_COMPONENT(ProjectileTrailFadeLifetime)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    void OnPoolAcquire() override;
    void OnPoolRelease() override;

    void BeginFade();

private:
    RTBEngine::ECS::TrailRenderer* trailRenderer = nullptr;
    float elapsed = 0.0f;
    float headTrimTimer = 0.0f;

    void Finish();
};
