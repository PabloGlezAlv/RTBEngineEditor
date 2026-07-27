#include "LocalHostileHitNotify.h"

#include "CombatAuthority.h"
#include "PlayerSpecialAttackCharge.h"

#include <RTBEngine/Scene/GameObject.h>

namespace LocalHostileHitNotify {

void NotifySuccessfulHit(RTBEngine::Scene::GameObject* instigator)
{
    if (!instigator || !CombatAuthority::IsLocallyControlled(instigator)) {
        return;
    }

    if (auto* charge = instigator->GetComponent<PlayerSpecialAttackCharge>()) {
        charge->RegisterSuccessfulHit();
    }
}

}
