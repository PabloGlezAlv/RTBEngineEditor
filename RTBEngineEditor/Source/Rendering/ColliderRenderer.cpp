#include "ColliderRenderer.h"
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Scene/BoxColliderComponent.h>
#include <RTBEngine/Scene/SphereColliderComponent.h>
#include <RTBEngine/Scene/CapsuleColliderComponent.h>
#include <RTBEngine/Scene/Transform.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <array>
#include <algorithm>
#include <cmath>
#include <vector>

namespace RTBEditor {

    static constexpr int SPHERE_SEGMENTS = 32;
    static constexpr int SPHERE_VERTEX_COUNT = 3 * SPHERE_SEGMENTS * 2;
    static constexpr float DEBUG_QUERY_LIFETIME_SECONDS = 5.0f;
    static constexpr std::size_t MAX_DEBUG_QUERY_RENDER_COUNT = 128;

    struct LineVertex {
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Vector4 color;
    };

    namespace {
        using namespace RTBEngine::Rendering::RHI;

        void SetupDynamicLineVertexArray(IRenderDevice& device, GpuId vao, GpuId vbo, std::size_t initialSize)
        {
            device.BindVertexArray(vao);
            device.SetArrayBufferData(
                vbo,
                nullptr,
                initialSize,
                BufferUsage::Dynamic);
            device.EnableVertexAttribFloat(0, 3, static_cast<int>(sizeof(LineVertex)), 0);
            device.EnableVertexAttribFloat(
                1, 4, static_cast<int>(sizeof(LineVertex)), offsetof(LineVertex, color));
            device.UnbindVertexArray();
        }

        void BeginLineDraw(IRenderDevice& device)
        {
            device.SetBlend(true);
            device.SetBlendFuncSeparate(
                BlendFactor::SrcAlpha,
                BlendFactor::OneMinusSrcAlpha,
                BlendFactor::SrcAlpha,
                BlendFactor::OneMinusSrcAlpha);
            device.SetDepthTest(true);
        }

        void EndLineDraw(IRenderDevice& device)
        {
            device.SetBlend(false);
        }
    }

    ColliderRenderer::ColliderRenderer() {
        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Default/Shaders/EditorLine.vert",
            "Default/Shaders/EditorLine.frag"
        );

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();

        vao = device.CreateVertexArray();
        vbo = device.CreateBuffer();
        SetupDynamicLineVertexArray(device, vao, vbo, 24 * sizeof(LineVertex));

        sphereVao = device.CreateVertexArray();
        sphereVbo = device.CreateBuffer();
        SetupDynamicLineVertexArray(device, sphereVao, sphereVbo, SPHERE_VERTEX_COUNT * sizeof(LineVertex));

        capsuleVao = device.CreateVertexArray();
        capsuleVbo = device.CreateBuffer();
        SetupDynamicLineVertexArray(device, capsuleVao, capsuleVbo, 0);

        debugQueryVao = device.CreateVertexArray();
        debugQueryVbo = device.CreateBuffer();
        SetupDynamicLineVertexArray(device, debugQueryVao, debugQueryVbo, 0);

