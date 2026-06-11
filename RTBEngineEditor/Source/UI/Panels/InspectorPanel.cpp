#include "InspectorPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <Windows.h>
#include <shellapi.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/ECS/ParticleSystem.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/Physics/PhysicsLayerSettings.h>
#include <RTBEngine/ECS/MissingComponent.h>
#include <RTBEngine/ECS/MeshRenderer.h>
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
#include <cctype>

namespace RTBEditor {

    namespace {
        struct CompatibleFbxScanResult {
            bool succeeded = false;
            std::vector<std::string> compatiblePaths;
            std::string status;
        };

        std::string MakeAssetReference(const std::filesystem::path& relativePath) {
            Project* project = Project::GetActiveProject();
            if (project) {
                return project->GetAssetReferencePath(relativePath);
            }
            return (std::filesystem::path("Assets") / relativePath).lexically_normal().generic_string();
        }

        std::string ToLowerCopy(std::string value) {
            std::transform(value.begin(), value.end(), value.begin(),
                [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            return value;
        }

        bool HasFbxExtension(const std::filesystem::path& path) {
            return ToLowerCopy(path.extension().string()) == ".fbx";
        }

        bool IsComponentInScene(const RTBEngine::ECS::Scene* scene,
                                const RTBEngine::ECS::Component* component) {
            if (!scene || !component) {
                return false;
            }

            for (const auto& gameObject : scene->GetGameObjects()) {
                if (!gameObject) {
                    continue;
                }

                const auto& components = gameObject->GetComponents();
                const bool found = std::any_of(
                    components.begin(),
                    components.end(),
                    [component](const RTBEngine::ECS::GameObject::ComponentPtr& candidate) {
                        return candidate.get() == component;
                    });

                if (found) {
                    return true;
                }
            }

            return false;
        }

        std::string StripClipVendorPrefix(const std::string& value) {
            const size_t pipe = value.find('|');
            return pipe != std::string::npos ? value.substr(pipe + 1) : value;
        }

        void ReleaseModelMeshes(RTBEngine::Rendering::ModelData& data) {
            for (RTBEngine::Rendering::Mesh* mesh : data.meshes) {
                delete mesh;
            }
            data.meshes.clear();
        }

        size_t CountClipMatchesSkeleton(const RTBEngine::Animation::Skeleton* targetSkeleton,
                                        const RTBEngine::Animation::AnimationClip* clip) {
            if (!targetSkeleton || !clip) {
                return 0;
            }

            size_t matchingBones = 0;
            for (size_t i = 0; i < targetSkeleton->GetBoneCount(); ++i) {
                const auto* targetBone = targetSkeleton->GetBone(static_cast<int>(i));
                if (targetBone && clip->HasBoneAnimation(targetBone->name)) {
                    ++matchingBones;
                }
            }

            return matchingBones;
        }

        bool IsAnimationDataCompatible(const RTBEngine::Animation::Skeleton* targetSkeleton,
                                       const RTBEngine::Rendering::ModelData& candidateData,
                                       std::string* reason = nullptr) {
            if (!targetSkeleton || targetSkeleton->GetBoneCount() == 0) {
                if (reason) *reason = "The current model has no skeleton.";
                return false;
            }
            if (candidateData.animations.empty()) {
                if (reason) *reason = "The candidate FBX has no animations.";
                return false;
            }

            const size_t minRequiredMatches = std::min<size_t>(
                12,
                std::max<size_t>(3, targetSkeleton->GetBoneCount() / 6));

            size_t bestMatchCount = 0;
            std::string bestClipName;

            for (const auto& clip : candidateData.animations) {
                if (!clip) {
                    continue;
                }

                const size_t matchingBones = CountClipMatchesSkeleton(targetSkeleton, clip.get());
                if (matchingBones > bestMatchCount) {
                    bestMatchCount = matchingBones;
                    bestClipName = clip->GetName();
                }

                if (matchingBones >= minRequiredMatches) {
                    if (reason) {
                        *reason = "Compatible clip '" + clip->GetName() + "' matches " +
                            std::to_string(matchingBones) + " bones.";
                    }
                    return true;
                }
            }

            if (reason) {
                if (bestClipName.empty()) {
                    *reason = "No clip in the candidate FBX matched the current skeleton.";
                } else {
                    *reason = "Best clip '" + bestClipName + "' only matches " +
                        std::to_string(bestMatchCount) + " bones.";
                }
            }
            return false;
        }

        void ReloadAnimatorClips(RTBEngine::Animation::Animator* animator) {
            if (!animator) {
                return;
            }

            animator->ClearClips();

            auto loadClipSource = [animator](const std::string& path) {
                if (path.empty()) {
                    return;
                }

                RTBEngine::Rendering::ModelData data =
                    RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(path);

                for (const auto& clip : data.animations) {
                    animator->AddClip(StripClipVendorPrefix(clip->GetName()), clip);
                }

                ReleaseModelMeshes(data);
            };

            loadClipSource(animator->modelRef);
            for (const auto& path : animator->additionalModels) {
                loadClipSource(path);
            }

            if (!animator->defaultClip.empty() && animator->GetClip(animator->defaultClip) == nullptr) {
                animator->defaultClip.clear();
            }

            if (!animator->currentClipName.empty() && animator->GetClip(animator->currentClipName) == nullptr) {
                animator->currentClipName.clear();
            }
        }

        CompatibleFbxScanResult FindCompatibleAnimationFbxPaths(const std::string& modelRef) {
            CompatibleFbxScanResult result;

            Project* project = Project::GetActiveProject();
            if (!project) {
                result.status = "No active project loaded.";
                return result;
            }

            if (modelRef.empty()) {
                result.status = "Set a Model first before scanning the project.";
                return result;
            }

            RTBEngine::Rendering::ModelData baseData =
                RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(modelRef);

            if (!baseData.skeleton || baseData.skeleton->GetBoneCount() == 0) {
                ReleaseModelMeshes(baseData);
                result.status = "The current model does not expose a valid skeleton.";
                return result;
            }

            const std::string normalizedModelRef =
                ToLowerCopy(std::filesystem::path(modelRef).lexically_normal().generic_string());

            size_t checkedCount = 0;
            size_t animationCount = 0;

            std::error_code ec;
            std::filesystem::recursive_directory_iterator it(
                project->GetAssetRootPath(),
                std::filesystem::directory_options::skip_permission_denied,
                ec);
            std::filesystem::recursive_directory_iterator end;

            for (; it != end; it.increment(ec)) {
                if (ec) {
                    ec.clear();
                    continue;
                }

                const auto& entry = *it;
                if (!entry.is_regular_file(ec) || ec) {
                    ec.clear();
                    continue;
                }
                if (!HasFbxExtension(entry.path())) {
                    continue;
                }

                const std::filesystem::path relativePath =
                    std::filesystem::relative(entry.path(), project->GetAssetRootPath(), ec);
                if (ec) {
                    ec.clear();
                    continue;
                }

                const std::string assetRef = project->GetAssetReferencePath(relativePath);
                if (ToLowerCopy(std::filesystem::path(assetRef).lexically_normal().generic_string()) == normalizedModelRef) {
                    continue;
                }

                ++checkedCount;

                RTBEngine::Rendering::ModelData candidateData =
                    RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(assetRef);

                if (!candidateData.animations.empty()) {
                    ++animationCount;

                    if (IsAnimationDataCompatible(baseData.skeleton.get(), candidateData)) {
                        result.compatiblePaths.push_back(assetRef);
                    }
                }

                ReleaseModelMeshes(candidateData);
            }

            ReleaseModelMeshes(baseData);

            std::sort(result.compatiblePaths.begin(), result.compatiblePaths.end());
            result.compatiblePaths.erase(
                std::unique(result.compatiblePaths.begin(), result.compatiblePaths.end()),
                result.compatiblePaths.end());

            std::ostringstream status;
            status << "Checked " << checkedCount << " FBX files";
            if (checkedCount > 0) {
                status << " (" << animationCount << " with animations)";
            }
            status << ". Found " << result.compatiblePaths.size() << " compatible files.";

            result.status = status.str();
            result.succeeded = true;
            return result;
        }

        bool IsScenePathProperty(RTBEngine::ECS::Component* component,
                                 const RTBEngine::Reflection::PropertyInfo& prop) {
            return component &&
                prop.name == "scenePath" &&
                std::string(component->GetTypeName()) == "SceneChangeButton";
        }

        std::string GetAssetRefTooltip(const std::string& assetType) {
            if (assetType == "fbx") {
                return "Drop an .fbx asset here.";
            }
            return "Drop a compatible asset here.";
        }

        void OpenGameScriptsProjectOrFile(const std::filesystem::path& fileToOpen) {
            Project* project = Project::GetActiveProject();
            const std::filesystem::path scriptsProjectPath =
                project ? project->GetGameScriptsProjectPath() : std::filesystem::path();

            if (!scriptsProjectPath.empty() && std::filesystem::exists(scriptsProjectPath)) {
                ShellExecuteA(nullptr, "open", scriptsProjectPath.string().c_str(), nullptr, nullptr, SW_SHOW);
                return;
            }

            RTB_ERROR("GameScripts project not found at: " +
                (scriptsProjectPath.empty() ? std::string("<empty path>") : scriptsProjectPath.string()));

            if (!fileToOpen.empty()) {
                ShellExecuteA(nullptr, "open", fileToOpen.string().c_str(), nullptr, nullptr, SW_SHOW);
            }
        }
    }

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

            bool isActive = context.selectedGameObject->IsActive();
            if (ImGui::Checkbox("##Active", &isActive)) {
                context.selectedGameObject->SetActive(isActive);
                RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Activar o desactivar el GameObject");
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), name.c_str(), _TRUNCATE);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                context.selectedGameObject->SetName(buffer);
                RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
            }

