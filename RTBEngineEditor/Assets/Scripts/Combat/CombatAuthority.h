#pragma once

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

namespace CombatAuthority {

    bool IsLocallyControlled(RTBEngine::Scene::GameObject* gameObject);
    bool HasSimulationAuthority(RTBEngine::Scene::GameObject* instigator);
    bool CanApplyDamage(RTBEngine::Scene::GameObject* instigator);
    bool CanConsumeAmmo(RTBEngine::Scene::GameObject* instigator);
    bool ShouldBroadcastSpawn(RTBEngine::Scene::GameObject* instigator);
    bool ShouldProjectileApplyDamage();

}
