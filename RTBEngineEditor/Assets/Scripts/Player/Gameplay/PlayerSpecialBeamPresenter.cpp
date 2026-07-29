#include "PlayerSpecialBeamPresenter.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/Texture.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

namespace {
    constexpr float kBeamVisualLift = 0.85f;

    RTBEngine::Rendering::Texture* LoadBeamTexture(const std::string& path)
    {
        if (path.empty()) {
            return nullptr;
        }

        RTBEngine::Rendering::Texture* texture =
            RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(path, false);
        if (!texture) {
            RTB_WARN("[PlayerSpecialBeamPresenter] Failed to load beam texture '" + path + "'.");
        }
        return texture;
    }

    RTBEngine::Math::Vector3 Lift(const RTBEngine::Math::Vector3& point)
    {
        return RTBEngine::Math::Vector3(point.x, point.y + kBeamVisualLift, point.z);
    }

    void AgentLog(const char* hypothesisId, const char* location, const char* message, const std::string& dataJson)
    {
        // #region agent log
        try {
            std::ofstream out(R"(c:\Users\pablo\Desktop\Proyectos\RTBEngine\debug-a93520.log)", std::ios::app);
            if (!out.is_open()) {
                return;
            }
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            out << "{\"sessionId\":\"a93520\",\"runId\":\"laser-vfx\",\"hypothesisId\":\"" << hypothesisId
                << "\",\"location\":\"" << location << "\",\"message\":\"" << message
                << "\",\"data\":" << dataJson << ",\"timestamp\":" << ms << "}\n";
        } catch (...) {
        }
        // #endregion
    }
}

void PlayerSpecialBeamPresenter::Bind(
    RTBEngine::Scene::TrailRenderer* core,
    RTBEngine::Scene::TrailRenderer* aura,
    RTBEngine::Scene::TrailRenderer* halo,
    RTBEngine::Scene::TrailRenderer* preview)
{
    coreTrail = core;
    auraTrail = aura;
    haloTrail = halo;
    previewTrail = preview;
}

void PlayerSpecialBeamPresenter::ApplyWidths(float beamWidthScale, float previewWidthScale)
{
    const float beamScale = std::max(0.05f, beamWidthScale);
    const float previewScale = std::max(0.05f, previewWidthScale);
    const float normalizedBeam = beamScale / 1.60f;
    const float normalizedPreview = previewScale / 1.10f;

    // Straight constant-width nested layers (no cone).
    coreWidth = 1.80f * normalizedBeam;
    auraWidth = 2.40f * normalizedBeam;
    haloWidth = 3.20f * normalizedBeam;
    previewWidth = 1.85f * normalizedPreview;
}

void PlayerSpecialBeamPresenter::EnsureTexturesLoaded()
{
    if (texturesResolved) {
        return;
    }

    coreTexture = LoadBeamTexture(coreTexturePath);
    auraTexture = LoadBeamTexture(auraTexturePath);
    haloTexture = LoadBeamTexture(haloTexturePath);
    previewTexture = LoadBeamTexture(previewTexturePath);
    texturesResolved = true;
}

void PlayerSpecialBeamPresenter::ConfigureTrail(
    RTBEngine::Scene::TrailRenderer* trail,
    float width,
    const RTBEngine::Math::Vector4& color,
    RTBEngine::Rendering::Texture* texture,
    float softEdge,
    float startWidthScale,
    float endWidthScale,
    float uvScrollSpeed,
    float laserCore,
    float laserNoise,
    RTBEngine::Scene::TrailBlendMode blendMode) const
{
    if (!trail) {
        return;
    }

    const float safeWidth = std::max(0.08f, width);
    trail->width = safeWidth;
    trail->startWidth = safeWidth * startWidthScale;
    trail->endWidth = safeWidth * endWidthScale;
    trail->color = color;
    trail->fadeAlphaAlongLength = false;
    trail->blendMode = blendMode;
    trail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
    trail->softEdge = softEdge;
    trail->uvScrollSpeed = uvScrollSpeed;
    trail->uvTilesPerMeter = (texture != nullptr) ? 0.22f : 0.35f;
    trail->texture = texture;
    trail->laserCore = laserCore;
    trail->laserNoise = laserNoise;
    trail->SetGlobalAlphaScale(1.0f);
    trail->SetEnabled(true);
}

