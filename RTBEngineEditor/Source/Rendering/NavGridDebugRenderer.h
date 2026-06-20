#pragma once

#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include "../UI/Panels/EditorPanel.h"
#include <GL/glew.h>

namespace RTBEditor {

    class NavGridDebugRenderer {
    public:
        NavGridDebugRenderer();
        ~NavGridDebugRenderer();

        NavGridDebugRenderer(const NavGridDebugRenderer&) = delete;
        NavGridDebugRenderer& operator=(const NavGridDebugRenderer&) = delete;

        void Render(RTBEngine::Rendering::Camera* camera,
                    RTBEngine::ECS::Scene* scene,
                    RTBEngine::ECS::GameObject* selectedObject,
                    const NavDebugSettings& settings);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        RTBEngine::Rendering::Shader* lineShader = nullptr;
    };

}
