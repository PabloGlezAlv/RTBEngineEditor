#pragma once

#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>
#include "../UI/Panels/EditorPanel.h"

namespace RTBEditor {

    class NavGridDebugRenderer {
    public:
        NavGridDebugRenderer();
        ~NavGridDebugRenderer();

        NavGridDebugRenderer(const NavGridDebugRenderer&) = delete;
        NavGridDebugRenderer& operator=(const NavGridDebugRenderer&) = delete;

        void Render(RTBEngine::Rendering::Camera* camera,
                    RTBEngine::Scene::Scene* scene,
                    RTBEngine::Scene::GameObject* selectedObject,
                    const NavDebugSettings& settings);

    private:
        RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };

}
