#include "DDGIDebugRenderer.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Rendering/GI/DDGISystem.h>
#include <RTBEngine/Rendering/GI/GiTypes.h>
#include <RTBEngine/Rendering/Lighting/LightingProjectSettings.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>

#include <cstddef>
#include <vector>
#include <chrono>
#include <fstream>

namespace RTBEditor {
    namespace {

        struct LineVertex {
            RTBEngine::Math::Vector3 position;
            RTBEngine::Math::Vector4 color;
        };

        void AppendLine(std::vector<LineVertex>& vertices,
                        const RTBEngine::Math::Vector3& a,
                        const RTBEngine::Math::Vector3& b,
                        const RTBEngine::Math::Vector4& color)
        {
            vertices.push_back({ a, color });
            vertices.push_back({ b, color });
        }

        void AppendAxisAlignedBox(std::vector<LineVertex>& vertices,
                                  const RTBEngine::Math::Vector3& minCorner,
                                  const RTBEngine::Math::Vector3& maxCorner,
                                  const RTBEngine::Math::Vector4& color)
        {
            const RTBEngine::Math::Vector3 corners[8] = {
                RTBEngine::Math::Vector3(minCorner.x, minCorner.y, minCorner.z),
                RTBEngine::Math::Vector3(maxCorner.x, minCorner.y, minCorner.z),
                RTBEngine::Math::Vector3(maxCorner.x, minCorner.y, maxCorner.z),
                RTBEngine::Math::Vector3(minCorner.x, minCorner.y, maxCorner.z),
                RTBEngine::Math::Vector3(minCorner.x, maxCorner.y, minCorner.z),
                RTBEngine::Math::Vector3(maxCorner.x, maxCorner.y, minCorner.z),
                RTBEngine::Math::Vector3(maxCorner.x, maxCorner.y, maxCorner.z),
                RTBEngine::Math::Vector3(minCorner.x, maxCorner.y, maxCorner.z),
            };

            constexpr int edges[12][2] = {
                {0, 1}, {1, 2}, {2, 3}, {3, 0},
                {4, 5}, {5, 6}, {6, 7}, {7, 4},
                {0, 4}, {1, 5}, {2, 6}, {3, 7},
            };

            for (const auto& edge : edges) {
                AppendLine(vertices, corners[edge[0]], corners[edge[1]], color);
            }
        }

        void AppendProbeMarker(std::vector<LineVertex>& vertices,
                               const RTBEngine::Math::Vector3& center,
                               float radius,
                               const RTBEngine::Math::Vector4& color)
        {
            const RTBEngine::Math::Vector3 north(center.x, center.y, center.z + radius);
            const RTBEngine::Math::Vector3 south(center.x, center.y, center.z - radius);
            const RTBEngine::Math::Vector3 east(center.x + radius, center.y, center.z);
            const RTBEngine::Math::Vector3 west(center.x - radius, center.y, center.z);
            const RTBEngine::Math::Vector3 up(center.x, center.y + radius, center.z);
            const RTBEngine::Math::Vector3 down(center.x, center.y - radius, center.z);

            AppendLine(vertices, north, south, color);
            AppendLine(vertices, east, west, color);
            AppendLine(vertices, up, down, color);
        }

        RTBEngine::Rendering::GI::DDGISettings ResolveSettings()
        {
            if (const RTBEngine::Rendering::GI::DDGIVolume* volume =
                    RTBEngine::Rendering::GI::DDGISystem::GetInstance().GetActiveVolume()) {
                return volume->GetSettings();
            }

            return RTBEngine::Rendering::LightingProjectSettings::Get().GetDDGISettings();
        }

    }

    DDGIDebugRenderer::DDGIDebugRenderer()
    {
        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Default/Shaders/EditorLine.vert",
            "Default/Shaders/EditorLine.frag");

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        vao = device.CreateVertexArray();
        vbo = device.CreateBuffer();

        device.BindVertexArray(vao);
        device.SetArrayBufferData(vbo, nullptr, 0, RTBEngine::Rendering::RHI::BufferUsage::Dynamic);
        device.EnableVertexAttribFloat(0, 3, static_cast<int>(sizeof(LineVertex)), 0);
        device.EnableVertexAttribFloat(
            1, 4, static_cast<int>(sizeof(LineVertex)), offsetof(LineVertex, color));
        device.UnbindVertexArray();
    }

    DDGIDebugRenderer::~DDGIDebugRenderer()
    {
        if (!RTBEngine::Rendering::RHI::RenderDevice::HasDevice()) {
            vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
            vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
            return;
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        if (vao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(vao);
            vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        }
        if (vbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(vbo);
            vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
        }
    }

    void DDGIDebugRenderer::Render(RTBEngine::Rendering::Camera* camera, const DDGIDebugSettings& settings)
    {
        if (!camera || !lineShader || !settings.enabled) {
            return;
        }

        const RTBEngine::Rendering::GI::DDGISettings ddgiSettings = ResolveSettings();
        if (!ddgiSettings.enabled) {
            return;
        }

        std::vector<LineVertex> vertices;
        const RTBEngine::Math::Vector4 boundsColor(0.35f, 0.85f, 1.0f, 0.95f);
        const RTBEngine::Math::Vector4 probeColor(0.95f, 0.55f, 0.15f, 0.9f);

        const RTBEngine::Math::Vector3 minCorner = ddgiSettings.origin;
        const RTBEngine::Math::Vector3 maxCorner = ddgiSettings.origin + ddgiSettings.extent;

        if (settings.showVolumeBounds) {
            AppendAxisAlignedBox(vertices, minCorner, maxCorner, boundsColor);
        }

        if (settings.showProbeGrid) {
            const RTBEngine::Math::Vector3 spacing(
                ddgiSettings.extent.x / static_cast<float>(ddgiSettings.gridX),
                ddgiSettings.extent.y / static_cast<float>(ddgiSettings.gridY),
                ddgiSettings.extent.z / static_cast<float>(ddgiSettings.gridZ));

            for (int z = 0; z < ddgiSettings.gridZ; ++z) {
                for (int y = 0; y < ddgiSettings.gridY; ++y) {
                    for (int x = 0; x < ddgiSettings.gridX; ++x) {
                        const RTBEngine::Math::Vector3 probePos = ddgiSettings.origin
                            + RTBEngine::Math::Vector3(
                                (static_cast<float>(x) + 0.5f) * spacing.x,
                                (static_cast<float>(y) + 0.5f) * spacing.y,
                                (static_cast<float>(z) + 0.5f) * spacing.z);
                        AppendProbeMarker(vertices, probePos, settings.probeDrawRadius, probeColor);
                    }
                }
            }
        }

        if (vertices.empty()) {
            return;
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        device.SetArrayBufferData(
            vbo,
            vertices.data(),
            vertices.size() * sizeof(LineVertex),
            RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

        device.SetDepthTest(false);

        device.BindVertexArray(vao);
        device.DrawArrays(
            RTBEngine::Rendering::RHI::PrimitiveTopology::Lines,
            0,
            static_cast<int>(vertices.size()));
        device.UnbindVertexArray();

        device.SetDepthTest(true);
        lineShader->Unbind();
    }

}
