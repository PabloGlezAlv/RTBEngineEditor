#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

#include <cmath>

namespace EnemyMeleeAIDetail {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;
    constexpr float kDegToRad = kPi / 180.0f;
    constexpr float kDirectionEpsilon = 0.0001f;

    inline float ClampAngleDegrees(float angle)
    {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }

    inline float MoveTowardsAngleDegrees(float current, float target, float maxDelta)
    {
        const float delta = ClampAngleDegrees(target - current);
        if (std::abs(delta) <= maxDelta) {
            return target;
        }

        return current + (delta > 0.0f ? maxDelta : -maxDelta);
    }

    inline bool HasPlanarDirection(const RTBEngine::Math::Vector3& vector)
    {
        return vector.LengthSquared() > kDirectionEpsilon;
    }
}
