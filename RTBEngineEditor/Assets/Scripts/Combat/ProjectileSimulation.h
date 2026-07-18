#pragma once

#include "ProjectileComponents.h"

#include <RTBEngine/ECS/Components/LocalTransform.h>
#include <RTBEngine/ECS/Entity.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }

    namespace ECS {

        class World;

        void RegisterProjectileSystems(World& world);

        Entity CreateProjectileEntity(World& world,
                                      Scene::GameObject* visual,
                                      const LocalTransform& transform,
                                      const ProjectileFlight& flight,
                                      const ProjectilePhysicsContext& physicsContext);

        void DestroyProjectileEntity(World& world, Entity entity);

    }
}
