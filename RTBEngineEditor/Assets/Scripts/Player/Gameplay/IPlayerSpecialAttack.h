#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

class IPlayerSpecialAttack {
public:
    virtual ~IPlayerSpecialAttack() = default;

    // aimStrength: 0..1 from special joystick pull (1 = full range).
    virtual bool TryActivate(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength = 1.0f) = 0;
    virtual bool IsActive() const = 0;
    virtual void UpdateAimPreview(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength = 1.0f) = 0;
    virtual void HideAimPreview() = 0;
    virtual void ApplyMovementLock(float deltaTime) = 0;
};
