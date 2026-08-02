#include "PlayerSpecialAttackUtil.h"

#include "PlayerSpecialBeamAttack.h"
#include "PlayerSpecialBouncingBallAttack.h"
#include "PlayerSpecialLeapAttack.h"

#include <RTBEngine/Scene/GameObject.h>

IPlayerSpecialAttack* ResolvePlayerSpecialAttack(RTBEngine::Scene::GameObject* owner)
{
    if (!owner) {
        return nullptr;
    }

    if (PlayerSpecialLeapAttack* leap = owner->GetComponent<PlayerSpecialLeapAttack>()) {
        return leap;
    }

    if (PlayerSpecialBouncingBallAttack* bouncingBall =
            owner->GetComponent<PlayerSpecialBouncingBallAttack>()) {
        return bouncingBall;
    }

    if (PlayerSpecialBeamAttack* beam = owner->GetComponent<PlayerSpecialBeamAttack>()) {
        return beam;
    }

    return nullptr;
}
