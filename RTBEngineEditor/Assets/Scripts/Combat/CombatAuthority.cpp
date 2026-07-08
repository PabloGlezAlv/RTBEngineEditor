#include "CombatAuthority.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>

namespace CombatAuthority {

    bool IsLocallyControlled(RTBEngine::ECS::GameObject* gameObject)
    {
        if (!gameObject) {
            return false;
        }

        const RTBEngine::ECS::NetworkIdentity* identity =
            gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        return identity->IsLocallyControlled();
    }

    bool HasSimulationAuthority(RTBEngine::ECS::GameObject* instigator)
    {
        if (!instigator) {
            return false;
        }

        const RTBEngine::ECS::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
            return identity->IsSimulatedByHost();
        }

        return true;
    }

    bool CanApplyDamage(RTBEngine::ECS::GameObject* instigator)
    {
        return HasSimulationAuthority(instigator);
    }

    bool CanConsumeAmmo(RTBEngine::ECS::GameObject* instigator)
    {
        return IsLocallyControlled(instigator);
    }

    bool ShouldBroadcastSpawn(RTBEngine::ECS::GameObject* instigator)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
            !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return false;
        }

        if (!instigator) {
            return false;
        }

        const RTBEngine::ECS::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        return identity && identity->networkPlayerSlot >= 0;
    }

    bool ShouldProjectileApplyDamage()
    {
        return !RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
            RTBEngine::Online::OnlineGameplayNet::IsLobbyHost();
    }

}
