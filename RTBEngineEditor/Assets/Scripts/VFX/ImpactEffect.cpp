#include "ImpactEffect.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ParticleSystem.h>

#include <algorithm>

using ThisClass = ImpactEffect;

RTB_REGISTER_COMPONENT(ImpactEffect)
    RTB_PROPERTY_RANGE(lifetime, 0.1f, 30.0f)
    RTB_PROPERTY_COMPONENT_LIST(particleSystems, ParticleSystem)
RTB_END_REGISTER(ImpactEffect)

void ImpactEffect::OnValidate()
{
    lifetime = std::max(0.1f, lifetime);
}

void ImpactEffect::PlayAt(const RTBEngine::Math::Vector3& position)
{
    if (owner) {
        owner->GetTransform().SetPosition(position);
        owner->SetActive(true);
    }

    SetEnabled(true);
    playing = true;
    elapsed = 0.0f;
    SetUpdateTickEnabled(true);

    for (RTBEngine::Scene::ParticleSystem* particles : particleSystems) {
        if (!particles) {
            continue;
        }
        particles->SetEnabled(true);
        particles->Restart();
    }
}

void ImpactEffect::OnUpdate(float deltaTime)
{
    if (!playing) {
        return;
    }

    elapsed += std::max(0.0f, deltaTime);
    if (elapsed < lifetime) {
        return;
    }

    FinishPlayback();
}

void ImpactEffect::StopEffects()
{
    for (RTBEngine::Scene::ParticleSystem* particles : particleSystems) {
        if (!particles) {
            continue;
        }
        particles->Stop();
    }
}

void ImpactEffect::FinishPlayback()
{
    playing = false;
    elapsed = 0.0f;
    StopEffects();
    SetUpdateTickEnabled(false);
    SetEnabled(false);

    if (owner) {
        owner->SetActive(false);
    }
}
