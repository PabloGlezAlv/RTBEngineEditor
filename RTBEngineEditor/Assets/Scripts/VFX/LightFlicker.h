#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace ECS {
        class LightComponent;
    }
}

class LightFlicker : public RTBEngine::ECS::Component {
public:
    LightFlicker() = default;
    ~LightFlicker() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    float baseIntensity = 2.5f;
    float amplitude = 0.20f;
    float speed = 4.0f;

    RTB_COMPONENT(LightFlicker)

private:
    void CacheBaseIntensity();
    void ApplyFlicker(float deltaTime);

    RTBEngine::ECS::LightComponent* lightComponent = nullptr;
    float flickerTime = 0.0f;
    float phaseOffset = 0.0f;
};
