#pragma once

#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>
#include "../UI/Panels/EditorPanel.h"

namespace RTBEditor {

    class DDGIDebugRenderer {
    public:
        DDGIDebugRenderer();
        ~DDGIDebugRenderer();

        DDGIDebugRenderer(const DDGIDebugRenderer&) = delete;
        DDGIDebugRenderer& operator=(const DDGIDebugRenderer&) = delete;

        void Render(RTBEngine::Rendering::Camera* camera, const DDGIDebugSettings& settings);

    private:
        RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };

}
