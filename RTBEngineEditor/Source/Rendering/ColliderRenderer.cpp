#include "ColliderRenderer.h"
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/ECS/BoxColliderComponent.h>
#include <RTBEngine/ECS/Transform.h>
#include <RTBEngine/Math/Math.h>
#include <vector>

namespace RTBEditor {

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

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Reserve space for 24 vertices (12 edges x 2 endpoints)
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);
    }

    ColliderRenderer::~ColliderRenderer() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
    }

    void ColliderRenderer::RenderSelection(RTBEngine::Rendering::Camera* camera,
                                           RTBEngine::ECS::GameObject* selectedObject) {
        if (!camera || !selectedObject || !lineShader) return;

        RTBEngine::ECS::BoxColliderComponent* collider =
            selectedObject->GetComponent<RTBEngine::ECS::BoxColliderComponent>();
        if (!collider) return;

        RTBEngine::Math::Vector3 halfExtents = collider->size * 0.5f;
        RTBEngine::Math::Matrix4 model = selectedObject->GetTransform().GetModelMatrix();
        RTBEngine::Math::Vector4 color(0.1f, 1.0f, 0.1f, 1.0f);

        // Build 8 corners in local space and transform to world space
        float hx = halfExtents.x;
        float hy = halfExtents.y;
        float hz = halfExtents.z;

        RTBEngine::Math::Vector3 corners[8];
        auto transform = [&](float x, float y, float z) -> RTBEngine::Math::Vector3 {
            RTBEngine::Math::Vector4 v = model * RTBEngine::Math::Vector4(x, y, z, 1.0f);
            return RTBEngine::Math::Vector3(v.x, v.y, v.z);
        };

        corners[0] = transform(-hx, -hy, -hz);
        corners[1] = transform( hx, -hy, -hz);
        corners[2] = transform( hx,  hy, -hz);
        corners[3] = transform(-hx,  hy, -hz);
        corners[4] = transform(-hx, -hy,  hz);
        corners[5] = transform( hx, -hy,  hz);
        corners[6] = transform( hx,  hy,  hz);
        corners[7] = transform(-hx,  hy,  hz);

        // 12 edges: bottom face, top face, 4 vertical pillars
        int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},  // bottom face
            {4, 5}, {5, 6}, {6, 7}, {7, 4},  // top face
            {0, 4}, {1, 5}, {2, 6}, {3, 7}   // pillars
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

}
