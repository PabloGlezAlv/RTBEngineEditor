#pragma once
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>

namespace RTBEngine {
    namespace Scene {
        class SphereColliderComponent;
        class CapsuleColliderComponent;
    }
}

namespace RTBEditor {

    class ColliderRenderer {
    public:
        ColliderRenderer();
        ~ColliderRenderer();

        ColliderRenderer(const ColliderRenderer&) = delete;
        ColliderRenderer& operator=(const ColliderRenderer&) = delete;

        void RenderSelection(RTBEngine::Rendering::Camera* camera,
                             RTBEngine::Scene::GameObject* selectedObject);
        void RenderDebugQueries(RTBEngine::Rendering::Camera* camera);

    private:
        RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;

        RTBEngine::Rendering::RHI::GpuId sphereVao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId sphereVbo = RTBEngine::Rendering::RHI::kInvalidGpuId;

        RTBEngine::Rendering::RHI::GpuId capsuleVao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId capsuleVbo = RTBEngine::Rendering::RHI::kInvalidGpuId;

        RTBEngine::Rendering::RHI::GpuId debugQueryVao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId debugQueryVbo = RTBEngine::Rendering::RHI::kInvalidGpuId;

        RTBEngine::Rendering::Shader* lineShader = nullptr;

        void RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                   RTBEngine::Scene::GameObject* object,
                                   RTBEngine::Scene::SphereColliderComponent* collider);
        void RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                   const RTBEngine::Math::Vector3& center,
                                   float radius,
                                   const RTBEngine::Math::Vector4& color);
        void RenderCapsuleWireframe(RTBEngine::Rendering::Camera* camera,
                                    RTBEngine::Scene::GameObject* object,
                                    RTBEngine::Scene::CapsuleColliderComponent* collider);
    };

}
