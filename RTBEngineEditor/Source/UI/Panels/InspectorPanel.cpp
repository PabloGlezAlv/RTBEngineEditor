#include "InspectorPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Reflection/TypeInfo.h>
#include <RTBEngine/Core/ResourceManager.h>
#include "../DragDropPayloads.h"
#include "../Modals/AssetBrowserModal.h"

namespace RTBEditor {

    InspectorPanel::InspectorPanel() {
        m_AssetBrowserModal = std::make_unique<AssetBrowserModal>();
    }

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

        // Render asset browser modal
        if (m_AssetBrowserModal) {
            m_AssetBrowserModal->Render();
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
            case RTBEngine::Reflection::PropertyType::TextureRef: {
                void** texPtr = (void**)data;
                ImGui::Text("%s:", prop.displayName.c_str());
                ImGui::SameLine();

                // Color del texto seg�n el estado
                if (*texPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)); // Green
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Bot�n invisible para drag-drop
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

                if (*texPtr) {
                    ImGui::Button("[Texture Set]##TextureDropArea", ImVec2(100, 0));
                }
                else {
                    ImGui::Button("[None]##TextureDropArea", ImVec2(100, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for textures
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE)) {
                        const TexturePayload* payloadData = (const TexturePayload*)payload->Data;
                        std::string fullPath = std::string("Assets/") + payloadData->path;
                        auto* texture = RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(fullPath);
                        if (texture) {
                            *texPtr = texture;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectTexture")) {
                    m_AssetBrowserModal->Open(AssetType::Texture, [texPtr](const std::string& path) {
                        std::string fullPath = std::string("Assets/") + path;
                        auto* texture = RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(fullPath);
                        if (texture) {
                            *texPtr = texture;
                        }
                    });
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearTexture")) {
                    *texPtr = nullptr;
                }
                break;
            }

            case RTBEngine::Reflection::PropertyType::AudioClipRef: {
                void** clipPtr = (void**)data;
                ImGui::Text("%s:", prop.displayName.c_str());
                ImGui::SameLine();

                // Color del texto seg�n el estado
                if (*clipPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)); // Green
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Bot�n invisible para drag-drop
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

                if (*clipPtr) {
                    ImGui::Button("[AudioClip Set]##AudioDropArea", ImVec2(120, 0));
                }
                else {
                    ImGui::Button("[None]##AudioDropArea", ImVec2(120, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for audio clips
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_AUDIOCLIP)) {
                        const AudioClipPayload* payloadData = (const AudioClipPayload*)payload->Data;
                        std::string fullPath = std::string("Assets/") + payloadData->path;
                        auto* audioClip = RTBEngine::Core::ResourceManager::GetInstance().LoadAudioClip(fullPath);
                        if (audioClip) {
                            *clipPtr = audioClip;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectAudioClip")) {
                    m_AssetBrowserModal->Open(AssetType::AudioClip, [clipPtr](const std::string& path) {
                        std::string fullPath = std::string("Assets/") + path;
                        auto* audioClip = RTBEngine::Core::ResourceManager::GetInstance().LoadAudioClip(fullPath);
                        if (audioClip) {
                            *clipPtr = audioClip;
                        }
                    });
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearAudioClip")) {
                    *clipPtr = nullptr;
                }
                break;
            }

            case RTBEngine::Reflection::PropertyType::MeshRef: {
                void** meshPtr = (void**)data;
                ImGui::Text("%s:", prop.displayName.c_str());
                ImGui::SameLine();

                // Color del texto seg�n el estado
                if (*meshPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)); // Green
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Bot�n invisible para drag-drop
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

                if (*meshPtr) {
                    ImGui::Button("[Mesh Set]##MeshDropArea", ImVec2(100, 0));
                }
                else {
                    ImGui::Button("[None]##MeshDropArea", ImVec2(100, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for meshes
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_MESH)) {
                        const MeshPayload* payloadData = (const MeshPayload*)payload->Data;
                        std::string fullPath = std::string("Assets/") + payloadData->path;
                        auto* mesh = RTBEngine::Core::ResourceManager::GetInstance().LoadModel(fullPath);
                        if (mesh) {
                            *meshPtr = mesh;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectMesh")) {
                    m_AssetBrowserModal->Open(AssetType::Mesh, [meshPtr](const std::string& path) {
                        std::string fullPath = std::string("Assets/") + path;
                        auto* mesh = RTBEngine::Core::ResourceManager::GetInstance().LoadModel(fullPath);
                        if (mesh) {
                            *meshPtr = mesh;
                        }
                    });
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearMesh")) {
                    *meshPtr = nullptr;
                }
                break;
            }

            case RTBEngine::Reflection::PropertyType::FontRef: {
                void** fontPtr = (void**)data;
                ImGui::Text("%s:", prop.displayName.c_str());
                ImGui::SameLine();

                // Text color based on state
                if (*fontPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)); // Green
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Invisible button for drag-drop area
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

                if (*fontPtr) {
                    ImGui::Button("[Font Set]##FontDropArea", ImVec2(100, 0));
                } else {
                    ImGui::Button("[None]##FontDropArea", ImVec2(100, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for fonts
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_FONT)) {
                        const FontPayload* payloadData = (const FontPayload*)payload->Data;
                        std::string fullPath = std::string("Assets/") + payloadData->path;

                        // Default font sizes
                        float sizes[] = { 12.0f, 16.0f, 18.0f, 24.0f, 32.0f, 48.0f };
                        auto* font = RTBEngine::Core::ResourceManager::GetInstance().LoadFont(fullPath, sizes, 6);
                        if (font) {
                            *fontPtr = font;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectFont")) {
                    m_AssetBrowserModal->Open(AssetType::Font, [fontPtr](const std::string& path) {
                        std::string fullPath = std::string("Assets/") + path;

                        // Default font sizes
                        float sizes[] = { 12.0f, 16.0f, 18.0f, 24.0f, 32.0f, 48.0f };
                        auto* font = RTBEngine::Core::ResourceManager::GetInstance().LoadFont(fullPath, sizes, 6);
                        if (font) {
                            *fontPtr = font;
                        }
                    });
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearFont")) {
                    *fontPtr = nullptr;
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::GameObjectRef: {
                RTBEngine::ECS::GameObject** goPtr = (RTBEngine::ECS::GameObject**)data;
                ImGui::Text("%s:", prop.displayName.c_str());
                ImGui::SameLine();

                // Text color based on state
                if (*goPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f)); // Light blue
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Invisible button for drag-drop area
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.5f, 0.8f, 0.5f));

                if (*goPtr) {
                    ImGui::Button((*goPtr)->GetName().c_str(), ImVec2(150, 0));
                } else {
                    ImGui::Button("[None]##GODropArea", ImVec2(150, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for GameObjects
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                        const GameObjectPayload* payloadData = (const GameObjectPayload*)payload->Data;
                        RTBEngine::ECS::GameObject* draggedGameObject = reinterpret_cast<RTBEngine::ECS::GameObject*>(payloadData->gameObjectId);
                        if (draggedGameObject) {
                            *goPtr = draggedGameObject;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearGameObject")) {
                    *goPtr = nullptr;
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::ComponentRef: {
                RTBEngine::ECS::Component** compPtr = (RTBEngine::ECS::Component**)data;
                ImGui::Text("%s (%s):", prop.displayName.c_str(), prop.componentTypeName.c_str());
                ImGui::SameLine();

                // Text color based on state
                if (*compPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 1.0f, 1.0f)); // Purple
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Invisible button for drag-drop area
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.3f, 0.8f, 0.5f));

                if (*compPtr) {
                    std::string label = std::string("[") + (*compPtr)->GetTypeName() + "]##CompDropArea";
                    ImGui::Button(label.c_str(), ImVec2(150, 0));
                } else {
                    ImGui::Button("[None]##CompDropArea", ImVec2(150, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for GameObject (will extract component)
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                        const GameObjectPayload* payloadData = (const GameObjectPayload*)payload->Data;
                        RTBEngine::ECS::GameObject* draggedGameObject = reinterpret_cast<RTBEngine::ECS::GameObject*>(payloadData->gameObjectId);

                        if (draggedGameObject) {
                            // Try to find component of the specified type from GameObject
                            bool foundComponent = false;
                            const auto& components = draggedGameObject->GetComponents();
                            for (const auto& comp : components) {
                                if (std::string(comp->GetTypeName()) == prop.componentTypeName) {
                                    *compPtr = comp.get();
                                    foundComponent = true;
                                    break;
                                }
                            }

                            // Visual feedback if component not found
                            if (!foundComponent) {
                                ImGui::SetTooltip("GameObject '%s' doesn't have a %s component",
                                    draggedGameObject->GetName().c_str(),
                                    prop.componentTypeName.c_str());
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearComponent")) {
                    *compPtr = nullptr;
                }

                // Help text
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Drag a GameObject with a %s component", prop.componentTypeName.c_str());
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
