#include "EditorGridRenderer.h"
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <vector>

namespace RTBEditor {

    struct LineVertex {
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Vector4 color;
    };

    namespace {
        using namespace RTBEngine::Rendering::RHI;

        void SetupLineVertexArray(IRenderDevice& device, GpuId vao, GpuId vbo,
                                  const void* data, std::size_t size, BufferUsage usage)
        {
            device.BindVertexArray(vao);
            device.SetArrayBufferData(vbo, data, size, usage);
            device.EnableVertexAttribFloat(0, 3, static_cast<int>(sizeof(LineVertex)), 0);
            device.EnableVertexAttribFloat(
                1, 4, static_cast<int>(sizeof(LineVertex)), offsetof(LineVertex, color));
            device.UnbindVertexArray();
        }
    }

    EditorGridRenderer::EditorGridRenderer() {
        gridVertexCount = 0;
        axesVertexCount = 0;

        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Default/Shaders/EditorLine.vert",
            "Default/Shaders/EditorLine.frag"
        );

        CreateGridMesh();
        CreateAxesMesh();
    }

    EditorGridRenderer::~EditorGridRenderer() {
        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        if (gridVAO != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(gridVAO);
        }
        if (gridVBO != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(gridVBO);
        }
        if (axesVAO != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(axesVAO);
        }
        if (axesVBO != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(axesVBO);
        }
    }

    void EditorGridRenderer::CreateGridMesh() {
        std::vector<LineVertex> vertices;
        RTBEngine::Math::Vector4 gridColor(0.3f, 0.3f, 0.3f, 0.5f);

        float halfSize = gridSize / 2.0f;
        int lineCount = (int)(gridSize / gridSpacing);

        for (int i = 0; i <= lineCount; i++) {
            float pos = -halfSize + i * gridSpacing;

            vertices.push_back({ RTBEngine::Math::Vector3(pos, 0.0f, -halfSize), gridColor });
            vertices.push_back({ RTBEngine::Math::Vector3(pos, 0.0f, halfSize), gridColor });

            vertices.push_back({ RTBEngine::Math::Vector3(-halfSize, 0.0f, pos), gridColor });
            vertices.push_back({ RTBEngine::Math::Vector3(halfSize, 0.0f, pos), gridColor });
        }

        gridVertexCount = (int)vertices.size();

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        gridVAO = device.CreateVertexArray();
        gridVBO = device.CreateBuffer();

        SetupLineVertexArray(
            device,
            gridVAO,
            gridVBO,
            vertices.data(),
            vertices.size() * sizeof(LineVertex),
            RTBEngine::Rendering::RHI::BufferUsage::Static);
    }

    void EditorGridRenderer::CreateAxesMesh() {
        std::vector<LineVertex> vertices;

        RTBEngine::Math::Vector4 red(1.0f, 0.0f, 0.0f, 1.0f);
        RTBEngine::Math::Vector4 green(0.0f, 1.0f, 0.0f, 1.0f);
        RTBEngine::Math::Vector4 blue(0.0f, 0.0f, 1.0f, 1.0f);

        vertices.push_back({ RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f), red });
        vertices.push_back({ RTBEngine::Math::Vector3(axisLength, 0.0f, 0.0f), red });

        vertices.push_back({ RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f), green });
        vertices.push_back({ RTBEngine::Math::Vector3(0.0f, axisLength, 0.0f), green });

        vertices.push_back({ RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f), blue });
        vertices.push_back({ RTBEngine::Math::Vector3(0.0f, 0.0f, axisLength), blue });

        axesVertexCount = (int)vertices.size();

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        axesVAO = device.CreateVertexArray();
        axesVBO = device.CreateBuffer();

        SetupLineVertexArray(
            device,
            axesVAO,
            axesVBO,
            vertices.data(),
            vertices.size() * sizeof(LineVertex),
            RTBEngine::Rendering::RHI::BufferUsage::Static);
    }

    void EditorGridRenderer::Render(RTBEngine::Rendering::Camera* camera) {
        if (!lineShader || !camera) return;

        lineShader->Bind();

        RTBEngine::Math::Vector3 camPos = camera->GetPosition();
        float gridX = floor(camPos.x / gridSpacing) * gridSpacing;
        float gridZ = floor(camPos.z / gridSpacing) * gridSpacing;

        RTBEngine::Math::Matrix4 gridTransform = RTBEngine::Math::Matrix4::Translate(
            RTBEngine::Math::Vector3(gridX, 0.0f, gridZ)
        );

        RTBEngine::Math::Matrix4 viewProjection = camera->GetViewProjectionMatrix();
        RTBEngine::Math::Matrix4 mvp = viewProjection * gridTransform;
        lineShader->SetMatrix4("uViewProjection", mvp);

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        device.SetBlend(true);
        device.SetBlendFuncSeparate(
            RTBEngine::Rendering::RHI::BlendFactor::SrcAlpha,
            RTBEngine::Rendering::RHI::BlendFactor::OneMinusSrcAlpha,
            RTBEngine::Rendering::RHI::BlendFactor::SrcAlpha,
            RTBEngine::Rendering::RHI::BlendFactor::OneMinusSrcAlpha);
        device.SetDepthTest(true);
        device.SetDepthWrite(false);

        if (gridVAO != RTBEngine::Rendering::RHI::kInvalidGpuId && gridVertexCount > 0) {
            device.BindVertexArray(gridVAO);
            device.DrawArrays(RTBEngine::Rendering::RHI::PrimitiveTopology::Lines, 0, gridVertexCount);
            device.UnbindVertexArray();
        }

        device.SetDepthWrite(true);

        lineShader->SetMatrix4("uViewProjection", viewProjection);

        if (axesVAO != RTBEngine::Rendering::RHI::kInvalidGpuId && axesVertexCount > 0) {
            device.BindVertexArray(axesVAO);
            device.DrawArrays(RTBEngine::Rendering::RHI::PrimitiveTopology::Lines, 0, axesVertexCount);
            device.UnbindVertexArray();
        }

        device.SetBlend(false);

        lineShader->Unbind();
    }

}
