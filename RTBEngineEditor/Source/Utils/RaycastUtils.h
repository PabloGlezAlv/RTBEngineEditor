#pragma once
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Rendering/Camera.h>

namespace RTBEditor {

    struct Ray {
        RTBEngine::Math::Vector3 origin;
        RTBEngine::Math::Vector3 direction;

        // Create a ray from screen coordinates into 3D world space
        static Ray ScreenPointToRay(
            const RTBEngine::Math::Vector2& screenPos,
            const RTBEngine::Math::Vector2& viewportSize,
            RTBEngine::Rendering::Camera* camera
        );

        // Test intersection with Axis-Aligned Bounding Box
        bool IntersectsAABB(
            const RTBEngine::Math::Vector3& aabbMin,
            const RTBEngine::Math::Vector3& aabbMax,
            float& outDistance
        ) const;
    };

}
