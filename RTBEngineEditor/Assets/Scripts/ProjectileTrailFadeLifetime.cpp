#include "ProjectileTrailFadeLifetime.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <vector>

using ThisClass = ProjectileTrailFadeLifetime;

RTB_REGISTER_COMPONENT(ProjectileTrailFadeLifetime)
    RTB_PROPERTY_RANGE(fadeDuration, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(headTrimInterval, 0.01f, 0.2f)
RTB_END_REGISTER(ProjectileTrailFadeLifetime)

void ProjectileTrailFadeLifetime::OnStart()
{
    trailRenderer = owner ? owner->GetComponent<RTBEngine::ECS::TrailRenderer>() : nullptr;
}

void ProjectileTrailFadeLifetime::OnUpdate(float deltaTime)
{
    if (!trailRenderer || !owner) {
        Finish();
        return;
    }

    elapsed += std::max(0.0f, deltaTime);

    const float normalized = fadeDuration > 0.0f
        ? std::clamp(elapsed / fadeDuration, 0.0f, 1.0f)
        : 1.0f;
    const float easedAlpha = 1.0f - normalized * normalized;
    trailRenderer->SetGlobalAlphaScale(easedAlpha);

    headTrimTimer += deltaTime;
    while (headTrimTimer >= headTrimInterval && trailRenderer->GetPointCount() > 2) {
        headTrimTimer -= headTrimInterval;

        const std::vector<RTBEngine::Math::Vector3>& points = trailRenderer->GetPoints();
        trailRenderer->SetPoints(
            std::vector<RTBEngine::Math::Vector3>(points.begin(), points.end() - 1));
    }

    if (normalized >= 1.0f) {
        Finish();
    }
}

void ProjectileTrailFadeLifetime::Finish()
{
    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (scene && owner) {
        scene->RemoveGameObject(owner);
        return;
    }

    SetUpdateTickEnabled(false);
}
