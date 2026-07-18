#pragma once
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>

namespace RTBEditor {
    class EditorGridRenderer {
    public:
        EditorGridRenderer();
        ~EditorGridRenderer();

        void Render(RTBEngine::Rendering::Camera* camera);

        void SetGridSize(float size) { gridSize = size; }
        void SetGridSpacing(float spacing) { gridSpacing = spacing; }
        void SetAxisLength(float length) { axisLength = length; }

    private:
        void CreateGridMesh();
        void CreateAxesMesh();

        RTBEngine::Rendering::RHI::GpuId gridVAO = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId gridVBO = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId axesVAO = RTBEngine::Rendering::RHI::kInvalidGpuId;
        RTBEngine::Rendering::RHI::GpuId axesVBO = RTBEngine::Rendering::RHI::kInvalidGpuId;
        int gridVertexCount;
        int axesVertexCount;

        float gridSize = 100.0f;
        float gridSpacing = 1.0f;
        float axisLength = 10.0f;

        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };
}
