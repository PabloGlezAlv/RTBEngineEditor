#pragma once

#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <memory>

namespace RTBEditor {

    // Scene-view helper: shows where directional / spot lights are aiming.
    class LightGizmoRenderer {
    public:
        LightGizmoRenderer();
        ~LightGizmoRenderer();

        LightGizmoRenderer(const LightGizmoRenderer&) = delete;
        LightGizmoRenderer& operator=(const LightGizmoRenderer&) = delete;

        void Render(RTBEngine::Rendering::Camera* camera,
                    RTBEngine::Scene::Scene* scene,
                    RTBEngine::Scene::GameObject* selectedObject);

    private:
        RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };

}
