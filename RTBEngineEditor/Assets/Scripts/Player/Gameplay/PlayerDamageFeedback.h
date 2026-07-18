#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace Scene {
        class AudioSourceComponent;
    }
}

class PlayerDamageFeedback : public RTBEngine::Scene::Component
{
public:
    PlayerDamageFeedback() = default;
    ~PlayerDamageFeedback() override = default;

    HealthComponent* health = nullptr;
    std::string hurtSoundPath = "Assets/Audio/Combat/player_grunt.mp3";
    float soundCooldown = 0.12f;

    RTB_COMPONENT(PlayerDamageFeedback)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

private:
    RTBEngine::Scene::AudioSourceComponent* hurtAudio = nullptr;
    RTBEngine::Core::EventSubscription damageSubscription;
    float soundCooldownRemaining = 0.0f;

    bool IsLocallyControlled() const;
    void ResolveHurtAudio();
    void RebindDamageSubscription();
    void UnbindDamageSubscription();
    void HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData);
    void ApplyKnockback(const HealthComponent::DamageContext& damage);
    void PlayHurtSound();
};
