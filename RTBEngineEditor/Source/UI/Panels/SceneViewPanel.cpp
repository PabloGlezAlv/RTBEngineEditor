#include "SceneViewPanel.h"
#include <imgui.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Input/MouseButton.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <GL/glew.h>

namespace RTBEditor {

    SceneViewPanel::SceneViewPanel() {
        // Create framebuffer with initial size
        framebuffer = std::make_unique<RTBEngine::Rendering::Framebuffer>();
        framebuffer->CreateWithColorAndDepth(1280, 720);
        viewportWidth = 1280;
        viewportHeight = 720;

        // Setup default camera position
        editorCamera.SetPosition(RTBEngine::Math::Vector3(0.0f, 2.0f, 5.0f));
        editorCamera.SetRotation(-20.0f, 180.0f);  // Looking down towards -Z (origin)
        editorCamera.SetFOV(45.0f);
        editorCamera.SetNearPlane(0.1f);
        editorCamera.SetFarPlane(1000.0f);
    }

    SceneViewPanel::~SceneViewPanel() {}

    void SceneViewPanel::OnUIRender(EditorContext& context) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Scene");

        isFocused = ImGui::IsWindowFocused();
        isHovered = ImGui::IsWindowHovered();

        // Get available size for the viewport
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        
        // Check if we need to resize the framebuffer
        int newWidth = (int)availableSize.x;
        int newHeight = (int)availableSize.y;
        
        if (newWidth > 0 && newHeight > 0) {
            if (viewportWidth != newWidth || viewportHeight != newHeight) {
                viewportWidth = newWidth;
                viewportHeight = newHeight;
                framebuffer->Resize(viewportWidth, viewportHeight);
                editorCamera.SetAspectRatio((float)viewportWidth / (float)viewportHeight);
            }
        }

        // Display the framebuffer texture
        GLuint textureID = framebuffer->GetColorTextureID();
        if (textureID != 0 && viewportWidth > 0 && viewportHeight > 0) {
            // Flip the texture vertically (OpenGL has origin at bottom-left)
            ImGui::Image(
                (ImTextureID)(intptr_t)textureID,
                ImVec2((float)viewportWidth, (float)viewportHeight),
                ImVec2(0, 1),  // UV top-left (flipped)
                ImVec2(1, 0)   // UV bottom-right (flipped)
            );
        }

        ImGui::End();
        ImGui::PopStyleVar();

        // Update camera if window is focused or hovered
        if (isFocused || isHovered) {
            float deltaTime = 1.0f / 60.0f;
            UpdateEditorCamera(deltaTime);
        }
    }

    void SceneViewPanel::UpdateEditorCamera(float deltaTime) {
        // Use ImGui input system for reliable editor controls
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            HandleMouseInput();
            HandleKeyboardInput(deltaTime);
        }
    }

    void SceneViewPanel::HandleMouseInput() {
        // Use ImGui mouse delta
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        
        // Rotate camera based on mouse movement
        editorCamera.Rotate(-delta.y * mouseSensitivity, delta.x * mouseSensitivity);
    }

    void SceneViewPanel::HandleKeyboardInput(float deltaTime) {
        float currentSpeed = cameraSpeed;
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            currentSpeed *= 3.0f;
        }

        float moveAmount = currentSpeed * deltaTime;

        // Use ImGui key checks
        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            editorCamera.MoveForward(moveAmount);
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            editorCamera.MoveForward(-moveAmount);
        }
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            editorCamera.MoveRight(moveAmount);
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            editorCamera.MoveRight(-moveAmount);
        }
        
        // Up/Down movement
        if (ImGui::IsKeyDown(ImGuiKey_E) || ImGui::IsKeyDown(ImGuiKey_Space)) {
            editorCamera.MoveUp(moveAmount);
        }
        if (ImGui::IsKeyDown(ImGuiKey_Q) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            editorCamera.MoveUp(-moveAmount);
        }
    }

}
