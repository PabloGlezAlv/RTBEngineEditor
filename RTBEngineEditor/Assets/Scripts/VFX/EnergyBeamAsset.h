#pragma once

#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class EnergyBeamComponent;

class EnergyBeamAsset : public RTBEngine::Data::DataAsset {
public:
    float radius = 0.28f;
    float coreWidthScale = 0.40f;
    float innerWidthScale = 0.72f;
    float outerWidthScale = 1.10f;
    float taperAmount = 0.55f;
    float tipCapScale = 1.15f;
    float muzzleFlareScale = 1.75f;
    float fadeOutDuration = 0.22f;
    int tipBurstCount = 22;

    RTBEngine::Math::Vector4 beamColor = RTBEngine::Math::Vector4(0.25f, 0.78f, 1.00f, 0.55f);
    RTBEngine::Math::Vector4 coreColor = RTBEngine::Math::Vector4(0.95f, 0.98f, 1.00f, 0.95f);
    float emissionStrength = 1.35f;
    float noiseScale = 3.5f;
    float noiseSpeed = 2.2f;
    float distortionStrength = 0.12f;
    float fresnelPower = 2.2f;
    float glowIntensity = 0.85f;
    float pulseSpeed = 4.0f;
    float pulseAmount = 0.12f;
    float softEdge = 0.90f;
    float uvScrollSpeed = 2.5f;
    float uvTilesPerMeter = 0.45f;
    bool useVerticalCross = false;

    float duration = 0.0f;
    float length = 8.0f;

    void ApplyTo(EnergyBeamComponent* beam) const;

    RTB_DATA_ASSET(EnergyBeamAsset)
};
