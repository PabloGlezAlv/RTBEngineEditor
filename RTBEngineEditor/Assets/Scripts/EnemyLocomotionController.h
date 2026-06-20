#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class EnemyLocomotionController : public RTBEngine::ECS::Component
{
public:
    EnemyLocomotionController() = default;
    ~EnemyLocomotionController() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    float moveSpeed = 2.6f;
    float turnSpeed = 540.0f;
    float knockbackImpulse = 3.5f;

    RTB_COMPONENT(EnemyLocomotionController)

public:
    void MoveTowards(const RTBEngine::Math::Vector3& targetDirection, float deltaTime);
    void StopPlanarMotion() const;
    void ApplyKnockback(const RTBEngine::Math::Vector3& hitDirection,
                        const RTBEngine::Math::Vector3& fallbackDirection);

private:
    void ClampSettings();
    void ConfigurePhysicsBody() const;
};
