#include "InspectorPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

namespace RTBEditor {

    InspectorPanel::InspectorPanel() {}
    InspectorPanel::~InspectorPanel() {}

    void InspectorPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Inspector");

        if (context.selectedGameObject) {
            auto& name = context.selectedGameObject->GetName();
            ImGui::Text("Name: %s", name.c_str());
            ImGui::Separator();

            DrawComponents(context.selectedGameObject);
        } else {
            ImGui::Text("Select a GameObject to see its properties.");
        }

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(RTBEngine::ECS::GameObject* gameObject) {
        // Transform Component
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& transform = gameObject->GetTransform();
            
            RTBEngine::Math::Vector3 pos = transform.GetPosition();
            if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f)) {
                transform.SetPosition(pos);
            }

            RTBEngine::Math::Vector3 rot = transform.GetRotation().ToEulerAngles();
            if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.1f)) {
                transform.SetRotation(rot);
            }

            RTBEngine::Math::Vector3 scale = transform.GetScale();
            if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f)) {
                transform.SetScale(scale);
            }
        }
    }

}