        RTBEngine::Physics::SetPhysicsDebugQueriesEnabled(true);
    }

    ColliderRenderer::~ColliderRenderer() {
        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        if (vao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(vao);
        }
        if (vbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(vbo);
        }
        if (sphereVao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(sphereVao);
        }
        if (sphereVbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(sphereVbo);
        }
        if (capsuleVao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(capsuleVao);
        }
        if (capsuleVbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(capsuleVbo);
        }
        if (debugQueryVao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyVertexArray(debugQueryVao);
        }
        if (debugQueryVbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
            device.DestroyBuffer(debugQueryVbo);
        }
    }

    void ColliderRenderer::RenderDebugQueries(RTBEngine::Rendering::Camera* camera) {
        if (!camera || !lineShader) {
            return;
        }

        std::array<RTBEngine::Physics::PhysicsDebugQueryEntry, MAX_DEBUG_QUERY_RENDER_COUNT> entries{};
        const int queryCount = RTBEngine::Physics::GetPhysicsDebugQuerySnapshot(
            entries.data(),
            static_cast<int>(entries.size()));

        if (queryCount <= 0) {
            return;
        }

        std::vector<LineVertex> lineVertices;
        lineVertices.reserve(static_cast<std::size_t>(queryCount) * 2);

        auto appendSegment = [&lineVertices](
            const RTBEngine::Math::Vector3& start,
            const RTBEngine::Math::Vector3& end,
            const RTBEngine::Math::Vector4& color) {
            lineVertices.push_back({ start, color });
            lineVertices.push_back({ end, color });
        };

        for (int i = 0; i < queryCount; ++i) {
            const RTBEngine::Physics::PhysicsDebugQueryEntry& entry = entries[static_cast<std::size_t>(i)];
            const float lifeAlpha = std::clamp(entry.remainingSeconds / DEBUG_QUERY_LIFETIME_SECONDS, 0.15f, 1.0f);

            if (entry.type == RTBEngine::Physics::PhysicsDebugQueryType::Raycast) {
                const RTBEngine::Math::Vector4 rayColor =
                    entry.hit
                        ? RTBEngine::Math::Vector4(1.0f, 0.35f, 0.2f, lifeAlpha)
                        : RTBEngine::Math::Vector4(1.0f, 0.8f, 0.15f, lifeAlpha);

                appendSegment(entry.start, entry.end, rayColor);

                if (entry.hit) {
                    const RTBEngine::Math::Vector3 hitPosition =
                        entry.start + (entry.end - entry.start) * entry.hitFraction;
                    RenderSphereWireframe(
                        camera,
                        hitPosition,
                        0.08f,
                        RTBEngine::Math::Vector4(1.0f, 0.15f, 0.1f, lifeAlpha));
                }

                continue;
            }

            const RTBEngine::Math::Vector4 pathColor =
                entry.hit
                    ? RTBEngine::Math::Vector4(0.2f, 0.95f, 1.0f, lifeAlpha)
                    : RTBEngine::Math::Vector4(0.35f, 0.75f, 1.0f, lifeAlpha * 0.9f);
            const RTBEngine::Math::Vector4 shellColor(pathColor.x, pathColor.y, pathColor.z, lifeAlpha * 0.65f);

            appendSegment(entry.start, entry.end, pathColor);
            RenderSphereWireframe(camera, entry.start, entry.radius, shellColor);
            RenderSphereWireframe(camera, entry.end, entry.radius, shellColor);

            if (entry.hit) {
                const RTBEngine::Math::Vector3 sweepHitCenter =
                    entry.start + (entry.end - entry.start) * entry.hitFraction;
                RenderSphereWireframe(
                    camera,
                    sweepHitCenter,
                    std::max(entry.radius, 0.05f),
                    RTBEngine::Math::Vector4(0.1f, 1.0f, 0.45f, lifeAlpha));
            }
        }

        if (lineVertices.empty()) {
            return;
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        device.SetArrayBufferData(
            debugQueryVbo,
            lineVertices.data(),
            lineVertices.size() * sizeof(LineVertex),
            RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        BeginLineDraw(device);

        device.BindVertexArray(debugQueryVao);
        device.DrawArrays(
            RTBEngine::Rendering::RHI::PrimitiveTopology::Lines,
            0,
            static_cast<int>(lineVertices.size()));
        device.UnbindVertexArray();

        EndLineDraw(device);
        lineShader->Unbind();
    }

    void ColliderRenderer::RenderSelection(RTBEngine::Rendering::Camera* camera,
                                           RTBEngine::Scene::GameObject* selectedObject) {
        if (!camera || !selectedObject || !lineShader) return;

        RTBEngine::Scene::BoxColliderComponent* boxCollider =
            selectedObject->GetComponent<RTBEngine::Scene::BoxColliderComponent>();
        if (boxCollider) {
            RTBEngine::Math::Vector3 halfExtents = boxCollider->size * 0.5f;
            const RTBEngine::Math::Vector3 center = boxCollider->GetCenterOffset();
            RTBEngine::Math::Matrix4 model = selectedObject->GetTransform().GetModelMatrix();
            RTBEngine::Math::Vector4 color(0.1f, 1.0f, 0.1f, 1.0f);

            float hx = halfExtents.x;
            float hy = halfExtents.y;
            float hz = halfExtents.z;
            float cx = center.x;
            float cy = center.y;
            float cz = center.z;

            RTBEngine::Math::Vector3 corners[8];
            auto transformCorner = [&](float x, float y, float z) -> RTBEngine::Math::Vector3 {
                RTBEngine::Math::Vector4 v = model * RTBEngine::Math::Vector4(x, y, z, 1.0f);
                return RTBEngine::Math::Vector3(v.x, v.y, v.z);
            };

            corners[0] = transformCorner(cx - hx, cy - hy, cz - hz);
            corners[1] = transformCorner(cx + hx, cy - hy, cz - hz);
            corners[2] = transformCorner(cx + hx, cy + hy, cz - hz);
            corners[3] = transformCorner(cx - hx, cy + hy, cz - hz);
            corners[4] = transformCorner(cx - hx, cy - hy, cz + hz);
            corners[5] = transformCorner(cx + hx, cy - hy, cz + hz);
            corners[6] = transformCorner(cx + hx, cy + hy, cz + hz);
            corners[7] = transformCorner(cx - hx, cy + hy, cz + hz);

            int edges[12][2] = {
                {0, 1}, {1, 2}, {2, 3}, {3, 0},
                {4, 5}, {5, 6}, {6, 7}, {7, 4},
                {0, 4}, {1, 5}, {2, 6}, {3, 7}
            };

            LineVertex vertices[24];
            for (int i = 0; i < 12; i++) {
                vertices[i * 2 + 0] = { corners[edges[i][0]], color };
                vertices[i * 2 + 1] = { corners[edges[i][1]], color };
            }

            auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
            device.SetArrayBufferData(
                vbo,
                vertices,
                sizeof(vertices),
                RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

            lineShader->Bind();
            lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

            BeginLineDraw(device);

            device.BindVertexArray(vao);
            device.DrawArrays(RTBEngine::Rendering::RHI::PrimitiveTopology::Lines, 0, 24);
            device.UnbindVertexArray();

            EndLineDraw(device);
            lineShader->Unbind();
        }

        RTBEngine::Scene::SphereColliderComponent* sphereCollider =
            selectedObject->GetComponent<RTBEngine::Scene::SphereColliderComponent>();
        if (sphereCollider) {
            RenderSphereWireframe(camera, selectedObject, sphereCollider);
        }

        RTBEngine::Scene::CapsuleColliderComponent* capsuleCollider =
            selectedObject->GetComponent<RTBEngine::Scene::CapsuleColliderComponent>();
        if (capsuleCollider) {
            RenderCapsuleWireframe(camera, selectedObject, capsuleCollider);
        }
    }

    void ColliderRenderer::RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                                  RTBEngine::Scene::GameObject* object,
                                                  RTBEngine::Scene::SphereColliderComponent* collider) {
        const float radius = collider->radius;
        const RTBEngine::Math::Vector3 center = object->GetTransform().GetPosition() + collider->centerOffset;
        RenderSphereWireframe(camera, center, radius, RTBEngine::Math::Vector4(0.1f, 1.0f, 0.1f, 1.0f));
    }

    void ColliderRenderer::RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                                  const RTBEngine::Math::Vector3& center,
                                                  float radius,
                                                  const RTBEngine::Math::Vector4& color) {
        if (!camera || !lineShader || radius <= 0.0f) {
            return;
        }

        LineVertex vertices[SPHERE_VERTEX_COUNT];
        int idx = 0;

        for (int seg = 0; seg < SPHERE_SEGMENTS; seg++) {
            float a0 = (float)seg       / SPHERE_SEGMENTS * 2.0f * 3.14159265f;
            float a1 = (float)(seg + 1) / SPHERE_SEGMENTS * 2.0f * 3.14159265f;

            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c0, center.y + radius * s0, center.z), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c1, center.y + radius * s1, center.z), color };

            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c0, center.y, center.z + radius * s0), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c1, center.y, center.z + radius * s1), color };

            vertices[idx++] = { RTBEngine::Math::Vector3(center.x, center.y + radius * c0, center.z + radius * s0), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x, center.y + radius * c1, center.z + radius * s1), color };
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        device.SetArrayBufferData(
            sphereVbo,
            vertices,
            sizeof(vertices),
            RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        BeginLineDraw(device);

        device.BindVertexArray(sphereVao);
        device.DrawArrays(RTBEngine::Rendering::RHI::PrimitiveTopology::Lines, 0, SPHERE_VERTEX_COUNT);
        device.UnbindVertexArray();

        EndLineDraw(device);
        lineShader->Unbind();
    }

    void ColliderRenderer::RenderCapsuleWireframe(RTBEngine::Rendering::Camera* camera,
                                                  RTBEngine::Scene::GameObject* object,
                                                  RTBEngine::Scene::CapsuleColliderComponent* collider) {
        const float r = collider->radius;
        const float cylinderHalf = std::max(0.0f, collider->height * 0.5f - r);
        const RTBEngine::Math::Vector3 localCenter = collider->centerOffset;
        const RTBEngine::Math::Vector4 color(0.1f, 1.0f, 0.1f, 1.0f);
        const RTBEngine::Math::Vector3 objectPosition = object->GetTransform().GetPosition();
        const RTBEngine::Math::Quaternion objectRotation = object->GetTransform().GetRotation();

        auto transformPoint = [&](const RTBEngine::Math::Vector3& localPoint) {
            return objectPosition + (objectRotation * localPoint);
        };

        std::vector<LineVertex> vertices;
        vertices.reserve(640);

        auto pushSegment = [&](const RTBEngine::Math::Vector3& a, const RTBEngine::Math::Vector3& b) {
            vertices.push_back({ transformPoint(a), color });
            vertices.push_back({ transformPoint(b), color });
        };

        for (int seg = 0; seg < SPHERE_SEGMENTS; ++seg) {
            const float a0 = static_cast<float>(seg) / SPHERE_SEGMENTS * 2.0f * 3.14159265f;
            const float a1 = static_cast<float>(seg + 1) / SPHERE_SEGMENTS * 2.0f * 3.14159265f;
            const float c0 = std::cos(a0);
            const float s0 = std::sin(a0);
            const float c1 = std::cos(a1);
            const float s1 = std::sin(a1);

            const RTBEngine::Math::Vector3 topCenter = localCenter + RTBEngine::Math::Vector3(0.0f, cylinderHalf, 0.0f);
            const RTBEngine::Math::Vector3 bottomCenter = localCenter - RTBEngine::Math::Vector3(0.0f, cylinderHalf, 0.0f);

            pushSegment(
                topCenter + RTBEngine::Math::Vector3(r * c0, 0.0f, r * s0),
                topCenter + RTBEngine::Math::Vector3(r * c1, 0.0f, r * s1));
            pushSegment(
                bottomCenter + RTBEngine::Math::Vector3(r * c0, 0.0f, r * s0),
                bottomCenter + RTBEngine::Math::Vector3(r * c1, 0.0f, r * s1));
        }

        pushSegment(
            localCenter + RTBEngine::Math::Vector3( r, -cylinderHalf, 0.0f),
            localCenter + RTBEngine::Math::Vector3( r,  cylinderHalf, 0.0f));
        pushSegment(
            localCenter + RTBEngine::Math::Vector3(-r, -cylinderHalf, 0.0f),
            localCenter + RTBEngine::Math::Vector3(-r,  cylinderHalf, 0.0f));
        pushSegment(
            localCenter + RTBEngine::Math::Vector3(0.0f, -cylinderHalf,  r),
            localCenter + RTBEngine::Math::Vector3(0.0f,  cylinderHalf,  r));
        pushSegment(
            localCenter + RTBEngine::Math::Vector3(0.0f, -cylinderHalf, -r),
            localCenter + RTBEngine::Math::Vector3(0.0f,  cylinderHalf, -r));

        for (int seg = 0; seg < SPHERE_SEGMENTS; ++seg) {
            const float a0 = static_cast<float>(seg) / SPHERE_SEGMENTS * 3.14159265f;
            const float a1 = static_cast<float>(seg + 1) / SPHERE_SEGMENTS * 3.14159265f;

            const RTBEngine::Math::Vector3 topCenter = localCenter + RTBEngine::Math::Vector3(0.0f, cylinderHalf, 0.0f);
            const RTBEngine::Math::Vector3 bottomCenter = localCenter - RTBEngine::Math::Vector3(0.0f, cylinderHalf, 0.0f);

            pushSegment(
                topCenter + RTBEngine::Math::Vector3(r * std::cos(a0), r * std::sin(a0), 0.0f),
                topCenter + RTBEngine::Math::Vector3(r * std::cos(a1), r * std::sin(a1), 0.0f));
            pushSegment(
                bottomCenter + RTBEngine::Math::Vector3(r * std::cos(a0 + 3.14159265f), r * std::sin(a0 + 3.14159265f), 0.0f),
                bottomCenter + RTBEngine::Math::Vector3(r * std::cos(a1 + 3.14159265f), r * std::sin(a1 + 3.14159265f), 0.0f));

            pushSegment(
                topCenter + RTBEngine::Math::Vector3(0.0f, r * std::sin(a0), r * std::cos(a0)),
                topCenter + RTBEngine::Math::Vector3(0.0f, r * std::sin(a1), r * std::cos(a1)));
            pushSegment(
                bottomCenter + RTBEngine::Math::Vector3(0.0f, r * std::sin(a0 + 3.14159265f), r * std::cos(a0 + 3.14159265f)),
                bottomCenter + RTBEngine::Math::Vector3(0.0f, r * std::sin(a1 + 3.14159265f), r * std::cos(a1 + 3.14159265f)));
        }

        auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
        device.SetArrayBufferData(
            capsuleVbo,
            vertices.data(),
            vertices.size() * sizeof(LineVertex),
            RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        BeginLineDraw(device);

        device.BindVertexArray(capsuleVao);
        device.DrawArrays(
            RTBEngine::Rendering::RHI::PrimitiveTopology::Lines,
            0,
            static_cast<int>(vertices.size()));
        device.UnbindVertexArray();

        EndLineDraw(device);
        lineShader->Unbind();
    }

}