            {
                RTBEngine::Physics::PhysicsLayerSettings& layerSettings =
                    RTBEngine::Physics::PhysicsLayerSettings::Get();
                const int layerCount = layerSettings.GetLayerCount();
                const int currentLayer = context.selectedGameObject->GetCollisionLayer();
                const char* preview = layerSettings.GetLayerName(currentLayer).c_str();

                ImGui::SetNextItemWidth(180.0f);
                if (ImGui::BeginCombo("Collision layer", preview)) {
                    for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex) {
                        const bool selected = layerIndex == currentLayer;
                        const std::string& layerName = layerSettings.GetLayerName(layerIndex);
                        if (ImGui::Selectable(layerName.c_str(), selected)) {
                            context.selectedGameObject->SetCollisionLayer(layerIndex);
                            layerSettings.ApplyToGameObject(context.selectedGameObject);
                            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                        }

                        if (selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
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
            } else if (ext == ".texture") {
                DrawTextureAssetInspector(context.selectedAssetPath);
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
                RTBEngine::Math::Vector2 pos = uiElement->GetAnchoredPosition();
                if (ImGui::DragFloat2("Position", (float*)&pos, 1.0f)) {
                    uiElement->SetAnchoredPosition(pos);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                RTBEngine::Math::Vector2 anchorMin = uiElement->GetAnchorMin();
                if (ImGui::DragFloat2("Anchor Min", (float*)&anchorMin, 0.01f, 0.0f, 1.0f)) {
                    uiElement->SetAnchorMin(anchorMin);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                RTBEngine::Math::Vector2 anchorMax = uiElement->GetAnchorMax();
                if (ImGui::DragFloat2("Anchor Max", (float*)&anchorMax, 0.01f, 0.0f, 1.0f)) {
                    uiElement->SetAnchorMax(anchorMax);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                const bool isStretched =
                    anchorMin.x != anchorMax.x || anchorMin.y != anchorMax.y;

                RTBEngine::Math::Vector2 size = uiElement->GetSizeDelta();
                if (ImGui::DragFloat2(isStretched ? "Size Delta" : "Size", (float*)&size, 1.0f)) {
                    uiElement->SetSizeDelta(size);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                if (isStretched) {
                    ImGui::TextDisabled("Stretch: el tamano final viene de anchors + size delta.");
                }

                RTBEngine::Math::Vector2 pivot = uiElement->GetPivot();
                if (ImGui::DragFloat2("Pivot", (float*)&pivot, 0.01f, 0.0f, 1.0f)) {
                    uiElement->SetPivot(pivot);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                float rot = uiElement->GetRotation();
                if (ImGui::DragFloat("Rotation", &rot, 0.5f)) {
                    uiElement->SetRotation(rot);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                RTBEngine::Math::Vector2 scl = uiElement->GetScale();
                if (ImGui::DragFloat2("Scale", (float*)&scl, 0.01f)) {
                    uiElement->SetScale(scl);
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
                } else if (std::string(typeName) == "ParticleSystem") {
                    DrawParticleSystemComponent(static_cast<RTBEngine::ECS::ParticleSystem*>(component.get()));
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

    bool InspectorPanel::DrawSceneStringProperty(RTBEngine::ECS::Component* component,
                                                 const RTBEngine::Reflection::PropertyInfo& prop,
                                                 std::string* value) {
        if (!value) {
            return false;
        }

        bool changed = false;

        const char* labelName = prop.displayName == "scenePath"
            ? "Scene"
            : prop.displayName.c_str();
        ImGui::Text("%s:", labelName);
        ImGui::SameLine();

        const bool hasScene = !value->empty();
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            hasScene ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

        const std::string label = (hasScene ? *value : std::string("[None]")) + "##SceneDropArea";
        ImGui::Button(label.c_str(), ImVec2(220, 0));

        ImGui::PopStyleColor(4);

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", hasScene ? value->c_str() : "Drop a .lua scene here.");
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_SCENE)) {
                const ScenePayload* payloadData = static_cast<const ScenePayload*>(payload->Data);
                *value = MakeAssetReference(payloadData->path);
                changed = true;
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("...##SelectScene")) {
            assetBrowserModal->Open(
                AssetType::Scene,
                [component, value](const std::string& path) {
                    *value = MakeAssetReference(path);
                    component->OnValidate();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                },
                [component, value](const std::string& path) {
                    *value = std::filesystem::path(path).lexically_normal().generic_string();
                    component->OnValidate();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                });
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("X##ClearScene")) {
            value->clear();
            changed = true;
        }

        return changed;
    }

    bool InspectorPanel::DrawAssetRefProperty(RTBEngine::ECS::Component* component,
                                              const RTBEngine::Reflection::PropertyInfo& prop,
                                              std::string* value) {
        if (!component || !value) {
            return false;
        }

        const std::string assetType = ToLowerCopy(prop.assetType);
        const bool isFbxAsset = (assetType == "fbx");
        if (!isFbxAsset) {
            return false;
        }

        bool changed = false;

        ImGui::Text("%s:", prop.displayName.c_str());
        ImGui::SameLine();

        const bool hasAsset = !value->empty();
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            hasAsset ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

        const std::string label = (hasAsset ? *value : std::string("[None]")) + "##AssetRefDropArea";
        ImGui::Button(label.c_str(), ImVec2(220, 0));

        ImGui::PopStyleColor(4);

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", hasAsset ? value->c_str() : GetAssetRefTooltip(assetType).c_str());
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_MESH)) {
                const MeshPayload* payloadData = static_cast<const MeshPayload*>(payload->Data);
                const std::filesystem::path relativePath(payloadData->path);
                if (HasFbxExtension(relativePath)) {
                    *value = MakeAssetReference(relativePath);
                    changed = true;
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("...##SelectAssetRef")) {
            assetBrowserModal->Open(
                AssetType::Fbx,
                [component, value](const std::string& path) {
                    *value = MakeAssetReference(path);
                    component->OnValidate();
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                });
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("X##ClearAssetRef")) {
            value->clear();
            changed = true;
        }

        return changed;
    }

    void InspectorPanel::DrawProperty(RTBEngine::ECS::Component* component, const RTBEngine::Reflection::PropertyInfo& prop) {
        void* data = prop.GetMutableData(component);
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
                if (IsScenePathProperty(component, prop)) {
                    changed |= DrawSceneStringProperty(component, prop, val);
                } else {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    strncpy_s(buffer, sizeof(buffer), val->c_str(), _TRUNCATE);
                    if (ImGui::InputText(prop.displayName.c_str(), buffer, sizeof(buffer))) {
                        *val = buffer;
                        changed = true;
                    }
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::AssetRef: {
                std::string* val = (std::string*)data;
                if (DrawAssetRefProperty(component, prop, val)) {
                    changed = true;
                } else {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    strncpy_s(buffer, sizeof(buffer), val->c_str(), _TRUNCATE);
                    if (ImGui::InputText(prop.displayName.c_str(), buffer, sizeof(buffer))) {
                        *val = buffer;
                        changed = true;
                    }
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
                const bool useModelTexture = dynamic_cast<RTBEngine::ECS::MeshRenderer*>(component) != nullptr;
                auto loadTextureForInspector = [&](const std::string& path) -> RTBEngine::Rendering::Texture* {
                    auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
                    if (path.size() > 8 && path.substr(path.size() - 8) == ".texture") {
                        return rm.LoadTextureAsset(path);
                    }
                    return useModelTexture ? rm.LoadModelTexture(path) : rm.LoadTexture(path);
                };
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
                        auto* texture = loadTextureForInspector(fullPath);
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
                        [texPtr, loadTextureForInspector](const std::string& path) {
                            std::string fullPath = "Assets/" + path;
                            auto* tex = loadTextureForInspector(fullPath);
                            if (tex) { *texPtr = tex; RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty(); }
                        },
                        [texPtr, loadTextureForInspector](const std::string& path) {
                            auto* tex = loadTextureForInspector(path);
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
                RTBEngine::ECS::Scene* activeScene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
                const bool hasLiveGameObject = *goPtr && IsGameObjectInScene(activeScene, *goPtr);
                ImGui::Text("%s:", prop.displayName.c_str());
                ImGui::SameLine();

                // Text color based on state
                if (hasLiveGameObject) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f)); // Light blue
                } else if (*goPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.25f, 1.0f)); // Orange
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Invisible button for drag-drop area
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.5f, 0.8f, 0.5f));

                if (hasLiveGameObject) {
                    ImGui::Button((*goPtr)->GetName().c_str(), ImVec2(150, 0));
                } else if (*goPtr) {
                    ImGui::Button("[Missing]##GODropArea", ImVec2(150, 0));
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
                RTBEngine::ECS::Scene* activeScene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
                const bool hasLiveComponent = *compPtr && IsComponentInScene(activeScene, *compPtr);
                ImGui::Text("%s (%s):", prop.displayName.c_str(), prop.componentTypeName.c_str());
                ImGui::SameLine();

                // Text color based on state
                if (hasLiveComponent) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 1.0f, 1.0f)); // Purple
                } else if (*compPtr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.25f, 1.0f)); // Orange
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
                }

                // Invisible button for drag-drop area
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.8f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.3f, 0.8f, 0.5f));

