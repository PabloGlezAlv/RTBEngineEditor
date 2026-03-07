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

        // Create collider renderer
        colliderRenderer = std::make_unique<ColliderRenderer>();
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

            // Draw view cube overlay
            DrawViewCube();
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

            // ImGuizmo returns degrees; Transform::SetRotation expects radians
            constexpr float kDeg2Rad = 3.14159265f / 180.0f;

            // Update the transform
            transform.SetPosition(RTBEngine::Math::Vector3(position[0], position[1], position[2]));
            transform.SetRotation(RTBEngine::Math::Vector3(
                rotation[0] * kDeg2Rad,
                rotation[1] * kDeg2Rad,
                rotation[2] * kDeg2Rad
            ));
            transform.SetScale(RTBEngine::Math::Vector3(scale[0], scale[1], scale[2]));
            
            // Notify components that something has changed
            for (auto& comp : context.selectedGameObject->GetComponents()) {
                if (comp) comp->OnValidate();
            }

            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
        }
    }

    void SceneViewPanel::AlignCameraToView(const RTBEngine::Math::Vector3& direction, const RTBEngine::Math::Vector3& up) {
        // Get current camera position
        RTBEngine::Math::Vector3 currentPos = editorCamera.GetPosition();

        // Calculate distance to origin (or selected object if any)
        float distance = currentPos.Length();
        if (distance < 1.0f) distance = 10.0f; // Default distance

        // Set new position maintaining the distance
        RTBEngine::Math::Vector3 newPos = -direction * distance;
        editorCamera.SetPosition(newPos);

        // Calculate rotation to look at origin
        RTBEngine::Math::Vector3 target(0, 0, 0);
        RTBEngine::Math::Vector3 forward = (target - newPos).Normalized();
        RTBEngine::Math::Vector3 right = up.Cross(forward).Normalized();
        RTBEngine::Math::Vector3 correctedUp = forward.Cross(right);

        // Convert to euler angles (pitch, yaw)
        float pitch = asinf(-forward.y) * (180.0f / 3.14159f);
        float yaw = atan2f(forward.x, forward.z) * (180.0f / 3.14159f);

        editorCamera.SetRotation(pitch, yaw);
    }

    void SceneViewPanel::DrawViewCube() {
        // Position in top-right corner
        const float size = 80.0f;
        const float padding = 10.0f;

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 cubePos(windowPos.x + windowSize.x - size - padding, windowPos.y + padding + 30.0f);

        // Set next window position
        ImGui::SetNextWindowPos(cubePos);
        ImGui::SetNextWindowSize(ImVec2(size, size));

        // Create a child window for the view cube
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 0.8f));

        if (ImGui::BeginChild("ViewCube", ImVec2(size, size), true, ImGuiWindowFlags_NoScrollbar)) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 center = ImGui::GetWindowPos() + ImVec2(size * 0.5f, size * 0.5f);

            // Get camera forward direction
            RTBEngine::Math::Vector3 camForward = editorCamera.GetForward();
            RTBEngine::Math::Vector3 camRight = editorCamera.GetRight();
            RTBEngine::Math::Vector3 camUp = editorCamera.GetUp();

            // Define axis directions and colors
            struct AxisInfo {
                RTBEngine::Math::Vector3 direction;
                const char* label;
                ImU32 color;
                ImU32 hoverColor;
            };

            AxisInfo axes[] = {
                // Primary axes
                { RTBEngine::Math::Vector3(1, 0, 0),  "X",  IM_COL32(220, 50, 50, 255),   IM_COL32(255, 100, 100, 255) },   // +X Right (Red)
                { RTBEngine::Math::Vector3(-1, 0, 0), "-X", IM_COL32(150, 30, 30, 255),   IM_COL32(200, 50, 50, 255) },     // -X Left
                { RTBEngine::Math::Vector3(0, 1, 0),  "Y",  IM_COL32(100, 220, 50, 255),  IM_COL32(150, 255, 100, 255) },   // +Y Up (Green)
                { RTBEngine::Math::Vector3(0, -1, 0), "-Y", IM_COL32(50, 150, 30, 255),   IM_COL32(100, 200, 50, 255) },    // -Y Down
                { RTBEngine::Math::Vector3(0, 0, 1),  "Z",  IM_COL32(50, 100, 220, 255),  IM_COL32(100, 150, 255, 255) },   // +Z Forward (Blue)
                { RTBEngine::Math::Vector3(0, 0, -1), "-Z", IM_COL32(30, 50, 150, 255),   IM_COL32(50, 100, 200, 255) }     // -Z Back
            };

            // Project axes to 2D and draw
            for (int i = 0; i < 6; i++) {
                const AxisInfo& axis = axes[i];

                // Project 3D direction to 2D screen space
                float dotForward = axis.direction.Dot(camForward);
                float dotRight = axis.direction.Dot(camRight);
                float dotUp = axis.direction.Dot(camUp);

                // Only draw visible faces (facing camera)
                if (dotForward < 0.1f) continue;

                // Calculate 2D position
                ImVec2 pos2D(
                    center.x + dotRight * 30.0f,
                    center.y - dotUp * 30.0f
                );

                // Check if mouse is hovering
                ImVec2 mousePos = ImGui::GetMousePos();
                float distToMouse = sqrtf(
                    (mousePos.x - pos2D.x) * (mousePos.x - pos2D.x) +
                    (mousePos.y - pos2D.y) * (mousePos.y - pos2D.y)
                );

                bool isHovered = distToMouse < 15.0f;
                ImU32 color = isHovered ? axis.hoverColor : axis.color;

                // Draw circle for axis
                drawList->AddCircleFilled(pos2D, 12.0f, color);
                drawList->AddCircle(pos2D, 12.0f, IM_COL32(255, 255, 255, 100), 12, 1.5f);

                // Draw label
                ImVec2 textSize = ImGui::CalcTextSize(axis.label);
                drawList->AddText(
                    ImVec2(pos2D.x - textSize.x * 0.5f, pos2D.y - textSize.y * 0.5f),
                    IM_COL32(255, 255, 255, 255),
                    axis.label
                );

                // Handle click
                if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    RTBEngine::Math::Vector3 up(0, 1, 0);
                    // If looking at top/bottom, use different up vector
                    if (fabsf(axis.direction.y) > 0.9f) {
                        up = RTBEngine::Math::Vector3(0, 0, -1);
                    }
                    AlignCameraToView(axis.direction, up);
                }
            }

            // Draw center sphere
            drawList->AddCircleFilled(center, 5.0f, IM_COL32(150, 150, 150, 255));
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

}
