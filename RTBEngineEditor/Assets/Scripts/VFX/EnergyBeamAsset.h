#pragma once

#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class EnergyBeamComponent;

class EnergyBeamAsset : public RTBEngine::Data::DataAsset {
public:
    float radius = 0.90f;
    float coreWidthScale = 0.55f;
    float innerWidthScale = 1.00f;
    float outerWidthScale = 1.65f;

    RTBEngine::Math::Vector4 beamColor = RTBEngine::Math::Vector4(0.35f, 0.85f, 1.00f, 0.85f);
    RTBEngine::Math::Vector4 coreColor = RTBEngine::Math::Vector4(1.00f, 0.98f, 0.94f, 1.00f);
    float emissionStrength = 2.5f;
    float noiseScale = 3.5f;
    float noiseSpeed = 2.2f;
    float distortionStrength = 0.15f;
    float fresnelPower = 2.5f;
    float glowIntensity = 1.4f;
    float pulseSpeed = 4.0f;
    float pulseAmount = 0.18f;
    float softEdge = 0.85f;
    float uvScrollSpeed = 2.5f;
    float uvTilesPerMeter = 0.35f;
    bool useVerticalCross = true;

    float duration = 0.0f;
    float length = 8.0f;

    void ApplyTo(EnergyBeamComponent* beam) const;

    RTB_DATA_ASSET(EnergyBeamAsset)
};
