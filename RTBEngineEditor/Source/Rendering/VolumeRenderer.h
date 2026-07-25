#pragma once

#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Math/Matrix/Matrix4.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>

namespace RTBEngine {
    namespace Scene {
        class Scene;
    }
}

namespace RTBEditor {

    class VolumeRenderer {
    public:
        VolumeRenderer();
        ~VolumeRenderer();

        VolumeRenderer(const VolumeRenderer&) = delete;
        VolumeRenderer& operator=(const VolumeRenderer&) = delete;

        void RenderSceneVolumes(RTBEngine::Rendering::Camera* camera, RTBEngine::Scene::Scene* scene);
        void RenderSelection(RTBEngine::Rendering::Camera* camera, RTBEngine::Scene::GameObject* selectedObject);

    private:
        void RenderBox(RTBEngine::Rendering::Camera* camera,
                       const RTBEngine::Math::Matrix4& model,
                       const RTBEngine::Math::Vector4& color);

        RTBEngine::Rendering::Shader* lineShader = nullptr;
        RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
    };

}
