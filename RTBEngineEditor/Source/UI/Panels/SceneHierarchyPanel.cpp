#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>
#include "../DragDropPayloads.h"

namespace RTBEditor {

    SceneHierarchyPanel::SceneHierarchyPanel() {}
    SceneHierarchyPanel::~SceneHierarchyPanel() {}

    void SceneHierarchyPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Hierarchy");

        auto activeScene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (activeScene) {
            for (const auto& gameObject : activeScene->GetGameObjects()) {
                // Only start drawing from root objects (those without a parent)
                if (gameObject->GetParent() == nullptr) {
                    DrawGameObjectNode(gameObject.get(), context);
                }
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
                context.selectedGameObject = nullptr;
            }

            // Context menu for the window
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::MenuItem("Create Empty Object")) {
                    // TODO: Scene object creation
                }
                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawGameObjectNode(RTBEngine::ECS::GameObject* gameObject, EditorContext& context) {
        auto& name = gameObject->GetName();

        ImGuiTreeNodeFlags flags = ((context.selectedGameObject == gameObject) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        
        const auto& children = gameObject->GetChildren();
        if (children.empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        // Using pointer as ID to ensure uniqueness
        bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, name.c_str());

        if (ImGui::IsItemClicked()) {
            context.selectedGameObject = gameObject;
        }

        // Drag-and-drop source for GameObject
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            GameObjectPayload payload;
            payload.gameObjectId = reinterpret_cast<uint64_t>(gameObject); // Store pointer as ID
            ImGui::SetDragDropPayload(PAYLOAD_GAMEOBJECT, &payload, sizeof(GameObjectPayload));
            ImGui::Text("GameObject: %s", name.c_str());
            ImGui::EndDragDropSource();
        }

        if (opened) {
            for (auto* child : children) {
                DrawGameObjectNode(child, context);
            }
            ImGui::TreePop();
        }
    }

}
