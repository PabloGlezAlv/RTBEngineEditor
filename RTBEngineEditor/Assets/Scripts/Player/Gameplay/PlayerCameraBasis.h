#pragma once

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

// Single source of truth for the fixed orbit camera. Camera placement, the WASD basis and
// the aim basis must all derive from here, or input stops matching what the player sees.
namespace PlayerCameraBasis {

    constexpr float kYawDegrees = 0.0f;
    constexpr float kPitchDegrees = 50.0f;

    inline RTBEngine::Math::Quaternion GetOrbitRotation()
    {
        constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
        return RTBEngine::Math::Quaternion::FromEulerAngles(
            kPitchDegrees * kDegToRad,
            kYawDegrees * kDegToRad,
            0.0f);
    }

    // Planar basis taken from the fixed orbit, not from the live camera world matrix: the
    // child camera inherits pawn yaw between FixedUpdate and LateUpdate and causes stutter.
    inline void GetPlanarBasis(
        RTBEngine::Math::Vector3& outForward,
        RTBEngine::Math::Vector3& outRight)
    {
        constexpr float kEpsilon = 0.0001f;

        outForward = GetOrbitRotation() * RTBEngine::Math::Vector3::Forward();
        outForward.y = 0.0f;
        if (outForward.LengthSquared() <= kEpsilon) {
            outForward = RTBEngine::Math::Vector3::Forward();
        } else {
            outForward.Normalize();
        }

        outRight = outForward.Cross(RTBEngine::Math::Vector3::Up());
        if (outRight.LengthSquared() <= kEpsilon) {
            outRight = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
        } else {
            outRight.Normalize();
        }
    }

}
