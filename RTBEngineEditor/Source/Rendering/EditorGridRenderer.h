#pragma once
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <GL/glew.h>

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

        GLuint gridVAO, gridVBO;
        GLuint axesVAO, axesVBO;
        int gridVertexCount;
        int axesVertexCount;

        float gridSize = 100.0f;
        float gridSpacing = 1.0f;
        float axisLength = 10.0f;

        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };
}
