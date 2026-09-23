#include "LocalHostileHitNotify.h"

#include "CombatAuthority.h"
#include "OnlineGameNetMessages.h"
#include "PlayerSpecialAttackCharge.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>

namespace LocalHostileHitNotify {

void NotifySuccessfulHit(RTBEngine::Scene::GameObject* instigator)
{
    if (!instigator) {
        return;
    }

    auto* charge = instigator->GetComponent<PlayerSpecialAttackCharge>();
    if (!charge) {
        return;
    }

    const bool online = RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby();
    if (online && !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        return;
    }

    if (!online && !CombatAuthority::IsLocallyControlled(instigator)) {
        return;
    }

    if (!charge->RegisterAuthoritativeHit()) {
        return;
    }

    if (!online || CombatAuthority::IsLocallyControlled(instigator)) {
        return;
    }

    const RTBEngine::Scene::NetworkIdentity* identity =
        instigator->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    if (!identity || identity->networkPlayerSlot < 0) {
        return;
    }

    GameNet::OnlineGameNetSubsystem::BroadcastSpecialCharge(
        identity->networkPlayerSlot,
        charge->GetCurrentHits());
}

}
