#pragma once
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <GL/glew.h>

namespace RTBEngine {
    namespace Scene {
        class SphereColliderComponent;
        class CapsuleColliderComponent;
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
                             RTBEngine::Scene::GameObject* selectedObject);
        void RenderDebugQueries(RTBEngine::Rendering::Camera* camera);

    private:
        //Box collider rendering
        GLuint vao = 0;
        GLuint vbo = 0;

        //Sphere collider rendering
        GLuint sphereVao = 0;
        GLuint sphereVbo = 0;

        //Capsule collider rendering
        GLuint capsuleVao = 0;
        GLuint capsuleVbo = 0;

        //Dynamic lines for debug queries
        GLuint debugQueryVao = 0;
        GLuint debugQueryVbo = 0;

        RTBEngine::Rendering::Shader* lineShader = nullptr;

        void RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                   RTBEngine::Scene::GameObject* object,
                                   RTBEngine::Scene::SphereColliderComponent* collider);
        void RenderSphereWireframe(RTBEngine::Rendering::Camera* camera,
                                   const RTBEngine::Math::Vector3& center,
                                   float radius,
                                   const RTBEngine::Math::Vector4& color);
        void RenderCapsuleWireframe(RTBEngine::Rendering::Camera* camera,
                                    RTBEngine::Scene::GameObject* object,
                                    RTBEngine::Scene::CapsuleColliderComponent* collider);
    };

}
