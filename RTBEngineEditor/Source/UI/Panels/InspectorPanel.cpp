#include "InspectorPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <Windows.h>
#include <shellapi.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/MissingComponent.h>
#include <RTBEngine/Reflection/TypeInfo.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/UI/UIElement.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include "../DragDropPayloads.h"
#include "../Modals/AssetBrowserModal.h"
#include "../../Project/Project.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace RTBEditor {

    InspectorPanel::InspectorPanel() {
        assetBrowserModal = std::make_unique<AssetBrowserModal>();
    }

    InspectorPanel::~InspectorPanel() {}

    void InspectorPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Inspector");

        // Validate that the selected GOs still exist in the active scene
        auto* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (scene) {
            auto existsInScene = [scene](RTBEngine::ECS::GameObject* go) {
                if (!go) return false;
                for (const auto& obj : scene->GetGameObjects()) {
                    if (obj.get() == go) return true;
                }
                return false;
            };

            // Clean multi-selection list
            context.selectedGameObjects.erase(
                std::remove_if(
                    context.selectedGameObjects.begin(),
                    context.selectedGameObjects.end(),
                    [&](RTBEngine::ECS::GameObject* go) { return !existsInScene(go); }),
                context.selectedGameObjects.end());

            // Ensure primary selection is valid
            if (!existsInScene(context.selectedGameObject)) {
                context.selectedGameObject = context.selectedGameObjects.empty()
                    ? nullptr
                    : context.selectedGameObjects.front();
            }
        } else {
            ClearSelection(context);
        }

        if (context.selectedGameObjects.size() > 1) {
            ImGui::Text("Multiple GameObjects selected.");
            ImGui::TextDisabled("Modo multi-objeto: la edicion de propiedades no esta disponible.");
        } else if (context.selectedGameObject) {
            auto& name = context.selectedGameObject->GetName();
            
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), name.c_str(), _TRUNCATE);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                context.selectedGameObject->SetName(buffer);
                RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
            }

            if (context.selectedGameObject->IsPrefabInstance()) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), "Prefab: %s", context.selectedGameObject->GetPrefabName().c_str());
                ImGui::SameLine();
                if (ImGui::Button("Unlink")) {
                    context.selectedGameObject->SetPrefabName("");
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }
                ImGui::Spacing();
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
                            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                        }

                    }
                }
                ImGui::EndPopup();
            }

            // Deferred component removal
            for (auto* comp : componentsToRemove) {
                context.selectedGameObject->RemoveComponent(comp);
                RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
            }

            componentsToRemove.clear();

        } else if (!context.selectedAssetPath.empty()) {
            std::string ext = context.selectedAssetPath.extension().string();
            for (auto& c : ext) c = std::tolower(c);
            if (ext == ".cubemap") {
                DrawCubemapAssetInspector(context.selectedAssetPath);
            } else if (ext == ".h" || ext == ".cpp") {
                DrawScriptPreview(context.selectedAssetPath);
            } else if (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
                DrawFbxAssetInspector(context.selectedAssetPath);
            }
        } else {
            ImGui::Text("Select a GameObject to see its properties.");
        }

        // Render asset browser modal
        if (assetBrowserModal) {
            assetBrowserModal->Render();
        }

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(RTBEngine::ECS::GameObject* gameObject) {
        // Detect if this GameObject has any UIElement — if so show Rect Transform instead of Transform
        RTBEngine::UI::UIElement* uiElement = gameObject->GetComponent<RTBEngine::UI::UIElement>();

        if (uiElement) {
            // Rect Transform (replaces Transform for UI objects)
            if (ImGui::CollapsingHeader("Rect Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                RTBEngine::Math::Vector2 pos = uiElement->anchoredPosition;
                if (ImGui::DragFloat2("Position", (float*)&pos, 1.0f)) {
                    uiElement->anchoredPosition = pos;
                    uiElement->SyncRectTransform();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                RTBEngine::Math::Vector2 size = uiElement->sizeDelta;
                if (ImGui::DragFloat2("Size", (float*)&size, 1.0f)) {
                    uiElement->sizeDelta = size;
                    uiElement->SyncRectTransform();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                RTBEngine::Math::Vector2 anchorMin = uiElement->anchorMin;
                if (ImGui::DragFloat2("Anchor Min", (float*)&anchorMin, 0.01f, 0.0f, 1.0f)) {
                    uiElement->anchorMin = anchorMin;
                    uiElement->SyncRectTransform();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                RTBEngine::Math::Vector2 anchorMax = uiElement->anchorMax;
                if (ImGui::DragFloat2("Anchor Max", (float*)&anchorMax, 0.01f, 0.0f, 1.0f)) {
                    uiElement->anchorMax = anchorMax;
                    uiElement->SyncRectTransform();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }
            }
        } else {
            // Transform Component (3D — shown for non-UI objects)
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& transform = gameObject->GetTransform();

                // Local position — offset relative to parent
                RTBEngine::Math::Vector3 localPos = transform.GetPosition();
                if (ImGui::DragFloat3("Position", (float*)&localPos, 0.1f)) {
                    transform.SetPosition(localPos);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                // Local rotation — angles relative to parent axes
                constexpr float kRad2Deg = 180.0f / 3.14159265f;
                constexpr float kDeg2Rad = 3.14159265f / 180.0f;
                RTBEngine::Math::Quaternion localRot = transform.GetRotation();
                if (cachedRotationTarget != gameObject) {
                    cachedRotationTarget = gameObject;
                    RTBEngine::Math::Vector3 r = localRot.ToEulerAngles();
                    cachedRotationDeg = RTBEngine::Math::Vector3(r.x * kRad2Deg, r.y * kRad2Deg, r.z * kRad2Deg);
                }
                if (ImGui::DragFloat3("Rotation", (float*)&cachedRotationDeg, 0.5f, 0.0f, 0.0f, "%.1f°")) {
                    RTBEngine::Math::Vector3 newLocalRad(cachedRotationDeg.x * kDeg2Rad, cachedRotationDeg.y * kDeg2Rad, cachedRotationDeg.z * kDeg2Rad);
                    transform.SetRotation(RTBEngine::Math::Quaternion::FromEulerAngles(newLocalRad));
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }
                if (!ImGui::IsItemActive()) {
                    RTBEngine::Math::Vector3 r = transform.GetRotation().ToEulerAngles();
                    cachedRotationDeg = RTBEngine::Math::Vector3(r.x * kRad2Deg, r.y * kRad2Deg, r.z * kRad2Deg);
                }

                // Local scale — real world scale = localScale * parent.worldScale (handled by engine)
                RTBEngine::Math::Vector3 localScale = transform.GetScale();
                if (ImGui::DragFloat3("Scale", (float*)&localScale, 0.01f)) {
                    transform.SetScale(localScale);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }
            }
        }

        // Other Components
        for (const auto& component : gameObject->GetComponents()) {
            const RTBEngine::Reflection::TypeInfo* typeInfo = component->GetTypeInfo();
            const char* typeName = component->GetTypeName();

            // Missing component — display warning header
            auto* missing = dynamic_cast<RTBEngine::ECS::MissingComponent*>(component.get());
            if (missing) {
                ImGui::PushID(component.get());
                std::string header = "Missing: " + missing->GetMissingTypeName();
                ImGui::PushStyleColor(ImGuiCol_Header,        ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive,  ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                bool open = ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor(3);
                if (ImGui::BeginPopupContextItem("ComponentSettings")) {
                    if (ImGui::MenuItem("Remove Component")) {
                        componentsToRemove.push_back(component.get());
                    }
                    ImGui::EndPopup();
                }
                if (open) {
                    ImGui::TextDisabled("Script '%s' could not be found.", missing->GetMissingTypeName().c_str());
                }
                ImGui::PopID();
                continue;
            }

            ImGui::PushID(component.get());
            std::string displayName = FormatTypeName(typeName);
            bool open = ImGui::CollapsingHeader(displayName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            // Context menu for component
            if (ImGui::BeginPopupContextItem("ComponentSettings")) {
                if (ImGui::MenuItem("Remove Component")) {
                    componentsToRemove.push_back(component.get());
                }
                ImGui::EndPopup();
            }

            if (open) {
                if (std::string(typeName) == "Animator") {
                    DrawAnimatorComponent(static_cast<RTBEngine::Animation::Animator*>(component.get()));
                } else if (typeInfo) {
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
        bool changed = false;
        
        ImGui::PushID(prop.name.c_str());
        
        if (prop.IsReadOnly()) ImGui::BeginDisabled();

        switch (prop.type) {
            case RTBEngine::Reflection::PropertyType::Float: {
                float* val = (float*)data;
                if (prop.range) {
                    changed |= ImGui::SliderFloat(prop.displayName.c_str(), val, prop.range->minValue, prop.range->maxValue);
                } else {
                    changed |= ImGui::DragFloat(prop.displayName.c_str(), val, 0.1f);
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::Int: {
                int* val = (int*)data;
                changed |= ImGui::DragInt(prop.displayName.c_str(), val, 1);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Bool: {
                bool* val = (bool*)data;
                changed |= ImGui::Checkbox(prop.displayName.c_str(), val);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector2: {
                float* val = (float*)data;
                changed |= ImGui::DragFloat2(prop.displayName.c_str(), val, 0.1f);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector3: {
                float* val = (float*)data;
                changed |= ImGui::DragFloat3(prop.displayName.c_str(), val, 0.1f);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector4: {
                float* val = (float*)data;
                changed |= ImGui::DragFloat4(prop.displayName.c_str(), val, 0.1f);
                break;
            }
            case RTBEngine::Reflection::PropertyType::Color: {
                float* val = (float*)data;
                if (prop.size == sizeof(float) * 3) {
                    changed |= ImGui::ColorEdit3(prop.displayName.c_str(), val);
                } else {
                    changed |= ImGui::ColorEdit4(prop.displayName.c_str(), val);
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
                    changed = true;
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
                            changed = true;
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
                    auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
                    std::string path = rm.GetTexturePath((RTBEngine::Rendering::Texture*)*texPtr);
                    std::string label = (path.empty() ? "[Texture Set]" : path) + "##TextureDropArea";
                    ImGui::Button(label.c_str(), ImVec2(150, 0));
                }
                else {
                    ImGui::Button("[None]##TextureDropArea", ImVec2(150, 0));
                }

                ImGui::PopStyleColor(4); // Pop text color + button colors

                // Drag-and-drop target for textures (file path or embedded pointer)
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE)) {
                        const TexturePayload* payloadData = (const TexturePayload*)payload->Data;
                        std::string fullPath = std::string("Assets/") + payloadData->path;
                        auto* texture = RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(fullPath);
                        if (texture) {
                            *texPtr = texture;
                            changed = true;
                        }
                    }
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE_PTR)) {
                        const TexturePtrPayload* payloadData = (const TexturePtrPayload*)payload->Data;
                        if (payloadData->texturePtr) {
                            *texPtr = payloadData->texturePtr;
                            changed = true;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectTexture")) {
                    assetBrowserModal->Open(
                        AssetType::Texture,
                        [texPtr](const std::string& path) {
                            auto* tex = RTBEngine::Core::ResourceManager::GetInstance().LoadTexture("Assets/" + path);
                            if (tex) { *texPtr = tex; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        },
                        [texPtr](const std::string& path) {
                            auto* tex = RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(path);
                            if (tex) { *texPtr = tex; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        }
                    );
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearTexture")) {
                    *texPtr = nullptr;
                    changed = true;
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
                    auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
                    std::string path = rm.GetAudioClipPath((RTBEngine::Audio::AudioClip*)*clipPtr);
                    std::string label = (path.empty() ? "[AudioClip Set]" : path) + "##AudioDropArea";
                    ImGui::Button(label.c_str(), ImVec2(150, 0));
                }
                else {
                    ImGui::Button("[None]##AudioDropArea", ImVec2(150, 0));
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
                            changed = true;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectAudioClip")) {
                    assetBrowserModal->Open(AssetType::AudioClip, [clipPtr](const std::string& path) {
                        std::string fullPath = std::string("Assets/") + path;
                        auto* audioClip = RTBEngine::Core::ResourceManager::GetInstance().LoadAudioClip(fullPath);
                        if (audioClip) {
                            *clipPtr = audioClip;
                            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                        }
                    });
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearAudioClip")) {
                    *clipPtr = nullptr;
                    changed = true;
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
                    auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
                    std::string path = rm.GetMeshPath((RTBEngine::Rendering::Mesh*)*meshPtr);
                    std::string label = (path.empty() ? "[Mesh Set]" : path) + "##MeshDropArea";
                    ImGui::Button(label.c_str(), ImVec2(150, 0));
                }
                else {
                    ImGui::Button("[None]##MeshDropArea", ImVec2(150, 0));
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
                            changed = true;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectMesh")) {
                    assetBrowserModal->Open(
                        AssetType::Mesh,
                        [meshPtr](const std::string& path) {
                            auto* mesh = RTBEngine::Core::ResourceManager::GetInstance().LoadModel("Assets/" + path);
                            if (mesh) { *meshPtr = mesh; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        },
                        [meshPtr](const std::string& path) {
                            auto* mesh = RTBEngine::Core::ResourceManager::GetInstance().LoadModel(path);
                            if (mesh) { *meshPtr = mesh; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        }
                    );
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearMesh")) {
                    *meshPtr = nullptr;
                    changed = true;
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
                    auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
                    std::string path = rm.GetFontPath((RTBEngine::Rendering::Font*)*fontPtr);
                    std::string label = (path.empty() ? "[Font Set]" : path) + "##FontDropArea";
                    ImGui::Button(label.c_str(), ImVec2(150, 0));
                } else {
                    ImGui::Button("[None]##FontDropArea", ImVec2(150, 0));
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
                            changed = true;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("...##SelectFont")) {
                    assetBrowserModal->Open(
                        AssetType::Font,
                        [fontPtr](const std::string& path) {
                            float sizes[] = { 12.0f, 16.0f, 18.0f, 24.0f, 32.0f, 48.0f };
                            auto* font = RTBEngine::Core::ResourceManager::GetInstance().LoadFont("Assets/" + path, sizes, 6);
                            if (font) { *fontPtr = font; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        },
                        [fontPtr](const std::string& path) {
                            float sizes[] = { 12.0f, 16.0f, 18.0f, 24.0f, 32.0f, 48.0f };
                            auto* font = RTBEngine::Core::ResourceManager::GetInstance().LoadFont(path, sizes, 6);
                            if (font) { *fontPtr = font; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        }
                    );
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearFont")) {
                    *fontPtr = nullptr;
                    changed = true;
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
                            changed = true;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearGameObject")) {
                    *goPtr = nullptr;
                    changed = true;
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
                                    changed = true;
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
                    changed = true;
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

        if (changed) {
            component->OnValidate();
            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
        }

        ImGui::PopID();
    }

    void InspectorPanel::DrawAnimatorComponent(RTBEngine::Animation::Animator* animator) {
        bool changed = false;

        //Model path
        {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            strncpy_s(buf, sizeof(buf), animator->modelRef.c_str(), _TRUNCATE);
            if (ImGui::InputText("Model", buf, sizeof(buf))) {
                animator->modelRef = buf;
                changed = true;
            }
        }

        //Additional models list
        ImGui::Text("Additional Models");
        std::vector<int> toRemove;
        for (int i = 0; i < static_cast<int>(animator->additionalModels.size()); i++) {
            ImGui::PushID(i);
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            strncpy_s(buf, sizeof(buf), animator->additionalModels[i].c_str(), _TRUNCATE);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 30.0f);
            if (ImGui::InputText("##addModel", buf, sizeof(buf))) {
                animator->additionalModels[i] = buf;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("x")) {
                toRemove.push_back(i);
                changed = true;
            }
            ImGui::PopID();
        }
        for (int i = static_cast<int>(toRemove.size()) - 1; i >= 0; i--) {
            animator->additionalModels.erase(animator->additionalModels.begin() + toRemove[i]);
        }
        if (ImGui::Button("+ Add Model")) {
            animator->additionalModels.push_back("");
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload Clips")) {
            // Reload animation clips from all model paths (meshes from additional files are discarded)
            auto StripPrefix = [](const std::string& s) -> std::string {
                size_t pipe = s.find('|');
                return (pipe != std::string::npos) ? s.substr(pipe + 1) : s;
            };
            // Primary model: take clips only, free the re-loaded mesh duplicates
            if (!animator->modelRef.empty()) {
                RTBEngine::Rendering::ModelData data = RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(animator->modelRef);
                for (const auto& clip : data.animations) {
                    animator->AddClip(StripPrefix(clip->GetName()), clip);
                }
                for (RTBEngine::Rendering::Mesh* mesh : data.meshes) {
                    delete mesh;
                }
            }
            // Additional models: take clips only, free meshes
            for (const auto& path : animator->additionalModels) {
                if (path.empty()) continue;
                RTBEngine::Rendering::ModelData data = RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(path);
                for (const auto& clip : data.animations) {
                    animator->AddClip(StripPrefix(clip->GetName()), clip);
                }
                for (RTBEngine::Rendering::Mesh* mesh : data.meshes) {
                    delete mesh;
                }
            }
            changed = true;
        }

        //Default clip combo
        {
            std::vector<std::string> clipNames = animator->GetClipNames();
            std::string currentDefault = animator->defaultClip;
            const char* previewValue = currentDefault.empty() ? "(none)" : currentDefault.c_str();
            if (ImGui::BeginCombo("Default Clip", previewValue)) {
                if (ImGui::Selectable("(none)", currentDefault.empty())) {
                    animator->defaultClip.clear();
                    changed = true;
                }
                for (const auto& name : clipNames) {
                    bool isSelected = (name == currentDefault);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        animator->defaultClip = name;
                        changed = true;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        //Speed slider
        if (ImGui::SliderFloat("Speed", &animator->speed, 0.0f, 3.0f)) {
            animator->SetSpeed(animator->speed);
            changed = true;
        }

        //Playing and looping checkboxes
        if (ImGui::Checkbox("Playing", &animator->playing)) changed = true;
        ImGui::SameLine();
        if (ImGui::Checkbox("Looping", &animator->looping)) changed = true;

        //Playback buttons
        ImGui::Spacing();
        if (ImGui::Button("Play")) {
            const std::string& clip = animator->defaultClip.empty()
                ? animator->GetCurrentClipName()
                : animator->defaultClip;
            if (!clip.empty()) animator->Play(clip, animator->looping);
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            animator->Stop();
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            animator->Pause();
            changed = true;
        }

        //Loaded clips read-only list
        ImGui::Spacing();
        ImGui::TextDisabled("Loaded Clips");
        ImGui::Separator();
        std::vector<std::string> clipNames = animator->GetClipNames();
        if (clipNames.empty()) {
            ImGui::TextDisabled("(none)");
        } else {
            for (const auto& name : clipNames) {
                ImGui::Text("%s", name.c_str());
            }
        }

        if (changed) {
            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
        }
    }

    std::string InspectorPanel::FormatTypeName(const char* typeName) {
        std::string result;
        for (int i = 0; typeName[i] != '\0'; i++) {
            if (i > 0 && std::isupper((unsigned char)typeName[i]) && std::islower((unsigned char)typeName[i - 1])) {
                result += ' ';
            }
            result += typeName[i];
        }
        return result;
    }

    void InspectorPanel::DrawCubemapAssetInspector(const std::filesystem::path& cubemapPath) {
        // Reload cached data when a different file is selected
        if (cubemapEditorPath != cubemapPath) {
            cubemapEditorPath = cubemapPath;
            cubemapFaces.fill(std::string());

            std::ifstream file(cubemapPath);
            if (file.is_open()) {
                static const char* faceKeys[] = { "right", "left", "top", "bottom", "front", "back" };
                std::string line;
                while (std::getline(file, line)) {
                    auto sep = line.find('=');
                    if (sep == std::string::npos) continue;
                    std::string key   = line.substr(0, sep);
                    std::string value = line.substr(sep + 1);
                    auto trim = [](std::string& s) {
                        size_t start = s.find_first_not_of(" \t\r\n");
                        size_t end   = s.find_last_not_of(" \t\r\n");
                        s = (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
                    };
                    trim(key);
                    trim(value);
                    for (int i = 0; i < 6; ++i) {
                        if (key == faceKeys[i]) {
                            cubemapFaces[i] = value;
                            break;
                        }
                    }
                }
            }
        }

        ImGui::Text("Cubemap Asset");
        ImGui::Text("%s", cubemapPath.filename().string().c_str());
        ImGui::Separator();
        ImGui::Spacing();

        static const char* faceLabels[] = { "Right (+X)", "Left  (-X)", "Top   (+Y)", "Bottom(-Y)", "Front (+Z)", "Back  (-Z)" };
        static const char* faceIds[]    = { "##right", "##left", "##top", "##bottom", "##front", "##back" };

        bool changed = false;
        for (int i = 0; i < 6; ++i) {
            ImGui::Text("%s", faceLabels[i]);
            ImGui::SameLine();

            bool hasTexture = !cubemapFaces[i].empty();
            ImGui::PushStyleColor(ImGuiCol_Text, hasTexture
                ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f)
                : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

            std::string label = (hasTexture
                ? std::filesystem::path(cubemapFaces[i]).filename().string()
                : std::string("[None]")) + faceIds[i];
            ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 52.0f, 0));
            ImGui::PopStyleColor(4);

            // Drag-drop target for texture
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE)) {
                    const TexturePayload* data = static_cast<const TexturePayload*>(payload->Data);

                    std::filesystem::path assetRoot = Project::GetActiveProject()
                        ? Project::GetActiveProject()->GetAssetDirectory()
                        : std::filesystem::path("Assets");
                    std::string fullPath = (assetRoot / data->path).string();
                    for (char& c : fullPath) if (c == '\\') c = '/';
                    cubemapFaces[i] = fullPath;
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::SameLine();
            ImGui::PushID(i);

            // Browse button — opens asset browser modal
            if (ImGui::SmallButton("...")) {
                int capturedIndex = i;
                assetBrowserModal->Open(
                    AssetType::Texture,
                    [this, capturedIndex](const std::string& path) {
                        // path is relative to Assets/
                        std::filesystem::path assetRoot = Project::GetActiveProject()
                            ? Project::GetActiveProject()->GetAssetDirectory()
                            : std::filesystem::path("Assets");
                        std::string fullPath = (assetRoot / path).string();
                        for (char& c : fullPath) if (c == '\\') c = '/';
                        cubemapFaces[capturedIndex] = fullPath;
                        SaveCubemapAsset(cubemapEditorPath);
                    },
                    [this, capturedIndex](const std::string& path) {
                        // path is a Default/... path
                        std::string fullPath = path;
                        for (char& c : fullPath) if (c == '\\') c = '/';
                        cubemapFaces[capturedIndex] = fullPath;
                        SaveCubemapAsset(cubemapEditorPath);
                    }
                );
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("X")) {
                cubemapFaces[i].clear();
                changed = true;
            }
            ImGui::PopID();
        }

        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0)) || changed) {
            SaveCubemapAsset(cubemapPath);
        }
    }

    void InspectorPanel::SaveCubemapAsset(const std::filesystem::path& cubemapPath) {
        std::ofstream file(cubemapPath);
        if (!file.is_open()) return;

        static const char* faceKeys[] = { "right", "left", "top", "bottom", "front", "back" };
        for (int i = 0; i < 6; ++i) {
            file << faceKeys[i] << "=" << cubemapFaces[i] << "\n";
        }
    }

    void InspectorPanel::DrawScriptPreview(const std::filesystem::path& scriptPath) {
        // Reload file content when selection changes
        if (scriptPreviewPath != scriptPath) {
            scriptPreviewPath = scriptPath;
            scriptPreviewContent.clear();
            std::ifstream file(scriptPath);
            if (file.is_open()) {
                std::ostringstream ss;
                ss << file.rdbuf();
                scriptPreviewContent = ss.str();
            }
        }

        std::string ext = scriptPath.extension().string();
        for (auto& c : ext) c = std::tolower(c);

        ImGui::Text("%s", scriptPath.filename().string().c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", ext == ".h" ? "Header" : "Source");
        ImGui::Separator();
        ImGui::Spacing();

        // Open script in the associated C++ project/solution so that includes and
        // IntelliSense work correctly, falling back to just the file if needed.
        if (ImGui::Button("Open in Editor")) {
            std::filesystem::path solutionPath =
                std::filesystem::current_path() / "RTBEngineEditor.sln";

            if (std::filesystem::exists(solutionPath)) {
                ShellExecuteA(
                    nullptr,
                    "open",
                    solutionPath.string().c_str(),
                    nullptr,
                    nullptr,
                    SW_SHOW
                );
            }
            else {
                ShellExecuteA(nullptr, "open", scriptPath.string().c_str(), nullptr, nullptr, SW_SHOW);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Read-only scrollable code preview
        ImVec2 available = ImGui::GetContentRegionAvail();
        ImGui::InputTextMultiline(
            "##ScriptPreview",
            const_cast<char*>(scriptPreviewContent.c_str()),
            scriptPreviewContent.size() + 1,
            ImVec2(available.x, available.y - 4.0f),
            ImGuiInputTextFlags_ReadOnly
        );
    }

    void InspectorPanel::DrawFbxAssetInspector(const std::filesystem::path& fbxPath)
    {
        ImGui::Text("%s", fbxPath.filename().string().c_str());
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Click the model in the Content Browser");
        ImGui::TextDisabled("to expand and drag its textures.");
    }
}
