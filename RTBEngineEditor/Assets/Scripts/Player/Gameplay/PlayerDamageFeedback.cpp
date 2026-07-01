#include "PlayerDamageFeedback.h"

#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Audio/AudioClip.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Physics/RigidBody.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerDamageFeedback;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }
}

RTB_REGISTER_COMPONENT(PlayerDamageFeedback)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_ASSET_PATH(hurtSoundPath, "audio")
    RTB_PROPERTY_RANGE(soundCooldown, 0.0f, 2.0f)
RTB_END_REGISTER(PlayerDamageFeedback)

void PlayerDamageFeedback::OnStart()
{
    if (!health && owner) {
        health = owner->GetComponent<HealthComponent>();
    }

    ResolveHurtAudio();
    RebindDamageSubscription();
}

void PlayerDamageFeedback::OnUpdate(float deltaTime)
{
    if (soundCooldownRemaining > 0.0f) {
        soundCooldownRemaining = std::max(0.0f, soundCooldownRemaining - std::max(0.0f, deltaTime));
    }
}

void PlayerDamageFeedback::OnDestroy()
{
    UnbindDamageSubscription();
}

bool PlayerDamageFeedback::IsLocallyControlled() const
{
    if (!owner) {
        return true;
    }

    const RTBEngine::ECS::NetworkIdentity* identity = owner->GetComponent<RTBEngine::ECS::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    return identity->IsLocallyControlled();
}

void PlayerDamageFeedback::ResolveHurtAudio()
{
    if (hurtAudio || !owner) {
        return;
    }

    hurtAudio = owner->GetComponent<RTBEngine::ECS::AudioSourceComponent>();
}

void PlayerDamageFeedback::RebindDamageSubscription()
{
    UnbindDamageSubscription();

    if (!health) {
        return;
    }

    damageSubscription = health->SubscribeToDamageTaken(
        [this](const HealthComponent::DamageTakenEvent& eventData) {
            HandleDamageTaken(eventData);
        });
}

void PlayerDamageFeedback::UnbindDamageSubscription()
{
    damageSubscription.Reset();
}

void PlayerDamageFeedback::HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData)
{
    if (!owner || eventData.damage.amount <= 0.0f || !IsLocallyControlled()) {
        return;
    }

    ApplyKnockback(eventData.damage);
    PlayHurtSound();
}

void PlayerDamageFeedback::ApplyKnockback(const HealthComponent::DamageContext& damage)
{
    if (damage.knockbackStrength <= 0.0f || !owner) {
        return;
    }

    auto* rigidBodyComponent = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (!rigidBodyComponent || !rigidBodyComponent->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rigidBodyComponent->GetRigidBody();
    if (!rigidBody || rigidBody->GetType() != RTBEngine::Physics::RigidBodyType::Dynamic) {
        return;
    }

    RTBEngine::Math::Vector3 planarDirection = damage.hitDirection;
    planarDirection.y = 0.0f;
    if (!HasPlanarDirection(planarDirection) && damage.instigator) {
        planarDirection = owner->GetWorldPosition() - damage.instigator->GetWorldPosition();
        planarDirection.y = 0.0f;
    }

    if (!HasPlanarDirection(planarDirection)) {
        planarDirection = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Back();
        planarDirection.y = 0.0f;
    }

    if (!HasPlanarDirection(planarDirection)) {
        return;
    }

    planarDirection.Normalize();

    if (auto* controller = owner->GetComponent<ThirdPersonCharacterController>()) {
        controller->AddPlanarKnockback(planarDirection, damage.knockbackStrength);
        return;
    }

    rigidBody->ApplyCentralImpulse(btVector3(
        planarDirection.x * damage.knockbackStrength,
        0.0f,
        planarDirection.z * damage.knockbackStrength));
}

void PlayerDamageFeedback::PlayHurtSound()
{
    if (soundCooldownRemaining > 0.0f || hurtSoundPath.empty()) {
        return;
    }

    ResolveHurtAudio();
    if (!hurtAudio) {
        return;
    }

    RTBEngine::Audio::AudioClip* clip =
        RTBEngine::Core::ResourceManager::GetInstance().LoadAudioClip(hurtSoundPath);
    if (!clip) {
        return;
    }

    hurtAudio->SetClip(clip);
    hurtAudio->Play();
    soundCooldownRemaining = std::max(0.0f, soundCooldown);
}
