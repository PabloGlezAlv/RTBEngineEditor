#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class HealthComponent;

class PlayerHealthNetSync : public RTBEngine::ECS::Component
{
public:
    PlayerHealthNetSync() = default;
    ~PlayerHealthNetSync() override = default;

    void OnStart() override;
    void OnDestroy() override;

    RTB_COMPONENT(PlayerHealthNetSync)

private:
    HealthComponent* health = nullptr;
    RTBEngine::Core::EventSubscription healthChangedSubscription;

    void BindHealth();
    void UnbindHealth();
    void SyncHealth(float normalizedHealth) const;
};
