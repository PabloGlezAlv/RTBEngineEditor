#pragma once
#include "EditorPanel.h"
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Rendering/Camera.h>
#include "../../Rendering/EditorGridRenderer.h"
#include "../../Rendering/ColliderRenderer.h"
#include "../../Rendering/NavGridDebugRenderer.h"
#include "../../Rendering/DDGIDebugRenderer.h"
#include "../../Utils/RaycastUtils.h"
#include <memory>

namespace RTBEditor {

    enum class GizmoOperation {
        Translate = 0,
        Rotate = 1,
        Scale = 2
    };

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

        // Collider renderer access
        ColliderRenderer* GetColliderRenderer() const { return colliderRenderer.get(); }
        NavGridDebugRenderer* GetNavGridDebugRenderer() const { return navGridDebugRenderer.get(); }
        DDGIDebugRenderer* GetDDGIDebugRenderer() const { return ddgiDebugRenderer.get(); }

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
        void HandleGizmo(EditorContext& context);
        void DrawViewCube();
        void AlignCameraToView(const RTBEngine::Math::Vector3& direction, const RTBEngine::Math::Vector3& up);

        std::unique_ptr<RTBEngine::Rendering::Framebuffer> framebuffer;
        RTBEngine::Rendering::Camera editorCamera;
        std::unique_ptr<EditorGridRenderer> gridRenderer;
        std::unique_ptr<ColliderRenderer> colliderRenderer;
        std::unique_ptr<NavGridDebugRenderer> navGridDebugRenderer;
        std::unique_ptr<DDGIDebugRenderer> ddgiDebugRenderer;

        // Viewport state
        int viewportWidth = 0;
        int viewportHeight = 0;
        bool isFocused = false;
        bool isHovered = false;

        // Camera control state
        float cameraSpeed = 5.0f;
        float mouseSensitivity = 0.1f;

        // Gizmo state
        GizmoOperation gizmoOperation = GizmoOperation::Translate;
        bool gizmoLocalMode = true; // true=Local, false=World
    };

}
