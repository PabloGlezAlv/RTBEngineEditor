#include "StunReceiver.h"

#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>

using ThisClass = StunReceiver;

RTB_REGISTER_COMPONENT(StunReceiver)
RTB_END_REGISTER(StunReceiver)

void StunReceiver::OnValidate()
{
    remaining = std::max(0.0f, remaining);
}

void StunReceiver::OnUpdate(float deltaTime)
{
    if (remaining <= 0.0f) {
        return;
    }

    remaining = std::max(0.0f, remaining - std::max(0.0f, deltaTime));
    if (remaining <= 0.0f) {
        SetUpdateTickEnabled(false);
    }
}

void StunReceiver::ApplyStun(float duration)
{
    remaining = std::max(remaining, std::max(0.0f, duration));
    if (remaining > 0.0f) {
        SetUpdateTickEnabled(true);
    }
}

void StunReceiver::ClearStun()
{
    remaining = 0.0f;
    SetUpdateTickEnabled(false);
}

StunReceiver* EnsureStunReceiver(RTBEngine::Scene::GameObject* owner)
{
    if (!owner) {
        return nullptr;
    }

    if (StunReceiver* existing = owner->GetComponent<StunReceiver>()) {
        return existing;
    }

    auto* stun = new StunReceiver();
    owner->AddComponent(stun);
    return stun;
}

void ApplyStunTo(RTBEngine::Scene::GameObject* owner, float duration)
{
    if (StunReceiver* stun = EnsureStunReceiver(owner)) {
        stun->ApplyStun(duration);
    }
}
