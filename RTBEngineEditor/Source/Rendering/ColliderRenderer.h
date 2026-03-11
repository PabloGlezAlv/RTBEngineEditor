#pragma once
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/ECS/GameObject.h>
#include <GL/glew.h>

namespace RTBEngine {
    namespace ECS {
        class SphereColliderComponent;
    }
}

namespace RTBEditor {

    class ColliderRenderer {
    public:
        ColliderRenderer();
        ~ColliderRenderer();

        ColliderRenderer(const ColliderRenderer&) = delete;
        ColliderRenderer& operator=(const ColliderRenderer&) = delete;

        // Renders a wireframe collider for the selected object's collider component, if it has one.
        void RenderSelection(RTBEngine::Rendering::Camera* camera,
                             RTBEngine::ECS::GameObject* selectedObject);

    private:
        //Box collider rendering
        GLuint vao = 0;
        GLuint vbo = 0;

        //Sphere collider rendering
        GLuint sphereVao = 0;
        GLuint sphereVbo = 0;

        RTBEngine::Rendering::Shader* lineShader = nullptr;

        void RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                   RTBEngine::ECS::GameObject* object,
                                   RTBEngine::ECS::SphereColliderComponent* collider);
    };

}
