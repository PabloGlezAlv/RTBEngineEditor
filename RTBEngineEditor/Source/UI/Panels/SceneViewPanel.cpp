#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "SceneViewPanel.h"
#include <imgui.h>
#include <ImGuizmo.h>
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

        // Toolbar for gizmo controls
        ImGui::PopStyleVar(); // Remove padding for toolbar
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

        // Gizmo operation buttons
        if (ImGui::Button("Translate (W)")) gizmoOperation = GizmoOperation::Translate;
        ImGui::SameLine();
        if (ImGui::Button("Rotate (E)")) gizmoOperation = GizmoOperation::Rotate;
        ImGui::SameLine();
        if (ImGui::Button("Scale (R)")) gizmoOperation = GizmoOperation::Scale;
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        if (ImGui::Button(gizmoLocalMode ? "Local" : "World")) {
            gizmoLocalMode = !gizmoLocalMode;
        }

        ImGui::PopStyleVar();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

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
            HandleGizmo(context);
        }

        ImGui::End();
        ImGui::PopStyleVar();

        // Update camera if window is focused or hovered
        if (isFocused || isHovered) {
            float deltaTime = 1.0f / 60.0f;
            UpdateEditorCamera(deltaTime);
        }

        // Handle keyboard shortcuts for gizmo operations
        if (isFocused && !ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            if (ImGui::IsKeyPressed(ImGuiKey_W)) gizmoOperation = GizmoOperation::Translate;
            if (ImGui::IsKeyPressed(ImGuiKey_E)) gizmoOperation = GizmoOperation::Rotate;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmoOperation = GizmoOperation::Scale;
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

    void SceneViewPanel::HandleGizmo(EditorContext& context) {
        if (!context.selectedGameObject) {
            return;
        }

        // Don't show gizmo if camera is being controlled
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            return;
        }

        // Setup ImGuizmo
        ImGuizmo::BeginFrame();
        ImGuizmo::SetDrawlist();

        // Set the rect for the viewport
        ImVec2 windowPos = ImGui::GetItemRectMin();
        ImGuizmo::SetRect(windowPos.x, windowPos.y, (float)viewportWidth, (float)viewportHeight);

        // Get view and projection matrices
        RTBEngine::Math::Matrix4 viewMatrix = editorCamera.GetViewMatrix();
        RTBEngine::Math::Matrix4 projMatrix = editorCamera.GetProjectionMatrix();

        // Get transform of selected object
        RTBEngine::ECS::Transform& transform = context.selectedGameObject->GetTransform();
        RTBEngine::Math::Matrix4 modelMatrix = transform.GetModelMatrix();

        // Determine operation
        ImGuizmo::OPERATION operation;
        switch (gizmoOperation) {
        case GizmoOperation::Translate: operation = ImGuizmo::TRANSLATE; break;
        case GizmoOperation::Rotate: operation = ImGuizmo::ROTATE; break;
        case GizmoOperation::Scale: operation = ImGuizmo::SCALE; break;
        default: operation = ImGuizmo::TRANSLATE; break;
        }

        // Determine mode
        ImGuizmo::MODE mode = gizmoLocalMode ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

        // Manipulate the object
        bool manipulated = ImGuizmo::Manipulate(
            viewMatrix.GetData(),
            projMatrix.GetData(),
            operation,
            mode,
            modelMatrix.GetData()
        );

        // If the gizmo was manipulated, update the transform
        if (manipulated && ImGuizmo::IsUsing()) {
            // Decompose the matrix to get position, rotation, scale
            float position[3], rotation[3], scale[3];
            ImGuizmo::DecomposeMatrixToComponents(modelMatrix.GetData(), position, rotation, scale);

            // Update the transform
            transform.SetPosition(RTBEngine::Math::Vector3(position[0], position[1], position[2]));
            transform.SetRotation(RTBEngine::Math::Vector3(rotation[0], rotation[1], rotation[2]));
            transform.SetScale(RTBEngine::Math::Vector3(scale[0], scale[1], scale[2]));
        }
    }

}
