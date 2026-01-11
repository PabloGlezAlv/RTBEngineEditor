#pragma once
#include "EditorPanel.h"
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Rendering/Camera.h>
#include "../../Rendering/EditorGridRenderer.h"
#include "../../Utils/RaycastUtils.h"
#include <imgui.h>
#include <memory>

namespace RTBEditor {

    class SceneViewPanel : public EditorPanel {
    public:
        SceneViewPanel();
        ~SceneViewPanel() override;

        void OnUIRender(EditorContext& context) override;

        // Get the framebuffer for rendering
        RTBEngine::Rendering::Framebuffer* GetFramebuffer() { return framebuffer.get(); }

        // Editor camera access
        RTBEngine::Rendering::Camera* GetEditorCamera() { return &editorCamera; }

        // Grid renderer access
        EditorGridRenderer* GetGridRenderer() const { return gridRenderer.get(); }

        // Check if the panel is focused/hovered
        bool IsFocused() const { return isFocused; }
        bool IsHovered() const { return isHovered; }

        // Get viewport size
        int GetViewportWidth() const { return viewportWidth; }
        int GetViewportHeight() const { return viewportHeight; }

    private:
        void UpdateEditorCamera(float deltaTime);
        void HandleMouseInput();
        void HandleKeyboardInput(float deltaTime);
        void HandleObjectPicking(EditorContext& context);

        std::unique_ptr<RTBEngine::Rendering::Framebuffer> framebuffer;
        RTBEngine::Rendering::Camera editorCamera;
        std::unique_ptr<EditorGridRenderer> gridRenderer;

        // Viewport state
        int viewportWidth = 0;
        int viewportHeight = 0;
        bool isFocused = false;
        bool isHovered = false;

        // Camera control state
        float cameraSpeed = 5.0f;
        float mouseSensitivity = 0.1f;
    };

}
