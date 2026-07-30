#include "EnergyBeamAsset.h"
#include "EnergyBeamComponent.h"

using ThisClass = EnergyBeamAsset;

RTB_REGISTER_DATA_ASSET(EnergyBeamAsset)
    RTB_PROPERTY_RANGE(radius, 0.01f, 8.0f)
    RTB_PROPERTY_RANGE(coreWidthScale, 0.05f, 2.0f)
    RTB_PROPERTY_RANGE(innerWidthScale, 0.1f, 3.0f)
    RTB_PROPERTY_RANGE(outerWidthScale, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(taperAmount, 0.0f, 0.95f)
    RTB_PROPERTY_RANGE(tipCapScale, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(muzzleFlareScale, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(fadeOutDuration, 0.0f, 2.0f)
    RTB_PROPERTY_RANGE(tipBurstCount, 0, 128)
    RTB_PROPERTY_COLOR(beamColor)
    RTB_PROPERTY_COLOR(coreColor)
    RTB_PROPERTY_RANGE(emissionStrength, 0.0f, 16.0f)
    RTB_PROPERTY_RANGE(noiseScale, 0.1f, 20.0f)
    RTB_PROPERTY_RANGE(noiseSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(distortionStrength, 0.0f, 2.0f)
    RTB_PROPERTY_RANGE(fresnelPower, 0.1f, 8.0f)
    RTB_PROPERTY_RANGE(glowIntensity, 0.0f, 8.0f)
    RTB_PROPERTY_RANGE(pulseSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(pulseAmount, 0.0f, 1.0f)
    RTB_PROPERTY_RANGE(softEdge, 0.05f, 4.0f)
    RTB_PROPERTY_RANGE(uvScrollSpeed, -20.0f, 20.0f)
    RTB_PROPERTY_RANGE(uvTilesPerMeter, 0.0f, 10.0f)
    RTB_PROPERTY(useVerticalCross)
    RTB_PROPERTY_RANGE(duration, 0.0f, 60.0f)
    RTB_PROPERTY_RANGE(length, 0.1f, 40.0f)
RTB_END_REGISTER_DATA_ASSET(EnergyBeamAsset)

void EnergyBeamAsset::ApplyTo(EnergyBeamComponent* beam) const
{
    if (!beam) {
        return;
    }

    beam->radius = radius;
    beam->coreWidthScale = coreWidthScale;
    beam->innerWidthScale = innerWidthScale;
    beam->outerWidthScale = outerWidthScale;
    beam->taperAmount = taperAmount;
    beam->tipCapScale = tipCapScale;
    beam->muzzleFlareScale = muzzleFlareScale;
    beam->fadeOutDuration = fadeOutDuration;
    beam->tipBurstCount = tipBurstCount;
    beam->beamColor = beamColor;
    beam->coreColor = coreColor;
    beam->emissionStrength = emissionStrength;
    beam->noiseScale = noiseScale;
    beam->noiseSpeed = noiseSpeed;
    beam->distortionStrength = distortionStrength;
    beam->fresnelPower = fresnelPower;
    beam->glowIntensity = glowIntensity;
    beam->pulseSpeed = pulseSpeed;
    beam->pulseAmount = pulseAmount;
    beam->softEdge = softEdge;
    beam->uvScrollSpeed = uvScrollSpeed;
    beam->uvTilesPerMeter = uvTilesPerMeter;
    beam->useVerticalCross = useVerticalCross;
    beam->duration = duration;
    beam->length = length;
}
