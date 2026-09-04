#pragma once

#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

// Lightweight CC: freezes AI/player locomotion while remaining > 0.
class StunReceiver : public RTBEngine::Scene::Component {
public:
    StunReceiver();
    ~StunReceiver() override = default;

    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    void ApplyStun(float duration);
    void ClearStun();
    bool IsStunned() const { return remaining > 0.0f; }
    float GetRemaining() const { return remaining; }

    RTB_COMPONENT(StunReceiver)

private:
    float remaining = 0.0f;
};

StunReceiver* EnsureStunReceiver(RTBEngine::Scene::GameObject* owner);
void ApplyStunTo(RTBEngine::Scene::GameObject* owner, float duration);