                if (hasLiveComponent) {
                    std::string label = std::string("[") + (*compPtr)->GetTypeName() + "]##CompDropArea";
                    ImGui::Button(label.c_str(), ImVec2(150, 0));
                } else if (*compPtr) {
                    ImGui::Button("[Missing]##CompDropArea", ImVec2(150, 0));
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

        if (animatorScanTarget != animator) {
            animatorScanTarget = animator;
            animatorScanStatus.clear();
        }

        //Model path
        {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            strncpy_s(buf, sizeof(buf), animator->modelRef.c_str(), _TRUNCATE);
            if (ImGui::InputText("Model", buf, sizeof(buf))) {
                animator->modelRef = buf;
                animatorScanStatus.clear();
                changed = true;
            }
        }

        //Additional models list
        std::string totalAnimations = std::to_string(animator->additionalModels.size());
        std::string header = "Additional Models (" + totalAnimations + ")";

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.18f, 0.32f, 0.22f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.24f, 0.42f, 0.28f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.16f, 0.28f, 0.20f, 1.0f));
        if (ImGui::CollapsingHeader(header.c_str()))
        {
            std::vector<int> toRemove;
            //Populate the list with animation list
            for (int i = 0; i < static_cast<int>(animator->additionalModels.size()); i++) {
                ImGui::PushID(i);

                char buf[1024];
                memset(buf, 0, sizeof(buf));
                strncpy_s(buf, sizeof(buf), animator->additionalModels[i].c_str(), _TRUNCATE);

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 30.0f);
                if (ImGui::InputText("##addModel", buf, sizeof(buf))) {
                    animator->additionalModels[i] = buf;
                    animatorScanStatus.clear();
                    changed = true;
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("x")) {
                    toRemove.push_back(i);
                    changed = true;
                }
                ImGui::PopID();
            }

            //Remove animation from list
            for (int i = static_cast<int>(toRemove.size()) - 1; i >= 0; i--) {
                animator->additionalModels.erase(animator->additionalModels.begin() + toRemove[i]);
            }
        }

        ImGui::PopStyleColor(3);

        if (ImGui::Button("+ Add Model")) {
            animator->additionalModels.push_back("");
            animatorScanStatus.clear();
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload Clips")) {
            ReloadAnimatorClips(animator);
            animatorScanStatus = "Reloaded clips from the model and additional animation FBX files.";
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Auto Find Compatible FBX")) {
            CompatibleFbxScanResult scanResult = FindCompatibleAnimationFbxPaths(animator->modelRef);
            animatorScanStatus = scanResult.status;

            if (scanResult.succeeded) {
                animator->additionalModels = std::move(scanResult.compatiblePaths);
                ReloadAnimatorClips(animator);
                changed = true;
            }
        }

        if (!animatorScanStatus.empty()) {
            ImGui::Spacing();
            ImGui::TextWrapped("%s", animatorScanStatus.c_str());
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
            ImGui::BeginChild("LoadedClipsList", ImVec2(0, 180), true);
            for (const auto& name : clipNames) {
                ImGui::Text("%s", name.c_str());
            }
            ImGui::EndChild();
        }
        if (changed) {
            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
        }
    }

    void InspectorPanel::DrawParticleSystemComponent(RTBEngine::ECS::ParticleSystem* particleSystem) {
        if (!particleSystem) {
            return;
        }

        bool changed = false;

        if (ImGui::Button("Play")) {
            particleSystem->Play();
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            particleSystem->Pause();
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            particleSystem->Stop();
            changed = true;
        }

        ImGui::Spacing();
        if (ImGui::Button("Burst")) {
            particleSystem->Emit(particleSystem->burstCount);
            changed = true;
        }
        ImGui::SameLine();
        ImGui::Text("Count: %d", particleSystem->burstCount);

        ImGui::Spacing();
        ImGui::Text("Active particles: %d / %d",
            particleSystem->GetActiveParticleCount(),
            particleSystem->maxParticles);
        ImGui::Text("State: %s%s",
            particleSystem->IsPlaying() ? "Playing" : "Stopped",
            particleSystem->IsPaused() ? " (Paused)" : "");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const RTBEngine::Reflection::TypeInfo* typeInfo = particleSystem->GetTypeInfo();
        if (typeInfo) {
            auto properties = typeInfo->GetInspectorProperties();
            for (const auto* prop : properties) {
                DrawProperty(particleSystem, *prop);
            }
        }

        if (changed) {
            particleSystem->OnValidate();
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

                    cubemapFaces[i] = MakeAssetReference(data->path);
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
                        cubemapFaces[capturedIndex] = MakeAssetReference(path);
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

    void InspectorPanel::DrawTextureAssetInspector(const std::filesystem::path& texturePath) {
        // Reload cached data when a different file is selected
        if (textureEditorPath != texturePath) {
            textureEditorPath = texturePath;
            textureAssetImage.clear();
            textureAssetFlip = true;

            std::ifstream file(texturePath);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    if (line.empty() || line[0] == '#') continue;
                    auto sep = line.find('=');
                    if (sep == std::string::npos) continue;
                    std::string key = line.substr(0, sep);
                    std::string value = line.substr(sep + 1);
                    auto trim = [](std::string& s) {
                        size_t start = s.find_first_not_of(" \t\r\n");
                        size_t end = s.find_last_not_of(" \t\r\n");
                        s = (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
                    };
                    trim(key);
                    trim(value);
                    if (key == "image") textureAssetImage = value;
                    else if (key == "flip") textureAssetFlip = (value == "true" || value == "1");
                }
            }
        }

        ImGui::Text("Texture Asset");
        ImGui::Text("%s", texturePath.filename().string().c_str());
        ImGui::Separator();
        ImGui::Spacing();

        //Image path
        bool changed = false;
        {
            ImGui::Text("Image:");
            ImGui::SameLine();

            bool hasImage = !textureAssetImage.empty();
            ImGui::PushStyleColor(ImGuiCol_Text, hasImage
                ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f)
                : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

            std::string label = (hasImage
                ? std::filesystem::path(textureAssetImage).filename().string()
                : std::string("[None]")) + "##TextureAssetImage";
            ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 52.0f, 0));
            ImGui::PopStyleColor(4);

            // Drag-drop target for image files
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE)) {
                    const TexturePayload* data = static_cast<const TexturePayload*>(payload->Data);
                    textureAssetImage = MakeAssetReference(data->path);
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("...##BrowseTexImage")) {
                assetBrowserModal->Open(
                    AssetType::Texture,
                    [this](const std::string& path) {
                        textureAssetImage = MakeAssetReference(path);
                        SaveTextureAsset(textureEditorPath);
                    },
                    [this](const std::string& path) {
                        std::string fullPath = path;
                        for (char& c : fullPath) if (c == '\\') c = '/';
                        textureAssetImage = fullPath;
                        SaveTextureAsset(textureEditorPath);
                    }
                );
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("X##ClearTexImage")) {
                textureAssetImage.clear();
                changed = true;
            }
        }

        ImGui::Spacing();

        //Flip vertically
        if (ImGui::Checkbox("Flip Vertically", &textureAssetFlip)) {
            changed = true;
        }

        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0)) || changed) {
            SaveTextureAsset(texturePath);
        }

        // Preview the actual image if available
        if (!textureAssetImage.empty()) {
            auto* tex = RTBEngine::Core::ResourceManager::GetInstance().GetTexture(textureAssetImage);
            if (!tex) {
                tex = RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(textureAssetImage, textureAssetFlip);
            }
            if (tex) {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Preview:");
                float previewSize = ImGui::GetContentRegionAvail().x;
                float aspect = (float)tex->GetWidth() / (float)tex->GetHeight();
                ImGui::Image((ImTextureID)(intptr_t)tex->GetID(),
                    ImVec2(previewSize, previewSize / aspect),
                    ImVec2(0, 1), ImVec2(1, 0));
            }
        }
    }

    void InspectorPanel::SaveTextureAsset(const std::filesystem::path& texturePath) {
        std::ofstream file(texturePath);
        if (!file.is_open()) return;

        file << "image=" << textureAssetImage << "\n";
        file << "flip=" << (textureAssetFlip ? "true" : "false") << "\n";
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
            OpenGameScriptsProjectOrFile(scriptPath);
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
