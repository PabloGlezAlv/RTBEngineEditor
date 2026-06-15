#include "ColliderRenderer.h"
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/ECS/BoxColliderComponent.h>
#include <RTBEngine/ECS/SphereColliderComponent.h>
#include <RTBEngine/ECS/CapsuleColliderComponent.h>
#include <RTBEngine/ECS/Transform.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <array>
#include <algorithm>
#include <cmath>
#include <vector>

namespace RTBEditor {

    static constexpr int SPHERE_SEGMENTS = 32;
    // 3 circles (XY, XZ, YZ) × 32 segments × 2 endpoints = 192 vertices
    static constexpr int SPHERE_VERTEX_COUNT = 3 * SPHERE_SEGMENTS * 2;
    static constexpr float DEBUG_QUERY_LIFETIME_SECONDS = 5.0f;
    static constexpr std::size_t MAX_DEBUG_QUERY_RENDER_COUNT = 128;

    struct LineVertex {
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Vector4 color;
    };

    ColliderRenderer::ColliderRenderer() {
        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Default/Shaders/EditorLine.vert",
            "Default/Shaders/EditorLine.frag"
        );

        //Box VAO/VBO — 24 vertices (12 edges × 2 endpoints)
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);

        //Sphere VAO/VBO — 192 vertices (3 circles × 32 segments × 2 endpoints)
        glGenVertexArrays(1, &sphereVao);
        glGenBuffers(1, &sphereVbo);

        glBindVertexArray(sphereVao);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
        glBufferData(GL_ARRAY_BUFFER, SPHERE_VERTEX_COUNT * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);

        glGenVertexArrays(1, &capsuleVao);
        glGenBuffers(1, &capsuleVbo);

        glBindVertexArray(capsuleVao);
        glBindBuffer(GL_ARRAY_BUFFER, capsuleVbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);

        glGenVertexArrays(1, &debugQueryVao);
        glGenBuffers(1, &debugQueryVbo);

        glBindVertexArray(debugQueryVao);
        glBindBuffer(GL_ARRAY_BUFFER, debugQueryVbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);

        RTBEngine::Physics::SetPhysicsDebugQueriesEnabled(true);
    }

    ColliderRenderer::~ColliderRenderer() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (sphereVao) glDeleteVertexArrays(1, &sphereVao);
        if (sphereVbo) glDeleteBuffers(1, &sphereVbo);
        if (capsuleVao) glDeleteVertexArrays(1, &capsuleVao);
        if (capsuleVbo) glDeleteBuffers(1, &capsuleVbo);
        if (debugQueryVao) glDeleteVertexArrays(1, &debugQueryVao);
        if (debugQueryVbo) glDeleteBuffers(1, &debugQueryVbo);
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

        glBindBuffer(GL_ARRAY_BUFFER, debugQueryVbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(lineVertices.size() * sizeof(LineVertex)),
            lineVertices.data(),
            GL_DYNAMIC_DRAW);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(debugQueryVao);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size()));
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        lineShader->Unbind();
    }

    void ColliderRenderer::RenderSelection(RTBEngine::Rendering::Camera* camera,
                                           RTBEngine::ECS::GameObject* selectedObject) {
        if (!camera || !selectedObject || !lineShader) return;

        //Box collider branch
        RTBEngine::ECS::BoxColliderComponent* boxCollider =
            selectedObject->GetComponent<RTBEngine::ECS::BoxColliderComponent>();
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

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            lineShader->Bind();
            lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);

            glBindVertexArray(vao);
            glDrawArrays(GL_LINES, 0, 24);
            glBindVertexArray(0);

            glDisable(GL_BLEND);
            lineShader->Unbind();
        }

        //Sphere collider branch
        RTBEngine::ECS::SphereColliderComponent* sphereCollider =
            selectedObject->GetComponent<RTBEngine::ECS::SphereColliderComponent>();
        if (sphereCollider) {
            RenderSphereWireframe(camera, selectedObject, sphereCollider);
        }

        RTBEngine::ECS::CapsuleColliderComponent* capsuleCollider =
            selectedObject->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>();
        if (capsuleCollider) {
            RenderCapsuleWireframe(camera, selectedObject, capsuleCollider);
        }
    }

    void ColliderRenderer::RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                                  RTBEngine::ECS::GameObject* object,
                                                  RTBEngine::ECS::SphereColliderComponent* collider) {
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

        // Generate 3 circles: XY plane, XZ plane, YZ plane
        for (int seg = 0; seg < SPHERE_SEGMENTS; seg++) {
            float a0 = (float)seg       / SPHERE_SEGMENTS * 2.0f * 3.14159265f;
            float a1 = (float)(seg + 1) / SPHERE_SEGMENTS * 2.0f * 3.14159265f;

            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            //XY circle
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c0, center.y + radius * s0, center.z), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c1, center.y + radius * s1, center.z), color };

            //XZ circle
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c0, center.y, center.z + radius * s0), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + radius * c1, center.y, center.z + radius * s1), color };

            //YZ circle
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x, center.y + radius * c0, center.z + radius * s0), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x, center.y + radius * c1, center.z + radius * s1), color };
        }

        glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(sphereVao);
        glDrawArrays(GL_LINES, 0, SPHERE_VERTEX_COUNT);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        lineShader->Unbind();
    }

    void ColliderRenderer::RenderCapsuleWireframe(RTBEngine::Rendering::Camera* camera,
                                                  RTBEngine::ECS::GameObject* object,
                                                  RTBEngine::ECS::CapsuleColliderComponent* collider) {
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

        glBindBuffer(GL_ARRAY_BUFFER, capsuleVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(LineVertex)), vertices.data(), GL_DYNAMIC_DRAW);

        lineShader->Bind();
        lineShader->SetMatrix4("uViewProjection", camera->GetViewProjectionMatrix());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(capsuleVao);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        lineShader->Unbind();
    }

}
