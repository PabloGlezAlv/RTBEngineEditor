#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <vector>

namespace RTBEngine {
    namespace Scene {
        class ParticleSystem;
    }
}

class ImpactEffect : public RTBEngine::Scene::Component {
public:
    ImpactEffect() = default;
    ~ImpactEffect() override = default;

    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    void PlayAt(const RTBEngine::Math::Vector3& position);

    float lifetime = 2.0f;
    std::vector<RTBEngine::Scene::ParticleSystem*> particleSystems;

    RTB_COMPONENT(ImpactEffect)

private:
    void StopEffects();
    void FinishPlayback();

    bool playing = false;
    float elapsed = 0.0f;
};
