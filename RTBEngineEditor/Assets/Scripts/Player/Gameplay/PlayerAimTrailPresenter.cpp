#include "PlayerAimTrailPresenter.h"

#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>

void PlayerAimTrailPresenter::Bind(
    RTBEngine::Scene::TrailRenderer* trailRenderer,
    float trailForwardOffset,
    float trailHeightOffset,
    float clipRadius)
{
    trail = trailRenderer;
    forwardOffset = trailForwardOffset;
    heightOffset = trailHeightOffset;
    wallClipRadius = std::max(0.05f, clipRadius);
}

void PlayerAimTrailPresenter::Hide() const
{
    if (!trail) {
        return;
    }

    trail->SetVisible(false);
    trail->ClearPoints();
}

void PlayerAimTrailPresenter::ShowSegment(
    const RTBEngine::Math::Vector3& start,
    const RTBEngine::Math::Vector3& end) const
{
    if (!trail) {
        return;
    }

    const RTBEngine::Math::Vector3 points[] = { start, end };
    trail->SetPoints(points, 2);
    trail->SetVisible(true);
}

RTBEngine::Math::Vector3 PlayerAimTrailPresenter::ResolveVisualOrigin(
    RTBEngine::Scene::GameObject* owner,
    const RTBEngine::Math::Vector3& attackDirection) const
{
    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetFeetWorld(owner);
    origin = CharacterCombatOrigins::ApplyPlanarDirectionOffset(
        origin,
        attackDirection,
        forwardOffset);
    origin.y += heightOffset;
    return origin;
}

RTBEngine::Math::Vector3 PlayerAimTrailPresenter::ResolveCombatOrigin(
    RTBEngine::Scene::GameObject* owner,
    const RTBEngine::Math::Vector3& attackDirection) const
{
    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetCapsuleCenterWorld(owner);
    return CharacterCombatOrigins::ApplyPlanarDirectionOffset(
        origin,
        attackDirection,
        forwardOffset);
}

float PlayerAimTrailPresenter::ResolveClippedLength(
    RTBEngine::Scene::GameObject* owner,
    const RTBEngine::Math::Vector3& attackDirection,
    float maxLength) const
{
    if (!owner || maxLength <= 0.0f) {
        return 0.0f;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld =
        CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return maxLength;
    }

    CharacterCombatUtils::PlanarEnvironmentClipQuery clipQuery;
    clipQuery.physicsWorld = physicsWorld;
    clipQuery.instigator = owner;
    clipQuery.origin = ResolveCombatOrigin(owner, attackDirection);
    clipQuery.direction = attackDirection;
    clipQuery.maxLength = maxLength;
    clipQuery.castRadius = wallClipRadius;
    clipQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");
    return CharacterCombatUtils::ResolvePlanarEnvironmentClipLength(clipQuery);
}
