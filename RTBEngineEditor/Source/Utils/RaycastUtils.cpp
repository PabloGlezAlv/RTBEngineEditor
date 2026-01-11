#include "RaycastUtils.h"
#include <algorithm>
#include <limits>

namespace RTBEditor {

    Ray Ray::ScreenPointToRay(
        const RTBEngine::Math::Vector2& screenPos,
        const RTBEngine::Math::Vector2& viewportSize,
        RTBEngine::Rendering::Camera* camera
    ) {
        // Convert screen coordinates to Normalized Device Coordinates (NDC)
        // Screen: (0, 0) at top-left, (width, height) at bottom-right
        // NDC: (-1, -1) at bottom-left, (1, 1) at top-right
        float ndcX = (2.0f * screenPos.x) / viewportSize.x - 1.0f;
        float ndcY = 1.0f - (2.0f * screenPos.y) / viewportSize.y;  // Flip Y

        // Create ray in clip space
        RTBEngine::Math::Vector4 rayClip(ndcX, ndcY, -1.0f, 1.0f);

        // Transform to view space
        RTBEngine::Math::Matrix4 invProjection = camera->GetProjectionMatrix().Inverse();
        RTBEngine::Math::Vector4 rayView = invProjection * rayClip;
        rayView.z = -1.0f;  // Forward in view space
        rayView.w = 0.0f;   // Direction vector

        // Transform to world space
        RTBEngine::Math::Matrix4 invView = camera->GetViewMatrix().Inverse();
        RTBEngine::Math::Vector4 rayWorld4 = invView * rayView;
        RTBEngine::Math::Vector3 rayWorld(rayWorld4.x, rayWorld4.y, rayWorld4.z);

        // Normalize direction
        float length = sqrtf(rayWorld.x * rayWorld.x + rayWorld.y * rayWorld.y + rayWorld.z * rayWorld.z);
        if (length > 0.0001f) {
            rayWorld.x /= length;
            rayWorld.y /= length;
            rayWorld.z /= length;
        }

        Ray ray;
        ray.origin = camera->GetPosition();
        ray.direction = rayWorld;
        return ray;
    }

    bool Ray::IntersectsAABB(
        const RTBEngine::Math::Vector3& aabbMin,
        const RTBEngine::Math::Vector3& aabbMax,
        float& outDistance
    ) const {
        // Slab method for ray-AABB intersection
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::max();

        // Test X slab
        if (fabs(direction.x) > 0.0001f) {
            float t1 = (aabbMin.x - origin.x) / direction.x;
            float t2 = (aabbMax.x - origin.x) / direction.x;
            tMin = std::max(tMin, std::min(t1, t2));
            tMax = std::min(tMax, std::max(t1, t2));
        } else {
            // Ray parallel to X slab
            if (origin.x < aabbMin.x || origin.x > aabbMax.x) {
                return false;
            }
        }

        // Test Y slab
        if (fabs(direction.y) > 0.0001f) {
            float t1 = (aabbMin.y - origin.y) / direction.y;
            float t2 = (aabbMax.y - origin.y) / direction.y;
            tMin = std::max(tMin, std::min(t1, t2));
            tMax = std::min(tMax, std::max(t1, t2));
        } else {
            // Ray parallel to Y slab
            if (origin.y < aabbMin.y || origin.y > aabbMax.y) {
                return false;
            }
        }

        // Test Z slab
        if (fabs(direction.z) > 0.0001f) {
            float t1 = (aabbMin.z - origin.z) / direction.z;
            float t2 = (aabbMax.z - origin.z) / direction.z;
            tMin = std::max(tMin, std::min(t1, t2));
            tMax = std::min(tMax, std::max(t1, t2));
        } else {
            // Ray parallel to Z slab
            if (origin.z < aabbMin.z || origin.z > aabbMax.z) {
                return false;
            }
        }

        // Check if slabs overlap
        if (tMax < tMin || tMax < 0.0f) {
            return false;
        }

        outDistance = tMin >= 0.0f ? tMin : tMax;
        return true;
    }

}
