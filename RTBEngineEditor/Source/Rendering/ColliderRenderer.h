#pragma once
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/ECS/GameObject.h>
#include <GL/glew.h>

namespace RTBEditor {

    class ColliderRenderer {
    public:
        ColliderRenderer();
        ~ColliderRenderer();

        ColliderRenderer(const ColliderRenderer&) = delete;
        ColliderRenderer& operator=(const ColliderRenderer&) = delete;

        // Renders a wireframe box for the selected object's BoxColliderComponent, if it has one.
        void RenderSelection(RTBEngine::Rendering::Camera* camera,
                             RTBEngine::ECS::GameObject* selectedObject);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };

}
