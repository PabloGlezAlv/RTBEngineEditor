#include "EditorGridRenderer.h"
#include <RTBEngine/Core/ResourceManager.h>
#include <vector>

namespace RTBEditor {

    struct LineVertex {
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Vector4 color;
    };

    EditorGridRenderer::EditorGridRenderer() {
        gridVAO = gridVBO = 0;
        axesVAO = axesVBO = 0;
        gridVertexCount = 0;
        axesVertexCount = 0;

        lineShader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
            "EditorLineShader",
            "Assets/Shaders/EditorLine.vert",
            "Assets/Shaders/EditorLine.frag"
        );

        CreateGridMesh();
        CreateAxesMesh();
    }

    EditorGridRenderer::~EditorGridRenderer() {
        if (gridVAO) glDeleteVertexArrays(1, &gridVAO);
        if (gridVBO) glDeleteBuffers(1, &gridVBO);
        if (axesVAO) glDeleteVertexArrays(1, &axesVAO);
        if (axesVBO) glDeleteBuffers(1, &axesVBO);
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

        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);

        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);
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

        glGenVertexArrays(1, &axesVAO);
        glGenBuffers(1, &axesVBO);

        glBindVertexArray(axesVAO);
        glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

        glBindVertexArray(0);
    }

    void EditorGridRenderer::Render(RTBEngine::Rendering::Camera* camera) {
        if (!lineShader || !camera) return;

        lineShader->Bind();

        // Calculate grid offset to follow camera (snapped to grid spacing)
        RTBEngine::Math::Vector3 camPos = camera->GetPosition();
        float gridX = floor(camPos.x / gridSpacing) * gridSpacing;
        float gridZ = floor(camPos.z / gridSpacing) * gridSpacing;

        // Create translation matrix for grid
        RTBEngine::Math::Matrix4 gridTransform = RTBEngine::Math::Matrix4::Translate(
            RTBEngine::Math::Vector3(gridX, 0.0f, gridZ)
        );

        RTBEngine::Math::Matrix4 viewProjection = camera->GetViewProjectionMatrix();
        RTBEngine::Math::Matrix4 mvp = viewProjection * gridTransform;
        lineShader->SetMatrix4("uViewProjection", mvp);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        if (gridVAO && gridVertexCount > 0) {
            glBindVertexArray(gridVAO);
            glDrawArrays(GL_LINES, 0, gridVertexCount);
        }

        glDepthMask(GL_TRUE);

        // Render axes at world origin (no transform)
        lineShader->SetMatrix4("uViewProjection", viewProjection);

        if (axesVAO && axesVertexCount > 0) {
            glBindVertexArray(axesVAO);
            glDrawArrays(GL_LINES, 0, axesVertexCount);
        }

        glBindVertexArray(0);
        glDisable(GL_BLEND);

        lineShader->Unbind();
    }

}
