#include "LightGizmoRenderer.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Rendering/Lighting/DirectionalLight.h>
#include <RTBEngine/Rendering/Lighting/SpotLight.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Scene/LightComponent.h>
#include <RTBEngine/Scene/GameObject.h>

#include <cmath>
#include <vector>

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

        RTBEngine::Math::Vector3 OrthonormalPerpendicular(const RTBEngine::Math::Vector3& dir)
        {
            const RTBEngine::Math::Vector3 n = dir.Normalized();
            RTBEngine::Math::Vector3 axis =
                (std::abs(n.y) < 0.99f)
                    ? RTBEngine::Math::Vector3(0.0f, 1.0f, 0.0f)
                    : RTBEngine::Math::Vector3(1.0f, 0.0f, 0.0f);
            return n.Cross(axis).Normalized();
        }

        void AppendArrow(std::vector<LineVertex>& vertices,
                         const RTBEngine::Math::Vector3& origin,
                         const RTBEngine::Math::Vector3& direction,
                         float length,
                         float headLength,
                         float headRadius,
                         const RTBEngine::Math::Vector4& color)
        {
            const RTBEngine::Math::Vector3 dir = direction.Normalized();
            const RTBEngine::Math::Vector3 tip = origin + dir * length;
            const RTBEngine::Math::Vector3 headBase = tip - dir * headLength;

            AppendLine(vertices, origin, tip, color);

            const RTBEngine::Math::Vector3 right = OrthonormalPerpendicular(dir);
            const RTBEngine::Math::Vector3 up = dir.Cross(right).Normalized();

            constexpr int kHeadSegments = 8;
            for (int i = 0; i < kHeadSegments; ++i) {
                const float a0 = (static_cast<float>(i) / kHeadSegments) * 6.2831853f;
                const float a1 = (static_cast<float>(i + 1) / kHeadSegments) * 6.2831853f;
                const RTBEngine::Math::Vector3 p0 =
                    headBase + (right * std::cos(a0) + up * std::sin(a0)) * headRadius;
                const RTBEngine::Math::Vector3 p1 =
                    headBase + (right * std::cos(a1) + up * std::sin(a1)) * headRadius;
                AppendLine(vertices, tip, p0, color);
                AppendLine(vertices, p0, p1, color);
            }
        }

        void AppendSunBurst(std::vector<LineVertex>& vertices,
                            const RTBEngine::Math::Vector3& origin,
                            const RTBEngine::Math::Vector3& direction,
                            float radius,
                            const RTBEngine::Math::Vector4& color)
        {
            const RTBEngine::Math::Vector3 dir = direction.Normalized();
            const RTBEngine::Math::Vector3 right = OrthonormalPerpendicular(dir);
            const RTBEngine::Math::Vector3 up = dir.Cross(right).Normalized();

            constexpr int kRays = 8;
            for (int i = 0; i < kRays; ++i) {
                const float a = (static_cast<float>(i) / kRays) * 6.2831853f;
                const RTBEngine::Math::Vector3 offset =
                    (right * std::cos(a) + up * std::sin(a)) * radius;
                AppendLine(vertices, origin - offset * 0.35f, origin + offset, color);
            }
        }

    }

    LightGizmoRenderer::LightGizmoRenderer()
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

    LightGizmoRenderer::~LightGizmoRenderer()
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

    void LightGizmoRenderer::Render(RTBEngine::Rendering::Camera* camera,
                                    RTBEngine::Scene::Scene* scene,
                                    RTBEngine::Scene::GameObject* selectedObject)
    {
        if (!camera || !scene || !lineShader) {
            return;
        }

        std::vector<LineVertex> vertices;

        for (RTBEngine::Scene::LightComponent* lightComp : scene->GetCachedLightComponents()) {
            if (!lightComp || !lightComp->IsEnabled() || !lightComp->GetLight()) {
                continue;
            }

            RTBEngine::Scene::GameObject* owner = lightComp->GetOwner();
            if (!owner || !owner->IsActiveInHierarchy()) {
                continue;
            }

            const auto type = lightComp->GetLight()->GetType();
            if (type != RTBEngine::Rendering::LightType::Directional
                && type != RTBEngine::Rendering::LightType::Spot) {
                continue;
            }

            const bool selected = (owner == selectedObject);
            const RTBEngine::Math::Color& c = lightComp->color;
            const float alpha = selected ? 1.0f : 0.75f;
            const RTBEngine::Math::Vector4 color(c.r, c.g, c.b, alpha);

            const RTBEngine::Math::Vector3 origin = owner->GetWorldPosition();
            RTBEngine::Math::Vector3 direction(0.0f, 0.0f, 1.0f);

            if (type == RTBEngine::Rendering::LightType::Directional) {
                auto* dirLight = static_cast<RTBEngine::Rendering::DirectionalLight*>(lightComp->GetLight());
                direction = dirLight->GetDirection();
                const float length = selected ? 3.5f : 2.5f;
                AppendSunBurst(vertices, origin, direction, selected ? 0.45f : 0.3f, color);
                AppendArrow(vertices, origin, direction, length, length * 0.22f, length * 0.12f, color);
            } else {
                auto* spot = static_cast<RTBEngine::Rendering::SpotLight*>(lightComp->GetLight());
                direction = spot->GetDirection();
                const float length = selected ? 2.5f : 1.8f;
                AppendArrow(vertices, origin, direction, length, length * 0.2f, length * 0.1f, color);

                // Spot cone ring at tip.
                const float outerDeg = lightComp->spotAngle;
                const float radius = length * std::tan(outerDeg * 0.5f * 3.14159265f / 180.0f);
                const RTBEngine::Math::Vector3 tip = origin + direction.Normalized() * length;
                const RTBEngine::Math::Vector3 right = OrthonormalPerpendicular(direction);
                const RTBEngine::Math::Vector3 up = direction.Normalized().Cross(right).Normalized();
                constexpr int kRing = 16;
                for (int i = 0; i < kRing; ++i) {
                    const float a0 = (static_cast<float>(i) / kRing) * 6.2831853f;
                    const float a1 = (static_cast<float>(i + 1) / kRing) * 6.2831853f;
                    const RTBEngine::Math::Vector3 p0 =
                        tip + (right * std::cos(a0) + up * std::sin(a0)) * radius;
                    const RTBEngine::Math::Vector3 p1 =
                        tip + (right * std::cos(a1) + up * std::sin(a1)) * radius;
                    AppendLine(vertices, origin, p0, color);
                    AppendLine(vertices, p0, p1, color);
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
        device.SetBlend(true);
        device.SetBlendFuncSeparate(
            RTBEngine::Rendering::RHI::BlendFactor::SrcAlpha,
            RTBEngine::Rendering::RHI::BlendFactor::OneMinusSrcAlpha,
            RTBEngine::Rendering::RHI::BlendFactor::SrcAlpha,
            RTBEngine::Rendering::RHI::BlendFactor::OneMinusSrcAlpha);

        device.BindVertexArray(vao);
        device.DrawArrays(
            RTBEngine::Rendering::RHI::PrimitiveTopology::Lines,
            0,
            static_cast<int>(vertices.size()));
        device.UnbindVertexArray();

        device.SetBlend(false);
        device.SetDepthTest(true);
        lineShader->Unbind();
    }

}
