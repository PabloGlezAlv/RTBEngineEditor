#include "VolumeRenderer.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Scene/ComponentQuery.h>
#include <RTBEngine/Scene/VolumeComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Transform.h>
#include <RTBEngine/Math/Math.h>
#include <array>

namespace RTBEditor {

    struct LineVertex {
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Vector4 color;
    };

    namespace {
        using namespace RTBEngine::Rendering::RHI;

        void SetupLineBuffer(IRenderDevice& device, GpuId vao, GpuId vbo)
        {
            device.BindVertexArray(vao);
            device.SetArrayBufferData(vbo, nullptr, 24 * sizeof(LineVertex), BufferUsage::Dynamic);
            device.EnableVertexAttribFloat(0, 3, static_cast<int>(sizeof(LineVertex)), 0);
            device.EnableVertexAttribFloat(
                1, 4, static_cast<int>(sizeof(LineVertex)), offsetof(LineVertex, color));
            device.UnbindVertexArray();
        }
    }

    VolumeRenderer::VolumeRenderer()
    {
        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Default/Shaders/EditorLine.vert",
            "Default/Shaders/EditorLine.frag");

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        vao = device.CreateVertexArray();
        vbo = device.CreateBuffer();
        SetupLineBuffer(device, vao, vbo);
    }

    VolumeRenderer::~VolumeRenderer()
    {
        if (!RTBEngine::Rendering::RHI::RenderDevice::HasDevice()) {
            vao = vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
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

    void VolumeRenderer::RenderBox(RTBEngine::Rendering::Camera* camera,
                                   const RTBEngine::Math::Matrix4& model,
                                   const RTBEngine::Math::Vector4& color)
    {
        if (!camera || !lineShader) {
            return;
        }

        const float hx = 0.5f;
        const float hy = 0.5f;
        const float hz = 0.5f;

        RTBEngine::Math::Vector3 corners[8];
        auto transformCorner = [&](float x, float y, float z) -> RTBEngine::Math::Vector3 {
            RTBEngine::Math::Vector4 v = model * RTBEngine::Math::Vector4(x, y, z, 1.0f);
            return RTBEngine::Math::Vector3(v.x, v.y, v.z);
        };

        corners[0] = transformCorner(-hx, -hy, -hz);
        corners[1] = transformCorner(hx, -hy, -hz);
        corners[2] = transformCorner(hx, hy, -hz);
        corners[3] = transformCorner(-hx, hy, -hz);
        corners[4] = transformCorner(-hx, -hy, hz);
        corners[5] = transformCorner(hx, -hy, hz);
        corners[6] = transformCorner(hx, hy, hz);
        corners[7] = transformCorner(-hx, hy, hz);

        const int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };

        LineVertex vertices[24];
        for (int i = 0; i < 12; ++i) {
            vertices[i * 2 + 0] = { corners[edges[i][0]], color };
            vertices[i * 2 + 1] = { corners[edges[i][1]], color };
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        device.SetArrayBufferData(vbo, vertices, sizeof(vertices), BufferUsage::Dynamic);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        device.SetBlend(true);
        device.SetBlendFuncSeparate(
            BlendFactor::SrcAlpha,
            BlendFactor::OneMinusSrcAlpha,
            BlendFactor::SrcAlpha,
            BlendFactor::OneMinusSrcAlpha);
        device.SetDepthTest(true);

        device.BindVertexArray(vao);
        device.DrawArrays(PrimitiveTopology::Lines, 0, 24);
        device.UnbindVertexArray();

        device.SetBlend(false);
        lineShader->Unbind();
    }

    void VolumeRenderer::RenderSceneVolumes(RTBEngine::Rendering::Camera* camera,
                                            RTBEngine::Scene::Scene* scene)
    {
        if (!camera || !scene) {
            return;
        }

        for (RTBEngine::Scene::Component* component
            : RTBEngine::Scene::ComponentQuery::GetComponents<RTBEngine::Scene::VolumeComponent>()) {
            auto* volume = dynamic_cast<RTBEngine::Scene::VolumeComponent*>(component);
            if (!volume || !volume->IsEnabled() || volume->isGlobal) {
                continue;
            }

            RTBEngine::Scene::GameObject* owner = volume->GetOwner();
            if (!owner || !owner->IsActiveInHierarchy()) {
                continue;
            }

            RTBEngine::Math::Matrix4 model = owner->GetWorldMatrix();
            model = model * RTBEngine::Math::Matrix4::Scale(volume->size);
            RenderBox(camera, model, RTBEngine::Math::Vector4(0.2f, 0.75f, 1.0f, 0.85f));
        }
    }

    void VolumeRenderer::RenderSelection(RTBEngine::Rendering::Camera* camera,
                                         RTBEngine::Scene::GameObject* selectedObject)
    {
        if (!camera || !selectedObject) {
            return;
        }

        auto* volume = selectedObject->GetComponent<RTBEngine::Scene::VolumeComponent>();
        if (!volume || !volume->IsEnabled()) {
            return;
        }

        if (volume->isGlobal) {
            return;
        }

        RTBEngine::Math::Matrix4 model = selectedObject->GetWorldMatrix();
        model = model * RTBEngine::Math::Matrix4::Scale(volume->size);
        RenderBox(camera, model, RTBEngine::Math::Vector4(0.95f, 0.85f, 0.2f, 1.0f));
    }

}
