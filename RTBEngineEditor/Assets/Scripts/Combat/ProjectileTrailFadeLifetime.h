#pragma once

#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace ECS {
        class TrailRenderer;
    }
}

class ProjectileTrailFadeLifetime : public RTBEngine::ECS::Component
{
public:
    ProjectileTrailFadeLifetime() = default;
    ~ProjectileTrailFadeLifetime() override = default;

    float fadeDuration = 0.55f;
    float headTrimInterval = 0.028f;

    RTB_COMPONENT(ProjectileTrailFadeLifetime)

    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    RTBEngine::ECS::TrailRenderer* trailRenderer = nullptr;
    float elapsed = 0.0f;
    float headTrimTimer = 0.0f;

    void Finish();
};
