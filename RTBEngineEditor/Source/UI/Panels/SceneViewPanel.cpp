#include "SceneViewPanel.h"
#include <imgui.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Input/MouseButton.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/Rendering/Mesh.h>
#include <GL/glew.h>
#include <limits>

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

        // Create grid renderer
        gridRenderer = std::make_unique<EditorGridRenderer>();
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

            HandleObjectPicking(context);
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

    void SceneViewPanel::HandleObjectPicking(EditorContext& context) {
        if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            return;
        }

        if (!ImGui::IsItemHovered()) {
            return;
        }

        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 windowPos = ImGui::GetItemRectMin();

        RTBEngine::Math::Vector2 localMousePos(
            mousePos.x - windowPos.x,
            mousePos.y - windowPos.y
        );

        if (localMousePos.x < 0 || localMousePos.x >= viewportWidth ||
            localMousePos.y < 0 || localMousePos.y >= viewportHeight) {
            return;
        }

        RTBEngine::Math::Vector2 viewportSize((float)viewportWidth, (float)viewportHeight);
        Ray ray = Ray::ScreenPointToRay(localMousePos, viewportSize, &editorCamera);

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) {
            return;
        }

        RTBEngine::ECS::GameObject* closestObject = nullptr;
        float closestDistance = std::numeric_limits<float>::max();

        const auto& gameObjects = scene->GetGameObjects();

        for (const auto& goPtr : gameObjects) {
            RTBEngine::ECS::GameObject* obj = goPtr.get();
            if (!obj) continue;

            RTBEngine::ECS::MeshRenderer* meshRenderer = obj->GetComponent<RTBEngine::ECS::MeshRenderer>();
            if (!meshRenderer) continue;

            const auto& meshes = meshRenderer->GetMeshes();
            if (meshes.empty()) continue;

            // Calculate world-space AABB from all meshes
            RTBEngine::Math::Vector3 worldMin(std::numeric_limits<float>::max());
            RTBEngine::Math::Vector3 worldMax(std::numeric_limits<float>::lowest());

            for (RTBEngine::Rendering::Mesh* mesh : meshes) {
                if (!mesh) continue;

                RTBEngine::Math::Vector3 meshMin = mesh->GetAABBMin();
                RTBEngine::Math::Vector3 meshMax = mesh->GetAABBMax();

                // Transform AABB by object transform
                RTBEngine::ECS::Transform& transform = obj->GetTransform();
                RTBEngine::Math::Vector3 position = transform.GetPosition();
                RTBEngine::Math::Vector3 scale = transform.GetScale();

                RTBEngine::Math::Vector3 transformedMin = position + meshMin * scale;
                RTBEngine::Math::Vector3 transformedMax = position + meshMax * scale;

                // Expand world AABB
                worldMin.x = std::min(worldMin.x, transformedMin.x);
                worldMin.y = std::min(worldMin.y, transformedMin.y);
                worldMin.z = std::min(worldMin.z, transformedMin.z);

                worldMax.x = std::max(worldMax.x, transformedMax.x);
                worldMax.y = std::max(worldMax.y, transformedMax.y);
                worldMax.z = std::max(worldMax.z, transformedMax.z);
            }

            // Test ray intersection
            float distance;
            if (ray.IntersectsAABB(worldMin, worldMax, distance)) {
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestObject = obj;
                }
            }
        }

        context.selectedGameObject = closestObject;
    }

}
