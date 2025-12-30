#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>

namespace RTBEditor {

    SceneHierarchyPanel::SceneHierarchyPanel() {}
    SceneHierarchyPanel::~SceneHierarchyPanel() {}

    void SceneHierarchyPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Hierarchy");

        auto activeScene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (activeScene) {
            for (const auto& gameObject : activeScene->GetGameObjects()) {
                DrawGameObjectNode(gameObject.get(), context);
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
        
        // Using pointer as ID to ensure uniqueness
        bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, name.c_str());

        if (ImGui::IsItemClicked()) {
            context.selectedGameObject = gameObject;
        }

        if (opened) {
            // Recurse for children if the engine supports it in the future
            // For now, objects are in a flat list in Scene
            ImGui::TreePop();
        }
    }

}
