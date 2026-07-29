#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <string>

namespace RTBEngine {
    namespace Rendering {
        class Texture;
    }
}

class PlayerSpecialBeamPresenter
{
public:
    RTBEngine::Scene::TrailRenderer* coreTrail = nullptr;
    RTBEngine::Scene::TrailRenderer* auraTrail = nullptr;
    RTBEngine::Scene::TrailRenderer* haloTrail = nullptr;
    RTBEngine::Scene::TrailRenderer* previewTrail = nullptr;

    float coreWidth = 0.35f;
    float auraWidth = 1.20f;
    float haloWidth = 2.20f;
    float previewWidth = 0.90f;

    RTBEngine::Math::Vector4 coreColor = RTBEngine::Math::Vector4(1.00f, 0.98f, 0.92f, 1.00f);
    RTBEngine::Math::Vector4 auraColor = RTBEngine::Math::Vector4(0.55f, 0.90f, 1.00f, 0.90f);
    RTBEngine::Math::Vector4 haloColor = RTBEngine::Math::Vector4(0.25f, 0.45f, 1.00f, 0.65f);
    RTBEngine::Math::Vector4 previewColor = RTBEngine::Math::Vector4(0.70f, 0.92f, 1.00f, 0.75f);

    std::string coreTexturePath = "Assets/Textures/VFX/Beam/BeamCore.png";
    std::string auraTexturePath = "Assets/Textures/VFX/Beam/BeamAura.png";
    std::string haloTexturePath = "Assets/Textures/VFX/Beam/BeamHalo.png";
    std::string previewTexturePath = "Assets/Textures/VFX/Beam/BeamPreview.png";

    void Bind(
        RTBEngine::Scene::TrailRenderer* core,
        RTBEngine::Scene::TrailRenderer* aura,
        RTBEngine::Scene::TrailRenderer* halo,
        RTBEngine::Scene::TrailRenderer* preview);

    void ApplyWidths(float beamWidthScale, float previewWidthScale);
    void EnsureTexturesLoaded();

    void ShowPreview(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& end);
    void HidePreview() const;

    void ShowBeam(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& end);
    void HideBeam() const;
    void HideAll() const;

private:
    RTBEngine::Rendering::Texture* coreTexture = nullptr;
    RTBEngine::Rendering::Texture* auraTexture = nullptr;
    RTBEngine::Rendering::Texture* haloTexture = nullptr;
    RTBEngine::Rendering::Texture* previewTexture = nullptr;
    bool texturesResolved = false;

    void ConfigureTrail(
        RTBEngine::Scene::TrailRenderer* trail,
        float width,
        const RTBEngine::Math::Vector4& color,
        RTBEngine::Rendering::Texture* texture,
        float softEdge,
        float startWidthScale,
        float endWidthScale,
        float uvScrollSpeed,
        float laserCore = 0.0f,
        float laserNoise = 0.0f,
        RTBEngine::Scene::TrailBlendMode blendMode = RTBEngine::Scene::TrailBlendMode::Additive) const;

    void SetPolyline(
        RTBEngine::Scene::TrailRenderer* trail,
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& end) const;
};
