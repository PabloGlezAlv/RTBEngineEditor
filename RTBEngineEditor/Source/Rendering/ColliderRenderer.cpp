#include "ColliderRenderer.h"
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/ECS/BoxColliderComponent.h>
#include <RTBEngine/ECS/SphereColliderComponent.h>
#include <RTBEngine/ECS/CapsuleColliderComponent.h>
#include <RTBEngine/ECS/Transform.h>
#include <RTBEngine/Math/Math.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace RTBEditor {

    static constexpr int SPHERE_SEGMENTS = 32;
    // 3 circles (XY, XZ, YZ) × 32 segments × 2 endpoints = 192 vertices
    static constexpr int SPHERE_VERTEX_COUNT = 3 * SPHERE_SEGMENTS * 2;

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
    }

    ColliderRenderer::~ColliderRenderer() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (sphereVao) glDeleteVertexArrays(1, &sphereVao);
        if (sphereVbo) glDeleteBuffers(1, &sphereVbo);
        if (capsuleVao) glDeleteVertexArrays(1, &capsuleVao);
        if (capsuleVbo) glDeleteBuffers(1, &capsuleVbo);
    }

    void ColliderRenderer::RenderSelection(RTBEngine::Rendering::Camera* camera,
                                           RTBEngine::ECS::GameObject* selectedObject) {
        if (!camera || !selectedObject || !lineShader) return;

        //Box collider branch
        RTBEngine::ECS::BoxColliderComponent* boxCollider =
            selectedObject->GetComponent<RTBEngine::ECS::BoxColliderComponent>();
        if (boxCollider) {
            RTBEngine::Math::Vector3 halfExtents = boxCollider->size * 0.5f;
            RTBEngine::Math::Matrix4 model = selectedObject->GetTransform().GetModelMatrix();
            RTBEngine::Math::Vector4 color(0.1f, 1.0f, 0.1f, 1.0f);

            float hx = halfExtents.x;
            float hy = halfExtents.y;
            float hz = halfExtents.z;

            RTBEngine::Math::Vector3 corners[8];
            auto transformCorner = [&](float x, float y, float z) -> RTBEngine::Math::Vector3 {
                RTBEngine::Math::Vector4 v = model * RTBEngine::Math::Vector4(x, y, z, 1.0f);
                return RTBEngine::Math::Vector3(v.x, v.y, v.z);
            };

            corners[0] = transformCorner(-hx, -hy, -hz);
            corners[1] = transformCorner( hx, -hy, -hz);
            corners[2] = transformCorner( hx,  hy, -hz);
            corners[3] = transformCorner(-hx,  hy, -hz);
            corners[4] = transformCorner(-hx, -hy,  hz);
            corners[5] = transformCorner( hx, -hy,  hz);
            corners[6] = transformCorner( hx,  hy,  hz);
            corners[7] = transformCorner(-hx,  hy,  hz);

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
        float r = collider->radius;
        RTBEngine::Math::Vector3 center = object->GetTransform().GetPosition() + collider->centerOffset;
        RTBEngine::Math::Vector4 color(0.1f, 1.0f, 0.1f, 1.0f);

        LineVertex vertices[SPHERE_VERTEX_COUNT];
        int idx = 0;

        // Generate 3 circles: XY plane, XZ plane, YZ plane
        for (int seg = 0; seg < SPHERE_SEGMENTS; seg++) {
            float a0 = (float)seg       / SPHERE_SEGMENTS * 2.0f * 3.14159265f;
            float a1 = (float)(seg + 1) / SPHERE_SEGMENTS * 2.0f * 3.14159265f;

            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            //XY circle
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + r * c0, center.y + r * s0, center.z), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + r * c1, center.y + r * s1, center.z), color };

            //XZ circle
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + r * c0, center.y, center.z + r * s0), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x + r * c1, center.y, center.z + r * s1), color };

            //YZ circle
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x, center.y + r * c0, center.z + r * s0), color };
            vertices[idx++] = { RTBEngine::Math::Vector3(center.x, center.y + r * c1, center.z + r * s1), color };
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
