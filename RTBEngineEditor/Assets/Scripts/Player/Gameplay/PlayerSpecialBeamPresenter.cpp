#include "PlayerSpecialBeamPresenter.h"

#include "../../VFX/EnergyBeamAsset.h"
#include "../../VFX/EnergyBeamComponent.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>

namespace {
    constexpr float kBeamVisualLift = 0.85f;

    RTBEngine::Math::Vector3 Lift(const RTBEngine::Math::Vector3& point)
    {
        return RTBEngine::Math::Vector3(point.x, point.y + kBeamVisualLift, point.z);
    }
}

void PlayerSpecialBeamPresenter::Bind(
    EnergyBeamComponent* energyBeam,
    RTBEngine::Scene::TrailRenderer* preview)
{
    beam = energyBeam;
    previewTrail = preview;
}

void PlayerSpecialBeamPresenter::ApplyDefaults()
{
    if (!beam) {
        return;
    }

    beam->ApplyDefaults();
    defaultBeamRadius = beam->radius;
}

void PlayerSpecialBeamPresenter::ApplyFromAsset(const EnergyBeamAsset* asset)
{
    if (!beam) {
        return;
    }

    if (asset) {
        asset->ApplyTo(beam);
        defaultBeamRadius = beam->radius;
        return;
    }

    ApplyDefaults();
}

void PlayerSpecialBeamPresenter::ApplyWidths(float beamWidthScale, float previewWidthScale)
{
    const float beamScale = std::max(0.05f, beamWidthScale);
    const float previewScale = std::max(0.05f, previewWidthScale);
    const float normalizedBeam = beamScale / 1.60f;
    const float normalizedPreview = previewScale / 1.10f;

    if (beam) {
        beam->radius = std::max(0.05f, defaultBeamRadius * normalizedBeam);
    }

    previewWidth = std::max(0.08f, 1.85f * normalizedPreview);
}

void PlayerSpecialBeamPresenter::ConfigurePreviewTrail() const
{
    if (!previewTrail) {
        return;
    }

    const float safeWidth = std::max(previewWidth, 1.85f);
    previewTrail->width = safeWidth;
    previewTrail->startWidth = safeWidth;
    previewTrail->endWidth = safeWidth;
    previewTrail->color = RTBEngine::Math::Vector4(1.00f, 1.00f, 1.00f, 0.54f);
    previewTrail->fadeAlphaAlongLength = false;
    previewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
    previewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
    previewTrail->softEdge = 0.0f;
    previewTrail->uvScrollSpeed = 0.0f;
    previewTrail->uvTilesPerMeter = 0.35f;
    previewTrail->texture = nullptr;
    previewTrail->SetGlobalAlphaScale(1.0f);
    previewTrail->SetEnabled(true);
}

void PlayerSpecialBeamPresenter::SetPreviewPolyline(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& end) const
{
    if (!previewTrail) {
        return;
    }

    const RTBEngine::Math::Vector3 start = Lift(origin);
    const RTBEngine::Math::Vector3 tip = Lift(end);
    const RTBEngine::Math::Vector3 points[] = { start, tip };
    previewTrail->SetPoints(points, 2);
    previewTrail->SetVisible(true);
    previewTrail->SetEnabled(true);

    if (RTBEngine::Scene::GameObject* trailOwner = previewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

void PlayerSpecialBeamPresenter::SetBeamEndpoints(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& end) const
{
    if (!beam) {
        return;
    }

    const RTBEngine::Math::Vector3 start = Lift(origin);
    const RTBEngine::Math::Vector3 tip = Lift(end);
    beam->SetEndpoints(start, tip);
}

void PlayerSpecialBeamPresenter::ShowPreview(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& end)
{
    ConfigurePreviewTrail();
    SetPreviewPolyline(origin, end);
}

void PlayerSpecialBeamPresenter::HidePreview() const
{
    if (!previewTrail) {
        return;
    }

    previewTrail->SetVisible(false);
    previewTrail->ClearPoints();
}

void PlayerSpecialBeamPresenter::ShowBeam(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& end)
{
    if (!beam) {
        return;
    }

    if (RTBEngine::Scene::GameObject* beamOwner = beam->GetOwner()) {
        beamOwner->SetActive(true);
    }

    SetBeamEndpoints(origin, end);
    beam->visible = true;
    beam->duration = 0.0f;
    beam->Play();
}

void PlayerSpecialBeamPresenter::HideBeam() const
{
    if (!beam) {
        return;
    }

    // Soft end: tip burst + fade instead of hard cut.
    beam->BeginFadeOut();
}

void PlayerSpecialBeamPresenter::HideAll() const
{
    HidePreview();
    HideBeam();
}
