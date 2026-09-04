#include "CombatAuthority.h"

#include "CharacterBase.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>

namespace CombatAuthority {
    namespace {
        const RTBEngine::Scene::NetworkIdentity* ResolveNetworkIdentity(
            RTBEngine::Scene::GameObject* gameObject)
        {
            if (const CharacterBase* character = gameObject->GetComponent<CharacterBase>()) {
                return character->GetNetworkIdentity();
            }

            return gameObject->GetComponent<RTBEngine::Scene::NetworkIdentity>();
        }
    }

    bool IsLocallyControlled(RTBEngine::Scene::GameObject* gameObject)
    {
        if (!gameObject) {
            return false;
        }

        if (const CharacterBase* character = gameObject->GetComponent<CharacterBase>()) {
            return character->IsLocallyControlled();
        }

        const RTBEngine::Scene::NetworkIdentity* identity =
            gameObject->GetComponent<RTBEngine::Scene::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        return identity->IsLocallyControlled();
    }

    bool HasSimulationAuthority(RTBEngine::Scene::GameObject* instigator)
    {
        if (!instigator) {
            return false;
        }

        if (const CharacterBase* character = instigator->GetComponent<CharacterBase>()) {
            return character->HasSimulationAuthority();
        }

        const RTBEngine::Scene::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::Scene::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        return identity->IsSimulatedByHost();
    }

    bool CanApplyDamage(RTBEngine::Scene::GameObject* instigator)
    {
        return HasSimulationAuthority(instigator);
    }

    bool CanConsumeAmmo(RTBEngine::Scene::GameObject* instigator)
    {
        return IsLocallyControlled(instigator);
    }

    bool ShouldBroadcastSpawn(RTBEngine::Scene::GameObject* instigator)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
            !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return false;
        }

        if (!instigator) {
            return false;
        }

        const RTBEngine::Scene::NetworkIdentity* identity = ResolveNetworkIdentity(instigator);
        return identity && identity->networkPlayerSlot >= 0;
    }

    bool ShouldProjectileApplyDamage()
    {
        return !RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
            RTBEngine::Online::OnlineGameplayNet::IsLobbyHost();
    }

}