void PlayerSpecialBeamPresenter::SetPolyline(
    RTBEngine::Scene::TrailRenderer* trail,
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& end) const
{
    if (!trail) {
        return;
    }

    const RTBEngine::Math::Vector3 start = Lift(origin);
    const RTBEngine::Math::Vector3 tip = Lift(end);
    const RTBEngine::Math::Vector3 points[] = { start, tip };
    trail->SetPoints(points, 2);
    trail->SetVisible(true);
    trail->SetEnabled(true);

    if (RTBEngine::Scene::GameObject* trailOwner = trail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

void PlayerSpecialBeamPresenter::ShowPreview(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& end)
{
    // Same look as Attack Aim Trail (white Alpha ribbon), just wider.
    ConfigureTrail(
        previewTrail,
        std::max(previewWidth, 1.85f),
        RTBEngine::Math::Vector4(1.00f, 1.00f, 1.00f, 0.54f),
        nullptr,
        0.0f,
        1.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        RTBEngine::Scene::TrailBlendMode::Alpha);
    SetPolyline(previewTrail, origin, end);

    // #region agent log
    {
        static int s_n = 0;
        if ((++s_n % 40) == 1 && previewTrail) {
            std::ostringstream data;
            data << "{\"blend\":" << static_cast<int>(previewTrail->blendMode)
                 << ",\"hasTex\":" << (previewTrail->texture ? "true" : "false")
                 << ",\"laserCore\":" << previewTrail->laserCore
                 << ",\"softEdge\":" << previewTrail->softEdge
                 << ",\"colorR\":" << previewTrail->color.x
                 << ",\"colorA\":" << previewTrail->color.w
                 << ",\"width\":" << previewTrail->width << "}";
            AgentLog("N", "PlayerSpecialBeamPresenter.cpp:ShowPreview", "aim_preview", data.str());
        }
    }
    // #endregion
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
    EnsureTexturesLoaded();

    // Straight constant-width laser for now (no muzzle→tip taper / cone).
    ConfigureTrail(
        haloTrail,
        std::max(haloWidth, 3.20f),
        RTBEngine::Math::Vector4(0.25f, 0.60f, 1.00f, 0.40f),
        haloTexture,
        0.45f,
        1.0f,
        1.0f,
        0.4f,
        0.0f,
        0.0f,
        RTBEngine::Scene::TrailBlendMode::Additive);
    ConfigureTrail(
        auraTrail,
        std::max(auraWidth, 2.40f),
        RTBEngine::Math::Vector4(0.55f, 0.92f, 1.00f, 0.70f),
        auraTexture,
        0.50f,
        1.0f,
        1.0f,
        0.8f,
        0.0f,
        0.0f,
        RTBEngine::Scene::TrailBlendMode::Additive);
    ConfigureTrail(
        coreTrail,
        std::max(coreWidth, 1.80f),
        RTBEngine::Math::Vector4(1.00f, 0.98f, 0.95f, 0.95f),
        coreTexture,
        0.55f,
        1.0f,
        1.0f,
        1.2f,
        0.0f,
        0.0f,
        RTBEngine::Scene::TrailBlendMode::Additive);

    SetPolyline(haloTrail, origin, end);
    SetPolyline(auraTrail, origin, end);
    SetPolyline(coreTrail, origin, end);

    // #region agent log
    {
        static int s_n = 0;
        if ((++s_n % 40) == 1 && coreTrail) {
            std::ostringstream data;
            data << "{\"blend\":" << static_cast<int>(coreTrail->blendMode)
                 << ",\"hasTex\":" << (coreTrail->texture ? "true" : "false")
                 << ",\"laserCore\":" << coreTrail->laserCore
                 << ",\"width\":" << coreTrail->width
                 << ",\"startW\":" << coreTrail->startWidth
                 << ",\"endW\":" << coreTrail->endWidth
                 << ",\"auraW\":" << (auraTrail ? auraTrail->width : 0.0f)
                 << ",\"haloW\":" << (haloTrail ? haloTrail->width : 0.0f)
                 << ",\"straight\":true}";
            AgentLog("P", "PlayerSpecialBeamPresenter.cpp:ShowBeam", "attack_beam", data.str());
        }
    }
    // #endregion
}

void PlayerSpecialBeamPresenter::HideBeam() const
{
    auto hide = [](RTBEngine::Scene::TrailRenderer* trail) {
        if (!trail) {
            return;
        }
        trail->SetVisible(false);
        trail->ClearPoints();
    };

    hide(coreTrail);
    hide(auraTrail);
    hide(haloTrail);
}

void PlayerSpecialBeamPresenter::HideAll() const
{
    HidePreview();
    HideBeam();
}
