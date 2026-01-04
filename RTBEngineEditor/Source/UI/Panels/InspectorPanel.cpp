#include "InspectorPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Reflection/TypeInfo.h>

namespace RTBEditor {

    InspectorPanel::InspectorPanel() {}
    InspectorPanel::~InspectorPanel() {}

    void InspectorPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Inspector");

        if (context.selectedGameObject) {
            auto& name = context.selectedGameObject->GetName();
            
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), name.c_str(), _TRUNCATE);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                context.selectedGameObject->SetName(buffer);
            }

            ImGui::Separator();

            DrawComponents(context.selectedGameObject);

            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                ImGui::OpenPopup("AddComponentPopup");
            }

            if (ImGui::BeginPopup("AddComponentPopup")) {
                auto types = RTBEngine::Reflection::TypeRegistry::GetInstance().GetRegisteredTypes();
                for (const auto& type : types) {
                    if (ImGui::MenuItem(type.c_str())) {
                        auto* newComp = RTBEngine::Reflection::TypeRegistry::GetInstance().CreateComponent(type);
                        if (newComp) {
                            context.selectedGameObject->AddComponent(newComp);
                        }
                    }
                }
                ImGui::EndPopup();
            }

            // Deferred component removal
            for (auto* comp : m_ComponentsToRemove) {
                context.selectedGameObject->RemoveComponent(comp);
            }
            m_ComponentsToRemove.clear();

        } else {
            ImGui::Text("Select a GameObject to see its properties.");
        }

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(RTBEngine::ECS::GameObject* gameObject) {
        // Transform Component (Special case as it's not a regular component list item)
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

        // Other Components
        for (const auto& component : gameObject->GetComponents()) {
            const RTBEngine::Reflection::TypeInfo* typeInfo = component->GetTypeInfo();
            const char* typeName = component->GetTypeName();

            ImGui::PushID(component.get());
            bool open = ImGui::CollapsingHeader(typeName, ImGuiTreeNodeFlags_DefaultOpen);
            
            // Context menu for component
            if (ImGui::BeginPopupContextItem("ComponentSettings")) {
                if (ImGui::MenuItem("Remove Component")) {
                    m_ComponentsToRemove.push_back(component.get());
                }
                ImGui::EndPopup();
            }

            if (open) {
                if (typeInfo) {
                    auto properties = typeInfo->GetInspectorProperties();
                    for (const auto* prop : properties) {
                        DrawProperty(component.get(), *prop);
                    }
                } else {
                    ImGui::TextDisabled("No reflection data available.");
                }
            }
            ImGui::PopID();
        }
    }

    void InspectorPanel::DrawProperty(RTBEngine::ECS::Component* component, const RTBEngine::Reflection::PropertyInfo& prop) {
        void* data = (char*)component + prop.offset;
        
        ImGui::PushID(prop.name.c_str());
        
        if (prop.IsReadOnly()) ImGui::BeginDisabled();

        switch (prop.type) {
            case RTBEngine::Reflection::PropertyType::Float: {
                float* val = (float*)data;
                if (prop.range) {
                    ImGui::SliderFloat(prop.displayName.c_str(), val, prop.range->min, prop.range->max);
                } else {
                    ImGui::DragFloat(prop.displayName.c_str(), val, 0.1f);
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::Int: {
                int* val = (int*)data;
                ImGui::DragInt(prop.displayName.c_str(), val, 1);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Bool: {
                bool* val = (bool*)data;
                ImGui::Checkbox(prop.displayName.c_str(), val);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector2: {
                float* val = (float*)data;
                ImGui::DragFloat2(prop.displayName.c_str(), val, 0.1f);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector3: {
                float* val = (float*)data;
                ImGui::DragFloat3(prop.displayName.c_str(), val, 0.1f);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector4: {
                float* val = (float*)data;
                ImGui::DragFloat4(prop.displayName.c_str(), val, 0.1f);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Color: {
                float* val = (float*)data;
                if (prop.size == sizeof(float) * 3) {
                    ImGui::ColorEdit3(prop.displayName.c_str(), val);
                } else {
                    ImGui::ColorEdit4(prop.displayName.c_str(), val);
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::String: {
                std::string* val = (std::string*)data;
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                strncpy_s(buffer, sizeof(buffer), val->c_str(), _TRUNCATE);
                if (ImGui::InputText(prop.displayName.c_str(), buffer, sizeof(buffer))) {
                    *val = buffer;
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::Enum: {
                // Simple combo for enums
                int* val = (int*)data;
                const char* previewValue = (size_t)*val < prop.enumNames.size() ? prop.enumNames[*val].c_str() : "Unknown";
                if (ImGui::BeginCombo(prop.displayName.c_str(), previewValue)) {
                    for (int i = 0; i < (int)prop.enumNames.size(); i++) {
                        bool isSelected = (*val == i);
                        if (ImGui::Selectable(prop.enumNames[i].c_str(), isSelected)) {
                            *val = i;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            default:
                ImGui::Text("%s: [Unsupported Type]", prop.displayName.c_str());
                break;
        }

        if (prop.tooltip) {
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", prop.tooltip->c_str());
            }
        }

        if (prop.IsReadOnly()) ImGui::EndDisabled();

        ImGui::PopID();
    }

}
