#include "SwarmSimulation.h"

#include <RTBEngine/ECS/Components/LocalTransform.h>
#include <RTBEngine/ECS/Components/VisualLink.h>
#include <RTBEngine/ECS/World.h>
#include <RTBEngine/Scene/GameObject.h>

#include <cmath>

namespace RTBEngine {
    namespace ECS {

        namespace {
            constexpr float kTwoPi = 6.28318530718f;

            void TickSwarmMotionSystem(World& world, float deltaTime)
            {
                if (deltaTime <= 0.0f) {
                    return;
                }

                world.ForEach<LocalTransform, SwarmMotion>(
                    [&](Entity entity, LocalTransform& transform, SwarmMotion& motion) {
                        motion.phase += motion.angularSpeed * deltaTime;
                        if (motion.phase > kTwoPi) {
                            motion.phase -= kTwoPi;
                        }

                        const float cosP = std::cos(motion.phase);
                        const float sinP = std::sin(motion.phase);
                        transform.position.x = motion.center.x + cosP * motion.orbitRadius;
                        transform.position.z = motion.center.z + sinP * motion.orbitRadius;
                        transform.position.y = motion.height
                            + std::sin(motion.phase * motion.bobSpeed) * motion.bobAmplitude;
                        transform.fixedHeight = transform.position.y;

                        if (VisualLink* link = world.TryGet<VisualLink>(entity)) {
                            if (link->visual) {
                                link->visual->GetTransform().SetPosition(transform.position);
                            }
                        }
                    });
            }
        }

        void RegisterSwarmSystems(World& world)
        {
            static bool registered = false;
            if (registered) {
                return;
            }
            registered = true;

            world.GetScheduler().Register(
                SystemPhase::Simulation,
                [](World& activeWorld, float deltaTime) {
                    TickSwarmMotionSystem(activeWorld, deltaTime);
                });
        }

        Entity CreateSwarmEntity(World& world,
                                 Scene::GameObject* visual,
                                 const LocalTransform& transform,
                                 const SwarmMotion& motion,
                                 const SwarmColor& color)
        {
            Entity entity = world.Create();
            world.Add<LocalTransform>(entity, transform);
            world.Add<SwarmMotion>(entity, motion);
            world.Add<SwarmColor>(entity, color);
            if (visual) {
                world.Add<VisualLink>(entity, VisualLink{ visual });
            }
            return entity;
        }

        void DestroySwarmEntity(World& world, Entity entity)
        {
            if (entity.IsValid()) {
                world.Destroy(entity);
            }
        }

    }
}
