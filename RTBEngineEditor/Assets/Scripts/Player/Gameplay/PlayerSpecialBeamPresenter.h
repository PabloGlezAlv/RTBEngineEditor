#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Scene/TrailRenderer.h>

class EnergyBeamAsset;
class EnergyBeamComponent;

class PlayerSpecialBeamPresenter
{
public:
    EnergyBeamComponent* beam = nullptr;
    RTBEngine::Scene::TrailRenderer* previewTrail = nullptr;

    float defaultBeamRadius = 0.90f;
    float previewWidth = 1.85f;

    void Bind(EnergyBeamComponent* energyBeam, RTBEngine::Scene::TrailRenderer* preview);

    void ApplyDefaults();
    void ApplyFromAsset(const EnergyBeamAsset* asset);
    void ApplyWidths(float beamWidthScale, float previewWidthScale);

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
    void ConfigurePreviewTrail() const;
    void SetPreviewPolyline(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& end) const;
    void SetBeamEndpoints(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& end) const;
};
