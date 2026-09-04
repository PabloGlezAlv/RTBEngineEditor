#include "LightFlicker.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/LightComponent.h>

#include <cmath>
#include <cstdlib>

using ThisClass = LightFlicker;

RTB_REGISTER_COMPONENT(LightFlicker)
    RTB_PROPERTY(baseIntensity)
    RTB_PROPERTY(amplitude)
    RTB_PROPERTY(speed)
RTB_END_REGISTER(LightFlicker)

void LightFlicker::OnStart()
{
    phaseOffset = static_cast<float>(std::rand() % 1000) * 0.01f;
    CacheBaseIntensity();
}

void LightFlicker::OnValidate()
{
    CacheBaseIntensity();
}

void LightFlicker::OnUpdate(float deltaTime)
{
    ApplyFlicker(deltaTime);
}

void LightFlicker::CacheBaseIntensity()
{
    lightComponent = owner->GetComponent<RTBEngine::Scene::LightComponent>();
    if (lightComponent) {
        baseIntensity = lightComponent->intensity;
    }
}

void LightFlicker::ApplyFlicker(float deltaTime)
{
    if (!lightComponent) {
        CacheBaseIntensity();
        if (!lightComponent) {
            return;
        }
    }

    flickerTime += deltaTime * speed;
    const float waveA = std::sin(flickerTime * 3.7f + phaseOffset);
    const float waveB = std::sin(flickerTime * 7.3f + phaseOffset * 1.7f);
    const float flicker = 1.0f + amplitude * (waveA * 0.6f + waveB * 0.4f);

    lightComponent->intensity = baseIntensity * flicker;
    lightComponent->SyncProperties();
}
