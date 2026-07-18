#pragma once

#include "SwarmComponents.h"

#include <RTBEngine/ECS/Entity.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }

    namespace ECS {

        class World;
        struct LocalTransform;

        void RegisterSwarmSystems(World& world);

        Entity CreateSwarmEntity(World& world,
                                 Scene::GameObject* visual,
                                 const LocalTransform& transform,
                                 const SwarmMotion& motion,
                                 const SwarmColor& color = SwarmColor{});

        void DestroySwarmEntity(World& world, Entity entity);

    }
}
