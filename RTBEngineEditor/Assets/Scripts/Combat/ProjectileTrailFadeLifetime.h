#pragma once

#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
    }
}

class ProjectileTrailFadeLifetime : public RTBEngine::Scene::Component
{
public:
    ProjectileTrailFadeLifetime() = default;
    ~ProjectileTrailFadeLifetime() override = default;

    float fadeDuration = 0.55f;
    float headTrimInterval = 0.028f;

    void OnEnable() override;
    void OnDisable() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void BeginFade();

    RTB_COMPONENT(ProjectileTrailFadeLifetime)

private:
    RTBEngine::Scene::TrailRenderer* trailRenderer = nullptr;
    float elapsed = 0.0f;
    float headTrimTimer = 0.0f;

    void Finish();
};
