#include "ProjectileSimulation.h"

#include <RTBEngine/ECS/Components/VisualLink.h>
#include <RTBEngine/ECS/World.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>
#include <cmath>

namespace RTBEngine {
    namespace ECS {

        namespace {
            constexpr float kDirectionEpsilon = 0.0001f;
            constexpr float kDistanceEpsilon = 0.0001f;

            void TickProjectileFlightSystem(World& world, float deltaTime)
            {
                if (deltaTime <= 0.0f) {
                    return;
                }

                world.ForEach<LocalTransform, ProjectileFlight, ProjectilePhysicsContext>(
                    [&](Entity entity,
                        LocalTransform& transform,
                        ProjectileFlight& flight,
                        ProjectilePhysicsContext& physicsContext) {
                        if (flight.pendingDestroy) {
                            return;
                        }

                        const float remainingDistance = flight.maxDistance - flight.distanceTravelled;
                        if (remainingDistance <= kDistanceEpsilon) {
                            flight.pendingDestroy = true;
                            return;
                        }

                        const float stepDistance = std::min(flight.speed * deltaTime, remainingDistance);
                        if (stepDistance <= kDistanceEpsilon) {
                            return;
                        }

                        Math::Vector3 previousPosition = transform.position;
                        previousPosition.y = transform.fixedHeight;

                        Math::Vector3 nextPosition = previousPosition + flight.direction * stepDistance;
                        nextPosition.y = transform.fixedHeight;

                        ProjectilePendingHit* pendingHit = world.TryGet<ProjectilePendingHit>(entity);
                        if (!pendingHit) {
                            pendingHit = &world.Add<ProjectilePendingHit>(entity);
                        }
                        pendingHit->active = false;

                        bool shouldStop = false;
                        Math::Vector3 resolvedPosition = nextPosition;

                        if (physicsContext.physicsWorld) {
                            Physics::PhysicsQueryHit hit;
                            Physics::PhysicsQueryOptions options;
                            options.ignoredObject = physicsContext.instigator;
                            options.ignoreIgnoredObjectHierarchy = true;
                            options.ignoreTriggers = true;

                            if (physicsContext.physicsWorld->SphereCastClosest(
                                    previousPosition,
                                    nextPosition,
                                    flight.radius,
                                    hit,
                                    options)) {
                                const float hitFraction = std::clamp(hit.fraction, 0.0f, 1.0f);
                                resolvedPosition =
                                    previousPosition + (nextPosition - previousPosition) * hitFraction;
                                resolvedPosition.y = transform.fixedHeight;

                                pendingHit->active = true;
                                pendingHit->hitObject = hit.gameObject;
                                pendingHit->hitPoint = hit.point;
                                pendingHit->hitFraction = hitFraction;
                                shouldStop = true;
                            }
                        }

                        transform.position = shouldStop ? resolvedPosition : nextPosition;
                        transform.position.y = transform.fixedHeight;
                        flight.distanceTravelled =
                            std::min(flight.maxDistance, flight.distanceTravelled + stepDistance);
                        flight.shouldStop = shouldStop;

                        if (!shouldStop &&
                            flight.distanceTravelled + kDistanceEpsilon >= flight.maxDistance) {
                            flight.pendingDestroy = true;
                        }

                        (void)entity;
                    });
            }

            void TickProjectilePresentationSystem(World& world, float /*deltaTime*/)
            {
                world.ForEach<LocalTransform, VisualLink, ProjectileFlight>(
                    [](Entity /*entity*/,
                       LocalTransform& transform,
                       VisualLink& visualLink,
                       ProjectileFlight& /*flight*/) {
                        if (!visualLink.visual) {
                            return;
                        }

                        visualLink.visual->GetTransform().SetPosition(transform.position);
                    });
            }
        }

        void RegisterProjectileSystems(World& world)
        {
            static bool registered = false;
            if (registered) {
                return;
            }
            registered = true;

            world.GetScheduler().Register(
                SystemPhase::Simulation,
                [](World& activeWorld, float deltaTime) {
                    TickProjectileFlightSystem(activeWorld, deltaTime);
                });

            world.GetScheduler().Register(
                SystemPhase::Presentation,
                [](World& activeWorld, float deltaTime) {
                    TickProjectilePresentationSystem(activeWorld, deltaTime);
                });
        }

        Entity CreateProjectileEntity(World& world,
                                      Scene::GameObject* visual,
                                      const LocalTransform& transform,
                                      const ProjectileFlight& flight,
                                      const ProjectilePhysicsContext& physicsContext)
        {
            Entity entity = world.Create();
            world.Add<LocalTransform>(entity, transform);
            world.Add<ProjectileFlight>(entity, flight);
            world.Add<ProjectilePhysicsContext>(entity, physicsContext);
            world.Add<ProjectilePendingHit>(entity);
            world.Add<VisualLink>(entity, VisualLink{ visual });
            return entity;
        }

        void DestroyProjectileEntity(World& world, Entity entity)
        {
            if (!entity.IsValid()) {
                return;
            }

            world.Destroy(entity);
        }

    }
}
