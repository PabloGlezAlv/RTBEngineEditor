#pragma once

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

namespace CombatAuthority {

    bool IsLocallyControlled(RTBEngine::ECS::GameObject* gameObject);
    bool HasSimulationAuthority(RTBEngine::ECS::GameObject* instigator);
    bool CanApplyDamage(RTBEngine::ECS::GameObject* instigator);
    bool CanConsumeAmmo(RTBEngine::ECS::GameObject* instigator);
    bool ShouldBroadcastSpawn(RTBEngine::ECS::GameObject* instigator);
    bool ShouldProjectileApplyDamage();

}
