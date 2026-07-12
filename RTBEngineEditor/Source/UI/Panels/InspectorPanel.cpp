#include "InspectorPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <Windows.h>
#include <shellapi.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Scene/ParticleSystem.h>
#include <RTBEngine/Scene/NavGridComponent.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Physics/PhysicsLayerSettings.h>
#include <RTBEngine/Scene/MissingComponent.h>
#include <RTBEngine/Scene/MeshRenderer.h>
#include <RTBEngine/Scene/BoxColliderComponent.h>
#include <RTBEngine/Reflection/TypeInfo.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/ShaderAsset.h>
#include <RTBEngine/Rendering/ShaderProperties.h>
#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Scripting/DataAssetSaver.h>
#include <RTBEngine/UI/UIElement.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include "../DragDropPayloads.h"
#include "../Modals/AssetBrowserModal.h"
#include "../../Project/Project.h"
#include <RTBEngine/Reflection/ListPropertyAccess.h>
#include <RTBEngine/Reflection/NameFormatting.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <vector>

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

        std::filesystem::path GetProjectAssetRoot() {
            Project* project = Project::GetActiveProject();
            if (project) {
                return project->GetAssetRootPath();
            }
            return std::filesystem::path("Assets");
        }

        std::string FormatShaderOptionLabel(const std::string& shaderRef) {
            if (shaderRef.empty()) {
                return "basic";
            }

            if (RTBEngine::Core::ResourceManager::IsShaderAssetRef(shaderRef)) {
                return std::filesystem::path(shaderRef).stem().string() + " (Shader Asset)";
            }

            if (shaderRef == "basic") {
                return "Basic (Built-in)";
            }

            return shaderRef;
        }

        std::string CreateShaderAssetInDirectory(const std::filesystem::path& directory) {
            const std::filesystem::path assetRoot = GetProjectAssetRoot();
            std::filesystem::path targetDirectory = directory;
            if (targetDirectory.empty()) {
                targetDirectory = assetRoot / "Shaders";
            }

            std::error_code ec;
            std::filesystem::create_directories(targetDirectory, ec);

            std::filesystem::path assetPath = targetDirectory / "NewShader.shader";
            int suffix = 1;
            while (std::filesystem::exists(assetPath)) {
                assetPath = targetDirectory / ("NewShader" + std::to_string(suffix++) + ".shader");
            }

            if (!RTBEngine::Rendering::ShaderAsset::CreateTemplate(assetPath, assetRoot)) {
                return {};
            }

            std::filesystem::path relativePath =
                std::filesystem::relative(assetPath, assetRoot, ec);
            if (ec) {
                return {};
            }

            const std::string assetRef = MakeAssetReference(relativePath);
            auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
            resources.ScanShaderAssets(assetRoot);
            resources.LoadShaderAsset(assetRef, true);
            return assetRef;
        }

        std::string ToLowerCopy(std::string value) {
            std::transform(value.begin(), value.end(), value.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return value;
        }

        bool MatchesComponentSearch(const std::string& typeName, const char* filter) {
            if (!filter || filter[0] == '\0') {
                return true;
            }

            return ToLowerCopy(typeName).find(ToLowerCopy(filter)) != std::string::npos;
        }

        constexpr float kRotationRad2Deg = 180.0f / 3.14159265f;
        constexpr float kRotationDeg2Rad = 3.14159265f / 180.0f;

        float WrapDegrees(float degrees)
        {
            float wrapped = std::fmod(degrees + 180.0f, 360.0f);
            if (wrapped < 0.0f) {
                wrapped += 360.0f;
            }
            return wrapped - 180.0f;
        }

        RTBEngine::Math::Vector3 RotationDegreesFromQuaternion(const RTBEngine::Math::Quaternion& rotation)
        {
            const RTBEngine::Math::Vector3 radians = rotation.ToEulerAngles();
            return RTBEngine::Math::Vector3(
                WrapDegrees(radians.x * kRotationRad2Deg),
                WrapDegrees(radians.y * kRotationRad2Deg),
                WrapDegrees(radians.z * kRotationRad2Deg));
        }

        bool QuaternionsRepresentSameRotation(
            const RTBEngine::Math::Quaternion& a,
            const RTBEngine::Math::Quaternion& b)
        {
            return std::abs(a.Dot(b)) > 0.9999f;
        }

        void ApplyInspectorRotationDegrees(
            RTBEngine::ECS::Transform& transform,
            const RTBEngine::Math::Vector3& degrees)
        {
            const RTBEngine::Math::Vector3 radians(
                WrapDegrees(degrees.x) * kRotationDeg2Rad,
                WrapDegrees(degrees.y) * kRotationDeg2Rad,
                WrapDegrees(degrees.z) * kRotationDeg2Rad);
            transform.SetRotation(RTBEngine::Math::Quaternion::FromEulerAngles(radians));
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
            return RTBEngine::Animation::Animator::NormalizeClipName(value);
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

            animator->ReloadClipLibrary();
            animator->ReloadKeyClips();
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
            if (assetType == "prefab") {
                return "Drop a .prefab asset here.";
            }
            if (assetType == "rtbasset") {
                return "Drop a .rtbasset data asset here.";
            }
            if (assetType == "texture") {
                return "Drop a texture asset here.";
            }
            if (assetType == "shader") {
                return "Drop a .shader asset here.";
            }
            return "Drop a compatible asset here.";
        }

        AssetType ResolveAssetBrowserType(const std::string& assetType)
        {
            if (assetType == "fbx") {
                return AssetType::Fbx;
            }
            if (assetType == "prefab") {
                return AssetType::Prefab;
            }
            if (assetType == "rtbasset") {
                return AssetType::DataAsset;
            }
            if (assetType == "texture") {
                return AssetType::Texture;
            }
            if (assetType == "shader") {
                return AssetType::Shader;
            }
            if (assetType == "lua") {
                return AssetType::Scene;
            }
            if (assetType == "audio") {
                return AssetType::AudioClip;
            }
            return AssetType::Any;
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

        std::string GetPathDisplayName(const std::string& path, const char* emptyLabel = "[None]")
        {
            if (path.empty()) {
                return emptyLabel;
            }

            const std::filesystem::path filePath(path);
            const std::string fileName = filePath.filename().string();
            return fileName.empty() ? path : fileName;
        }

        ImVec2 InspectorPickerButtonSize(int trailingSmallButtons = 2)
        {
            const ImGuiStyle& style = ImGui::GetStyle();
            const float spacing = style.ItemInnerSpacing.x;
            const float smallButtonWidth = ImGui::CalcTextSize("...").x + style.FramePadding.x * 2.0f;
            const float trailingWidth = trailingSmallButtons > 0
                ? static_cast<float>(trailingSmallButtons) * smallButtonWidth
                    + static_cast<float>(trailingSmallButtons - 1) * spacing
                    + spacing
                : 0.0f;
            const float width = ImGui::GetContentRegionAvail().x - trailingWidth;
            return ImVec2(width > 48.0f ? width : 48.0f, 0.0f);
        }

        constexpr float kInspectorLabelWidthRatio = 0.42f;

        void BeginInspectorRow(const char* label)
        {
            const float contentWidth = ImGui::GetContentRegionAvail().x;
            const float computedWidth = contentWidth * kInspectorLabelWidthRatio;
            const float labelWidth = computedWidth > 120.0f ? computedWidth : 120.0f;

            ImGui::Columns(2, label, false);
            ImGui::SetColumnWidth(0, labelWidth);

            ImGui::AlignTextToFramePadding();
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + labelWidth - 4.0f);
            ImGui::TextUnformatted(label);
            ImGui::PopTextWrapPos();

            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
        }

        void EndInspectorRow()
        {
            ImGui::NextColumn();
            ImGui::Columns(1);
        }

        std::string FormatComponentRefLabel(const RTBEngine::Reflection::PropertyInfo& prop)
        {
            std::string rowLabel = prop.GetInspectorLabel();
            if (!prop.componentTypeName.empty()) {
                rowLabel += " (";
                rowLabel += RTBEngine::Reflection::FormatPropertyName(prop.componentTypeName);
                rowLabel += ")";
            }
            return rowLabel;
        }

        bool BeginInspectorListFoldout(const char* label)
        {
            return ImGui::TreeNodeEx(
                label,
                ImGuiTreeNodeFlags_DefaultOpen |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_OpenOnArrow);
        }

        void EndInspectorListFoldout()
        {
            ImGui::TreePop();
        }
    }

    InspectorPanel::InspectorPanel() {
        assetBrowserModal = std::make_unique<AssetBrowserModal>();
    }

    InspectorPanel::~InspectorPanel() {}

    void InspectorPanel::MarkDirtyFromInspector() {
        if (dirtyContext) {
            MarkEditingDirty(*dirtyContext);
        } else {
            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
        }
    }

    void InspectorPanel::OnUIRender(EditorContext& context) {
        dirtyContext = nullptr;
        ImGui::Begin("Inspector");

        // Validate that the selected GOs still exist in the active scene
        auto* scene = GetEditingScene(context);
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
            dirtyContext = &context;
            auto& name = context.selectedGameObject->GetName();

            bool isActive = context.selectedGameObject->IsActive();
            if (ImGui::Checkbox("##Active", &isActive)) {
                context.selectedGameObject->SetActive(isActive);
                MarkDirtyFromInspector();
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
                MarkDirtyFromInspector();
            }

            {
                RTBEngine::Physics::PhysicsLayerSettings& layerSettings =
                    RTBEngine::Physics::PhysicsLayerSettings::Get();
                const int layerCount = layerSettings.GetLayerCount();
                const int currentLayer = context.selectedGameObject->GetCollisionLayer();
                const char* preview = layerSettings.GetLayerName(currentLayer).c_str();

                BeginInspectorRow("Collision Layer");
                if (ImGui::BeginCombo("##collisionLayer", preview)) {
                    for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex) {
                        const bool selected = layerIndex == currentLayer;
                        const std::string& layerName = layerSettings.GetLayerName(layerIndex);
                        if (ImGui::Selectable(layerName.c_str(), selected)) {
                            context.selectedGameObject->SetCollisionLayer(layerIndex);
                            layerSettings.ApplyToGameObject(context.selectedGameObject);
                            MarkDirtyFromInspector();
                        }

                        if (selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                EndInspectorRow();
            }

            if (!IsPrefabEditMode(context) && context.selectedGameObject->IsPrefabInstance()) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), "Prefab: %s", context.selectedGameObject->GetPrefabName().c_str());
                ImGui::SameLine();
                if (ImGui::Button("Unlink")) {
                    context.selectedGameObject->SetPrefabName("");
                    MarkDirtyFromInspector();
                }
                ImGui::Spacing();
            }

            ImGui::Separator();

            DrawComponents(context.selectedGameObject, context);

            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                ImGui::OpenPopup("AddComponentPopup");
            }

            const bool addComponentPopupOpen = ImGui::IsPopupOpen("AddComponentPopup");
            if (addComponentPopupWasOpen && !addComponentPopupOpen) {
                addComponentSearchBuffer[0] = '\0';
            }
            addComponentPopupWasOpen = addComponentPopupOpen;

            if (ImGui::BeginPopup("AddComponentPopup")) {
                ImGui::SetNextItemWidth(280.0f);
                if (ImGui::IsWindowAppearing()) {
                    ImGui::SetKeyboardFocusHere();
                }

                ImGui::InputTextWithHint(
                    "##ComponentSearch",
                    "Search components...",
                    addComponentSearchBuffer,
                    sizeof(addComponentSearchBuffer));

                auto types = RTBEngine::Reflection::TypeRegistry::GetInstance().GetRegisteredTypes();
                std::vector<std::string> sortedTypes(types.begin(), types.end());
                std::sort(sortedTypes.begin(), sortedTypes.end());

                int visibleCount = 0;
                for (const std::string& type : sortedTypes) {
                    if (MatchesComponentSearch(type, addComponentSearchBuffer)) {
                        ++visibleCount;
                    }
                }

                ImGui::TextDisabled("%d component%s", visibleCount, visibleCount == 1 ? "" : "s");
                ImGui::Separator();

                const float preferredHeight = visibleCount * 22.0f + 8.0f;
                const float listHeight = (std::min)(320.0f, (std::max)(120.0f, preferredHeight));
                if (ImGui::BeginChild("AddComponentList", ImVec2(280.0f, listHeight), true)) {
                    if (visibleCount == 0) {
                        ImGui::TextDisabled("No components match your search.");
                    } else {
                        for (const std::string& type : sortedTypes) {
                            if (!MatchesComponentSearch(type, addComponentSearchBuffer)) {
                                continue;
                            }

                            const RTBEngine::Reflection::TypeInfo* typeInfo =
                                RTBEngine::Reflection::TypeRegistry::GetInstance().GetTypeInfo(type);
                            if (typeInfo && typeInfo->IsDataAsset()) {
                                continue;
                            }

                            if (ImGui::Selectable(type.c_str())) {
                                auto* newComp =
                                    RTBEngine::Reflection::TypeRegistry::GetInstance().CreateComponent(type);
                                if (newComp) {
                                    context.selectedGameObject->AddComponent(newComp);

                                    if (auto* boxCollider =
                                            dynamic_cast<RTBEngine::ECS::BoxColliderComponent*>(newComp)) {
                                        boxCollider->FitToOwnerMesh();
                                    }

                                    MarkDirtyFromInspector();
                                }
                                addComponentSearchBuffer[0] = '\0';
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }
                }
                ImGui::EndChild();

                ImGui::EndPopup();
            }

            // Deferred component removal
            for (auto* comp : componentsToRemove) {
                context.selectedGameObject->RemoveComponent(comp);
                MarkDirtyFromInspector();
            }

            componentsToRemove.clear();
            dirtyContext = nullptr;

        } else if (!context.selectedAssetPath.empty()) {
            std::string ext = context.selectedAssetPath.extension().string();
            for (auto& c : ext) c = std::tolower(c);
            if (ext == ".cubemap") {
                DrawCubemapAssetInspector(context.selectedAssetPath);
            } else if (ext == ".texture") {
                DrawTextureAssetInspector(context.selectedAssetPath);
            } else if (ext == ".shader") {
                DrawShaderAssetInspector(context.selectedAssetPath);
            } else if (ext == ".h" || ext == ".cpp") {
                DrawScriptPreview(context.selectedAssetPath);
            } else if (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
                DrawFbxAssetInspector(context.selectedAssetPath);
            } else if (ext == ".rtbasset") {
                DrawDataAssetInspector(context.selectedAssetPath);
            } else if (ext == ".prefab") {
                DrawPrefabAssetInspector(context);
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

    void InspectorPanel::DrawComponents(RTBEngine::ECS::GameObject* gameObject, EditorContext& context) {
        dirtyContext = &context;
        // Detect if this GameObject has any UIElement — if so show Rect Transform instead of Transform
        RTBEngine::UI::UIElement* uiElement = gameObject->GetComponent<RTBEngine::UI::UIElement>();

        if (uiElement) {
            // Rect Transform (replaces Transform for UI objects)
            if (ImGui::CollapsingHeader("Rect Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                RTBEngine::Math::Vector2 pos = uiElement->GetAnchoredPosition();
                BeginInspectorRow("Position");
                if (ImGui::DragFloat2("##rectPosition", (float*)&pos, 1.0f)) {
                    uiElement->SetAnchoredPosition(pos);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                RTBEngine::Math::Vector2 anchorMin = uiElement->GetAnchorMin();
                BeginInspectorRow("Anchor Min");
                if (ImGui::DragFloat2("##rectAnchorMin", (float*)&anchorMin, 0.01f, 0.0f, 1.0f)) {
                    uiElement->SetAnchorMin(anchorMin);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                RTBEngine::Math::Vector2 anchorMax = uiElement->GetAnchorMax();
                BeginInspectorRow("Anchor Max");
                if (ImGui::DragFloat2("##rectAnchorMax", (float*)&anchorMax, 0.01f, 0.0f, 1.0f)) {
                    uiElement->SetAnchorMax(anchorMax);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                const bool isStretched =
                    anchorMin.x != anchorMax.x || anchorMin.y != anchorMax.y;

                RTBEngine::Math::Vector2 size = uiElement->GetSizeDelta();
                BeginInspectorRow(isStretched ? "Size Delta" : "Size");
                if (ImGui::DragFloat2("##rectSize", (float*)&size, 1.0f)) {
                    uiElement->SetSizeDelta(size);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                if (isStretched) {
                    ImGui::TextDisabled("Stretch: el tamano final viene de anchors + size delta.");
                }

                RTBEngine::Math::Vector2 pivot = uiElement->GetPivot();
                BeginInspectorRow("Pivot");
                if (ImGui::DragFloat2("##rectPivot", (float*)&pivot, 0.01f, 0.0f, 1.0f)) {
                    uiElement->SetPivot(pivot);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                float rot = uiElement->GetRotation();
                BeginInspectorRow("Rotation");
                if (ImGui::DragFloat("##rectRotation", &rot, 0.5f)) {
                    uiElement->SetRotation(rot);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                RTBEngine::Math::Vector2 scl = uiElement->GetScale();
                BeginInspectorRow("Scale");
                if (ImGui::DragFloat2("##rectScale", (float*)&scl, 0.01f)) {
                    uiElement->SetScale(scl);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

            }
        } else {
            // Transform Component (3D — shown for non-UI objects)
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& transform = gameObject->GetTransform();

                // Local position — offset relative to parent
                RTBEngine::Math::Vector3 localPos = transform.GetPosition();
                BeginInspectorRow("Position");
                if (ImGui::DragFloat3("##transformPosition", (float*)&localPos, 0.1f)) {
                    transform.SetPosition(localPos);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                // Local rotation — cached euler degrees; resync when selection or gizmo changes it.
                if (cachedRotationTarget != gameObject) {
                    cachedRotationTarget = gameObject;
                    cachedRotationDeg = RotationDegreesFromQuaternion(transform.GetRotation());
                    cachedRotationSource = transform.GetRotation().Normalized();
                }

                BeginInspectorRow("Rotation");
                if (ImGui::DragFloat3("##transformRotation", (float*)&cachedRotationDeg, 0.5f)) {
                    ApplyInspectorRotationDegrees(transform, cachedRotationDeg);
                    cachedRotationDeg = RotationDegreesFromQuaternion(transform.GetRotation());
                    cachedRotationSource = transform.GetRotation().Normalized();
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();

                const bool rotationFieldActive = ImGui::IsItemActive() || ImGui::IsItemFocused();
                if (!rotationFieldActive &&
                    !QuaternionsRepresentSameRotation(transform.GetRotation(), cachedRotationSource)) {
                    cachedRotationDeg = RotationDegreesFromQuaternion(transform.GetRotation());
                    cachedRotationSource = transform.GetRotation().Normalized();
                }

                // Local scale — real world scale = localScale * parent.worldScale (handled by engine)
                RTBEngine::Math::Vector3 localScale = transform.GetScale();
                BeginInspectorRow("Scale");
                if (ImGui::DragFloat3("##transformScale", (float*)&localScale, 0.01f)) {
                    transform.SetScale(localScale);
                    MarkDirtyFromInspector();
                }
                EndInspectorRow();
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
                } else if (std::string(typeName) == "MeshRenderer") {
                    DrawMeshRendererComponent(static_cast<RTBEngine::ECS::MeshRenderer*>(component.get()));
                } else if (std::string(typeName) == "NavGridComponent") {
                    DrawNavGridComponent(static_cast<RTBEngine::ECS::NavGridComponent*>(component.get()), context);
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

        const std::string label = prop.name == "scenePath"
            ? "Scene"
            : prop.GetInspectorLabel();
        BeginInspectorRow(label.c_str());

        const bool hasScene = !value->empty();
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            hasScene ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

        const std::string buttonLabel = GetPathDisplayName(hasScene ? *value : std::string(), "[None]") + "##SceneDropArea";
        ImGui::Button(buttonLabel.c_str(), InspectorPickerButtonSize(2));

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
                [this, component, value](const std::string& path) {
                    *value = MakeAssetReference(path);
                    component->OnValidate();
                    MarkDirtyFromInspector();
                },
                [this, component, value](const std::string& path) {
                    *value = std::filesystem::path(path).lexically_normal().generic_string();
                    component->OnValidate();
                    MarkDirtyFromInspector();
                });
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("X##ClearScene")) {
            value->clear();
            changed = true;
        }

        EndInspectorRow();

        return changed;
    }

    bool InspectorPanel::DrawAssetRefProperty(RTBEngine::ECS::Component* component,
                                              const RTBEngine::Reflection::PropertyInfo& prop,
                                              std::string* value,
                                              bool& changed,
                                              bool wrapInRow) {
        if (!value) {
            return false;
        }

        const std::string assetType = ToLowerCopy(prop.assetType);
        const bool isFbxAsset = (assetType == "fbx");
        const bool isPrefabAsset = (assetType == "prefab");
        const bool isDataAssetRef = (assetType == "rtbasset");
        const bool isTextureAsset = (assetType == "texture");
        const bool isSceneAsset = (assetType == "lua");
        const bool isAudioAsset = (assetType == "audio");
        if (!isFbxAsset && !isPrefabAsset && !isDataAssetRef && !isTextureAsset && !isSceneAsset && !isAudioAsset) {
            return false;
        }

        if (wrapInRow) {
            BeginInspectorRow(prop.GetInspectorLabel().c_str());
        }

        const bool hasAsset = !value->empty();
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            hasAsset ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

        const std::string dropAreaId = isPrefabAsset ? "##PrefabRefDropArea"
            : isDataAssetRef ? "##DataAssetRefDropArea"
            : "##AssetRefDropArea";
        const std::string buttonLabel = GetPathDisplayName(hasAsset ? *value : std::string(), "[None]") + dropAreaId;
        ImGui::Button(buttonLabel.c_str(), InspectorPickerButtonSize(2));

        ImGui::PopStyleColor(4);

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", hasAsset ? value->c_str() : GetAssetRefTooltip(assetType).c_str());
        }

        if (ImGui::BeginDragDropTarget()) {
            if (isFbxAsset) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_MESH)) {
                    const MeshPayload* payloadData = static_cast<const MeshPayload*>(payload->Data);
                    const std::filesystem::path relativePath(payloadData->path);
                    if (HasFbxExtension(relativePath)) {
                        *value = MakeAssetReference(relativePath);
                        changed = true;
                    }
                }
            } else if (isPrefabAsset) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_PREFAB)) {
                    const PrefabPayload* payloadData = static_cast<const PrefabPayload*>(payload->Data);
                    *value = MakeAssetReference(payloadData->path);
                    changed = true;
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();
        if (ImGui::SmallButton(isPrefabAsset ? "...##SelectPrefabRef"
            : isDataAssetRef ? "...##SelectDataAssetRef"
            : "...##SelectAssetRef")) {
            assetBrowserModal->Open(
                ResolveAssetBrowserType(assetType),
                [this, component, value](const std::string& path) {
                    *value = MakeAssetReference(path);
                    if (component) {
                        component->OnValidate();
                        MarkDirtyFromInspector();
                    }
                });
        }

        ImGui::SameLine();
        if (ImGui::SmallButton(isPrefabAsset ? "X##ClearPrefabRef" : "X##ClearAssetRef")) {
            value->clear();
            changed = true;
        }

        if (wrapInRow) {
            EndInspectorRow();
        }

        return true;
    }

    bool InspectorPanel::DrawListProperty(RTBEngine::ECS::Component* component,
                                          const RTBEngine::Reflection::PropertyInfo& prop) {
        using namespace RTBEngine::Reflection;

        bool changed = false;
        RTBEngine::ECS::Scene* activeScene = dirtyContext ? GetEditingScene(*dirtyContext) : nullptr;

        size_t elementCount = 0;
        switch (prop.listElementType) {
            case ListElementType::String: {
                auto* values = ListPropertyAccess::GetStringVector(component, prop);
                if (!values) {
                    return false;
                }
                elementCount = values->size();

                const std::string header = prop.GetInspectorLabel() + " (" + std::to_string(elementCount) + ")";
                if (!BeginInspectorListFoldout(header.c_str())) {
                    break;
                }

                std::vector<size_t> indicesToRemove;
                for (size_t index = 0; index < values->size(); ++index) {
                    ImGui::PushID(static_cast<int>(index));
                    ImGui::Text("Element %zu", index);
                    ImGui::SameLine();

                    char buffer[1024] = {};
                    strncpy_s(buffer, sizeof(buffer), (*values)[index].c_str(), _TRUNCATE);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
                    if (ImGui::InputText("##ListString", buffer, sizeof(buffer))) {
                        (*values)[index] = buffer;
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (index > 0 && ImGui::SmallButton("^##ListUp")) {
                        std::swap((*values)[index], (*values)[index - 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (index + 1 < values->size() && ImGui::SmallButton("v##ListDown")) {
                        std::swap((*values)[index], (*values)[index + 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::SmallButton("x##ListRemove")) {
                        indicesToRemove.push_back(index);
                        changed = true;
                    }

                    ImGui::PopID();
                }

                for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
                    values->erase(values->begin() + static_cast<std::ptrdiff_t>(*it));
                }

                if (ImGui::Button("+ Add##ListString")) {
                    values->push_back("");
                    changed = true;
                }
                EndInspectorListFoldout();
                break;
            }
            case ListElementType::AssetRef: {
                auto* values = ListPropertyAccess::GetStringVector(component, prop);
                if (!values) {
                    return false;
                }
                elementCount = values->size();

                const std::string header = prop.GetInspectorLabel() + " (" + std::to_string(elementCount) + ")";
                if (!BeginInspectorListFoldout(header.c_str())) {
                    break;
                }

                std::vector<size_t> indicesToRemove;
                for (size_t index = 0; index < values->size(); ++index) {
                    ImGui::PushID(static_cast<int>(index));
                    ImGui::Text("Element %zu", index);
                    ImGui::SameLine();

                    DrawAssetRefProperty(component, prop, &(*values)[index], changed, false);

                    ImGui::SameLine();
                    if (index > 0 && ImGui::SmallButton("^##ListUp")) {
                        std::swap((*values)[index], (*values)[index - 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (index + 1 < values->size() && ImGui::SmallButton("v##ListDown")) {
                        std::swap((*values)[index], (*values)[index + 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::SmallButton("x##ListRemove")) {
                        indicesToRemove.push_back(index);
                        changed = true;
                    }

                    ImGui::PopID();
                }

                for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
                    values->erase(values->begin() + static_cast<std::ptrdiff_t>(*it));
                }

                if (ImGui::Button("+ Add##ListAssetRef")) {
                    values->push_back("");
                    changed = true;
                }
                EndInspectorListFoldout();
                break;
            }
            case ListElementType::GameObjectRef: {
                auto* values = ListPropertyAccess::GetGameObjectVector(component, prop);
                if (!values) {
                    return false;
                }
                elementCount = values->size();

                const std::string header = prop.GetInspectorLabel() + " (" + std::to_string(elementCount) + ")";
                if (!BeginInspectorListFoldout(header.c_str())) {
                    break;
                }

                std::vector<size_t> indicesToRemove;
                for (size_t index = 0; index < values->size(); ++index) {
                    ImGui::PushID(static_cast<int>(index));

                    RTBEngine::ECS::GameObject* gameObjectRef = (*values)[index];
                    const bool hasLiveGameObject = gameObjectRef && IsGameObjectInScene(activeScene, gameObjectRef);

                    ImGui::Text("Element %zu", index);
                    ImGui::SameLine();

                    if (hasLiveGameObject) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
                    } else if (gameObjectRef) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.25f, 1.0f));
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    }

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.5f, 0.8f, 0.5f));

                    if (hasLiveGameObject) {
                        ImGui::Button(gameObjectRef->GetName().c_str(), ImVec2(150, 0));
                    } else if (gameObjectRef) {
                        ImGui::Button("[Missing]##ListGODrop", ImVec2(150, 0));
                    } else {
                        ImGui::Button("[None]##ListGODrop", ImVec2(150, 0));
                    }

                    ImGui::PopStyleColor(4);

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                            const GameObjectPayload* payloadData = static_cast<const GameObjectPayload*>(payload->Data);
                            RTBEngine::ECS::GameObject* draggedGameObject =
                                reinterpret_cast<RTBEngine::ECS::GameObject*>(payloadData->gameObjectId);
                            if (draggedGameObject) {
                                (*values)[index] = draggedGameObject;
                                changed = true;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::SameLine();
                    if (index > 0 && ImGui::SmallButton("^##ListUp")) {
                        std::swap((*values)[index], (*values)[index - 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (index + 1 < values->size() && ImGui::SmallButton("v##ListDown")) {
                        std::swap((*values)[index], (*values)[index + 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::SmallButton("x##ListRemove")) {
                        indicesToRemove.push_back(index);
                        changed = true;
                    }

                    ImGui::PopID();
                }

                for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
                    values->erase(values->begin() + static_cast<std::ptrdiff_t>(*it));
                }

                if (ImGui::Button("+ Add##ListGameObjectRef")) {
                    values->push_back(nullptr);
                    changed = true;
                }
                EndInspectorListFoldout();
                break;
            }
            case ListElementType::ComponentRef: {
                auto* values = ListPropertyAccess::GetComponentVector(component, prop);
                if (!values) {
                    return false;
                }
                elementCount = values->size();

                const std::string header = prop.GetInspectorLabel() + " (" + std::to_string(elementCount) + ")";
                if (!BeginInspectorListFoldout(header.c_str())) {
                    break;
                }

                std::vector<size_t> indicesToRemove;
                for (size_t index = 0; index < values->size(); ++index) {
                    ImGui::PushID(static_cast<int>(index));

                    RTBEngine::ECS::Component* componentRef = (*values)[index];
                    const bool hasLiveComponent = componentRef && IsComponentInScene(activeScene, componentRef);

                    ImGui::Text("Element %zu", index);
                    ImGui::SameLine();

                    if (hasLiveComponent) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 1.0f, 1.0f));
                    } else if (componentRef) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.25f, 1.0f));
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    }

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.8f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.3f, 0.8f, 0.5f));

                    if (hasLiveComponent) {
                        const std::string label = std::string("[") + componentRef->GetTypeName() + "]##ListCompDrop";
                        ImGui::Button(label.c_str(), ImVec2(150, 0));
                    } else if (componentRef) {
                        ImGui::Button("[Missing]##ListCompDrop", ImVec2(150, 0));
                    } else {
                        ImGui::Button("[None]##ListCompDrop", ImVec2(150, 0));
                    }

                    ImGui::PopStyleColor(4);

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                            const GameObjectPayload* payloadData = static_cast<const GameObjectPayload*>(payload->Data);
                            RTBEngine::ECS::GameObject* draggedGameObject =
                                reinterpret_cast<RTBEngine::ECS::GameObject*>(payloadData->gameObjectId);

                            if (draggedGameObject) {
                                bool foundComponent = false;
                                for (const auto& comp : draggedGameObject->GetComponents()) {
                                    if (std::string(comp->GetTypeName()) == prop.componentTypeName) {
                                        (*values)[index] = comp.get();
                                        foundComponent = true;
                                        changed = true;
                                        break;
                                    }
                                }

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
                    if (index > 0 && ImGui::SmallButton("^##ListUp")) {
                        std::swap((*values)[index], (*values)[index - 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (index + 1 < values->size() && ImGui::SmallButton("v##ListDown")) {
                        std::swap((*values)[index], (*values)[index + 1]);
                        changed = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::SmallButton("x##ListRemove")) {
                        indicesToRemove.push_back(index);
                        changed = true;
                    }

                    ImGui::PopID();
                }

                for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
                    values->erase(values->begin() + static_cast<std::ptrdiff_t>(*it));
                }

                if (ImGui::Button("+ Add##ListComponentRef")) {
                    values->push_back(nullptr);
                    changed = true;
                }
                EndInspectorListFoldout();
                break;
            }
            default:
                ImGui::Text("%s: [Unsupported List Element Type]", prop.GetInspectorLabel().c_str());
                break;
        }

        if (prop.tooltip && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", prop.tooltip->c_str());
        }

        return changed;
    }

    void InspectorPanel::DrawProperty(RTBEngine::ECS::Component* component, const RTBEngine::Reflection::PropertyInfo& prop) {
        void* data = prop.GetMutableData(component);
        bool changed = false;
        
        ImGui::PushID(prop.name.c_str());
        
        if (prop.IsReadOnly()) ImGui::BeginDisabled();

        const std::string inspectorLabel = prop.GetInspectorLabel();

        switch (prop.type) {
            case RTBEngine::Reflection::PropertyType::Float: {
                float* val = (float*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                if (prop.range) {
                    changed |= ImGui::SliderFloat("##value", val, prop.range->minValue, prop.range->maxValue);
                } else {
                    changed |= ImGui::DragFloat("##value", val, 0.1f);
                }
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::Int: {
                int* val = (int*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                changed |= ImGui::DragInt("##value", val, 1);
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::Bool: {
                bool* val = (bool*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                changed |= ImGui::Checkbox("##value", val);
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector2: {
                float* val = (float*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                changed |= ImGui::DragFloat2("##value", val, 0.1f);
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector3: {
                float* val = (float*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                changed |= ImGui::DragFloat3("##value", val, 0.1f);
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::Vector4: {
                float* val = (float*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                changed |= ImGui::DragFloat4("##value", val, 0.1f);
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::Color: {
                float* val = (float*)data;
                BeginInspectorRow(inspectorLabel.c_str());
                if (prop.size == sizeof(float) * 3) {
                    changed |= ImGui::ColorEdit3("##value", val);
                } else {
                    changed |= ImGui::ColorEdit4("##value", val);
                }
                EndInspectorRow();
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
                    BeginInspectorRow(inspectorLabel.c_str());
                    if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                        *val = buffer;
                        changed = true;
                    }
                    EndInspectorRow();
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::AssetRef: {
                std::string* val = (std::string*)data;
                if (!DrawAssetRefProperty(component, prop, val, changed)) {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    strncpy_s(buffer, sizeof(buffer), val->c_str(), _TRUNCATE);
                    BeginInspectorRow(inspectorLabel.c_str());
                    if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                        *val = buffer;
                        changed = true;
                    }
                    EndInspectorRow();
                }
                break;
            }
            case RTBEngine::Reflection::PropertyType::Enum: {
                int* val = (int*)data;
                const char* previewValue = (size_t)*val < prop.enumNames.size() ? prop.enumNames[*val].c_str() : "Unknown";
                BeginInspectorRow(inspectorLabel.c_str());
                if (ImGui::BeginCombo("##value", previewValue)) {
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
                EndInspectorRow();
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
                BeginInspectorRow(inspectorLabel.c_str());

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
                    const std::string display = path.empty()
                        ? "[Texture Set]"
                        : GetPathDisplayName(path, "[Texture Set]");
                    std::string label = display + "##TextureDropArea";
                    ImGui::Button(label.c_str(), InspectorPickerButtonSize(2));
                }
                else {
                    ImGui::Button("[None]##TextureDropArea", InspectorPickerButtonSize(2));
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
                        [this, texPtr, loadTextureForInspector](const std::string& path) {
                            std::string fullPath = "Assets/" + path;
                            auto* tex = loadTextureForInspector(fullPath);
                            if (tex) { *texPtr = tex; MarkDirtyFromInspector(); }
                        },
                        [this, texPtr, loadTextureForInspector](const std::string& path) {
                            auto* tex = loadTextureForInspector(path);
                            if (tex) { *texPtr = tex; MarkDirtyFromInspector(); }
                        }
                    );
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearTexture")) {
                    *texPtr = nullptr;
                    changed = true;
                }
                EndInspectorRow();
                break;
            }

            case RTBEngine::Reflection::PropertyType::AudioClipRef: {
                void** clipPtr = (void**)data;
                BeginInspectorRow(inspectorLabel.c_str());

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
                    const std::string display = path.empty()
                        ? "[AudioClip Set]"
                        : GetPathDisplayName(path, "[AudioClip Set]");
                    std::string label = display + "##AudioDropArea";
                    ImGui::Button(label.c_str(), InspectorPickerButtonSize(2));
                }
                else {
                    ImGui::Button("[None]##AudioDropArea", InspectorPickerButtonSize(2));
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
                    assetBrowserModal->Open(AssetType::AudioClip, [this, clipPtr](const std::string& path) {
                        std::string fullPath = std::string("Assets/") + path;
                        auto* audioClip = RTBEngine::Core::ResourceManager::GetInstance().LoadAudioClip(fullPath);
                        if (audioClip) {
                            *clipPtr = audioClip;
                            MarkDirtyFromInspector();
                        }
                    });
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearAudioClip")) {
                    *clipPtr = nullptr;
                    changed = true;
                }
                EndInspectorRow();
                break;
            }

            case RTBEngine::Reflection::PropertyType::MeshRef: {
                void** meshPtr = (void**)data;
                BeginInspectorRow(inspectorLabel.c_str());

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
                    const std::string display = path.empty()
                        ? "[Mesh Set]"
                        : GetPathDisplayName(path, "[Mesh Set]");
                    std::string label = display + "##MeshDropArea";
                    ImGui::Button(label.c_str(), InspectorPickerButtonSize(2));
                }
                else {
                    ImGui::Button("[None]##MeshDropArea", InspectorPickerButtonSize(2));
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
                        [this, meshPtr](const std::string& path) {
                            auto* mesh = RTBEngine::Core::ResourceManager::GetInstance().LoadModel("Assets/" + path);
                            if (mesh) { *meshPtr = mesh; MarkDirtyFromInspector(); }
                        },
                        [this, meshPtr](const std::string& path) {
                            auto* mesh = RTBEngine::Core::ResourceManager::GetInstance().LoadModel(path);
                            if (mesh) { *meshPtr = mesh; MarkDirtyFromInspector(); }
                        }
                    );
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearMesh")) {
                    *meshPtr = nullptr;
                    changed = true;
                }
                EndInspectorRow();
                break;
            }

            case RTBEngine::Reflection::PropertyType::FontRef: {
                void** fontPtr = (void**)data;
                BeginInspectorRow(inspectorLabel.c_str());

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
                    const std::string display = path.empty()
                        ? "[Font Set]"
                        : GetPathDisplayName(path, "[Font Set]");
                    std::string label = display + "##FontDropArea";
                    ImGui::Button(label.c_str(), InspectorPickerButtonSize(2));
                } else {
                    ImGui::Button("[None]##FontDropArea", InspectorPickerButtonSize(2));
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
                        [this, fontPtr](const std::string& path) {
                            float sizes[] = { 12.0f, 16.0f, 18.0f, 24.0f, 32.0f, 48.0f };
                            auto* font = RTBEngine::Core::ResourceManager::GetInstance().LoadFont("Assets/" + path, sizes, 6);
                            if (font) { *fontPtr = font; MarkDirtyFromInspector(); }
                        },
                        [this, fontPtr](const std::string& path) {
                            float sizes[] = { 12.0f, 16.0f, 18.0f, 24.0f, 32.0f, 48.0f };
                            auto* font = RTBEngine::Core::ResourceManager::GetInstance().LoadFont(path, sizes, 6);
                            if (font) { *fontPtr = font; MarkDirtyFromInspector(); }
                        }
                    );
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearFont")) {
                    *fontPtr = nullptr;
                    changed = true;
                }
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::GameObjectRef: {
                RTBEngine::ECS::GameObject** goPtr = (RTBEngine::ECS::GameObject**)data;
                RTBEngine::ECS::Scene* activeScene = dirtyContext ? GetEditingScene(*dirtyContext) : nullptr;
                const bool hasLiveGameObject = *goPtr && IsGameObjectInScene(activeScene, *goPtr);
                BeginInspectorRow(inspectorLabel.c_str());

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
                    const std::string buttonLabel = (*goPtr)->GetName() + "##GODrop_" + prop.name;
                    ImGui::Button(buttonLabel.c_str(), InspectorPickerButtonSize(1));
                } else if (*goPtr) {
                    ImGui::Button("[Missing]##GODropArea", InspectorPickerButtonSize(1));
                } else {
                    ImGui::Button("[None]##GODropArea", InspectorPickerButtonSize(1));
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
                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::ComponentRef: {
                RTBEngine::ECS::Component** compPtr = (RTBEngine::ECS::Component**)data;
                RTBEngine::ECS::Scene* activeScene = dirtyContext ? GetEditingScene(*dirtyContext) : nullptr;
                const bool hasLiveComponent = *compPtr && IsComponentInScene(activeScene, *compPtr);
                const std::string componentRefLabel = FormatComponentRefLabel(prop);
                BeginInspectorRow(componentRefLabel.c_str());

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
                    std::string label = std::string("[") + (*compPtr)->GetTypeName() + "]##CompDrop_" + prop.name;
                    ImGui::Button(label.c_str(), InspectorPickerButtonSize(1));
                } else if (*compPtr) {
                    ImGui::Button("[Missing]##CompDropArea", InspectorPickerButtonSize(1));
                } else {
                    ImGui::Button("[None]##CompDropArea", InspectorPickerButtonSize(1));
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

                EndInspectorRow();
                break;
            }
            case RTBEngine::Reflection::PropertyType::List: {
                changed |= DrawListProperty(component, prop);
                break;
            }
            default:
                BeginInspectorRow(inspectorLabel.c_str());
                ImGui::TextUnformatted("[Unsupported Type]");
                EndInspectorRow();
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
            MarkDirtyFromInspector();
        }

        ImGui::PopID();
    }

    void InspectorPanel::DrawMeshShaderProperties(RTBEngine::ECS::MeshRenderer* meshRenderer, bool& changed) {
        if (!meshRenderer) {
            return;
        }

        const std::string shaderRef = meshRenderer->shaderRef.empty() ? "basic" : meshRenderer->shaderRef;
        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        if (RTBEngine::Core::ResourceManager::IsShaderAssetRef(shaderRef)) {
            resources.LoadShaderAsset(shaderRef);
        }

        const std::vector<RTBEngine::Rendering::ShaderPropertyDefinition> properties =
            RTBEngine::Rendering::ShaderProperties::GetDefinitions(shaderRef);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextUnformatted("Shader Properties");
        ImGui::Spacing();

        if (properties.empty()) {
            ImGui::TextWrapped("This shader does not declare any editable parameters.");
            return;
        }

        for (const RTBEngine::Rendering::ShaderPropertyDefinition& property : properties) {
            ImGui::PushID(property.uniformName.c_str());
            const char* label = property.displayName.empty()
                ? property.uniformName.c_str()
                : property.displayName.c_str();

            switch (property.type) {
            case RTBEngine::Rendering::ShaderPropertyType::Color: {
                RTBEngine::Math::Vector4 colorValue =
                    RTBEngine::Rendering::ShaderProperties::ResolveColorValue(
                        property,
                        meshRenderer->colorRef,
                        meshRenderer->shaderPropertyOverrides);

                if (property.uniformName == "uColor") {
                    BeginInspectorRow(label);
                    float colorComponents[4] = { colorValue.x, colorValue.y, colorValue.z, colorValue.w };
                    if (ImGui::ColorEdit4("##shaderColor", colorComponents)) {
                        meshRenderer->colorRef = RTBEngine::Math::Vector4(
                            colorComponents[0],
                            colorComponents[1],
                            colorComponents[2],
                            colorComponents[3]);
                        changed = true;
                    }
                    EndInspectorRow();
                } else {
                    BeginInspectorRow(label);
                    float colorComponents[4] = { colorValue.x, colorValue.y, colorValue.z, colorValue.w };
                    if (ImGui::ColorEdit4("##shaderColorOverride", colorComponents)) {
                        RTBEngine::Rendering::ShaderProperties::SetColorOverride(
                            meshRenderer->shaderPropertyOverrides,
                            property.uniformName,
                            RTBEngine::Math::Vector4(
                                colorComponents[0],
                                colorComponents[1],
                                colorComponents[2],
                                colorComponents[3]));
                        changed = true;
                    }
                    EndInspectorRow();
                }
                break;
            }
            case RTBEngine::Rendering::ShaderPropertyType::Texture: {
                if (property.uniformName == "uTexture") {
                    RTBEngine::Reflection::PropertyInfo textureProp;
                    textureProp.name = "textureRef";
                    textureProp.displayName = label;
                    textureProp.type = RTBEngine::Reflection::PropertyType::TextureRef;
                    DrawProperty(meshRenderer, textureProp);
                } else {
                    BeginInspectorRow(label);
                    ImGui::TextDisabled("Custom texture uniforms are not supported yet.");
                    EndInspectorRow();
                }
                break;
            }
            case RTBEngine::Rendering::ShaderPropertyType::Float: {
                float floatValue = RTBEngine::Rendering::ShaderProperties::ResolveFloatValue(
                    property,
                    meshRenderer->shaderPropertyOverrides);
                BeginInspectorRow(label);
                if (property.hasRange) {
                    if (ImGui::SliderFloat(
                            "##shaderFloat",
                            &floatValue,
                            property.minValue,
                            property.maxValue)) {
                        RTBEngine::Rendering::ShaderProperties::SetFloatOverride(
                            meshRenderer->shaderPropertyOverrides,
                            property.uniformName,
                            floatValue);
                        changed = true;
                    }
                } else if (ImGui::DragFloat("##shaderFloat", &floatValue, 0.01f)) {
                    RTBEngine::Rendering::ShaderProperties::SetFloatOverride(
                        meshRenderer->shaderPropertyOverrides,
                        property.uniformName,
                        floatValue);
                    changed = true;
                }
                EndInspectorRow();
                break;
            }
            case RTBEngine::Rendering::ShaderPropertyType::Vector2: {
                RTBEngine::Math::Vector4 vectorValue =
                    RTBEngine::Rendering::ShaderProperties::ResolveVectorValue(
                        property,
                        meshRenderer->shaderPropertyOverrides);
                float components[2] = { vectorValue.x, vectorValue.y };
                BeginInspectorRow(label);
                if (ImGui::DragFloat2("##shaderVec2", components, 0.01f)) {
                    RTBEngine::Rendering::ShaderProperties::SetVectorOverride(
                        meshRenderer->shaderPropertyOverrides,
                        property.uniformName,
                        RTBEngine::Math::Vector4(components[0], components[1], 0.0f, 0.0f),
                        2);
                    changed = true;
                }
                EndInspectorRow();
                break;
            }
            case RTBEngine::Rendering::ShaderPropertyType::Vector3: {
                RTBEngine::Math::Vector4 vectorValue =
                    RTBEngine::Rendering::ShaderProperties::ResolveVectorValue(
                        property,
                        meshRenderer->shaderPropertyOverrides);
                float components[3] = { vectorValue.x, vectorValue.y, vectorValue.z };
                BeginInspectorRow(label);
                if (ImGui::DragFloat3("##shaderVec3", components, 0.01f)) {
                    RTBEngine::Rendering::ShaderProperties::SetVectorOverride(
                        meshRenderer->shaderPropertyOverrides,
                        property.uniformName,
                        RTBEngine::Math::Vector4(components[0], components[1], components[2], 0.0f),
                        3);
                    changed = true;
                }
                EndInspectorRow();
                break;
            }
            case RTBEngine::Rendering::ShaderPropertyType::Vector4: {
                RTBEngine::Math::Vector4 vectorValue =
                    RTBEngine::Rendering::ShaderProperties::ResolveVectorValue(
                        property,
                        meshRenderer->shaderPropertyOverrides);
                float components[4] = { vectorValue.x, vectorValue.y, vectorValue.z, vectorValue.w };
                BeginInspectorRow(label);
                if (ImGui::DragFloat4("##shaderVec4", components, 0.01f)) {
                    RTBEngine::Rendering::ShaderProperties::SetVectorOverride(
                        meshRenderer->shaderPropertyOverrides,
                        property.uniformName,
                        RTBEngine::Math::Vector4(
                            components[0],
                            components[1],
                            components[2],
                            components[3]),
                        4);
                    changed = true;
                }
                EndInspectorRow();
                break;
            }
            }

            ImGui::PopID();
        }
    }

    void InspectorPanel::DrawMeshRendererComponent(RTBEngine::ECS::MeshRenderer* meshRenderer) {
        if (!meshRenderer) {
            return;
        }

        bool changed = false;
        const RTBEngine::Reflection::TypeInfo* typeInfo = meshRenderer->GetTypeInfo();
        if (typeInfo) {
            for (const RTBEngine::Reflection::PropertyInfo* prop : typeInfo->GetInspectorProperties()) {
                if (!prop
                    || prop->name == "shaderRef"
                    || prop->name == "colorRef"
                    || prop->name == "textureRef"
                    || prop->name == "shaderPropertyOverrides") {
                    continue;
                }
                DrawProperty(meshRenderer, *prop);
            }
        }

        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        resources.ScanShaderAssets(GetProjectAssetRoot());

        std::vector<std::string> shaderOptions = resources.GetMeshShaderOptions();
        if (meshRenderer->shaderRef.empty()) {
            meshRenderer->shaderRef = "basic";
        }

        if (std::find(shaderOptions.begin(), shaderOptions.end(), meshRenderer->shaderRef) ==
            shaderOptions.end()) {
            shaderOptions.push_back(meshRenderer->shaderRef);
        }

        const std::string previewLabel = FormatShaderOptionLabel(meshRenderer->shaderRef);
        BeginInspectorRow("Shader");
        if (ImGui::BeginCombo("##meshRendererShader", previewLabel.c_str())) {
            for (const std::string& option : shaderOptions) {
                const bool isSelected = option == meshRenderer->shaderRef;
                if (ImGui::Selectable(FormatShaderOptionLabel(option).c_str(), isSelected)) {
                    meshRenderer->shaderRef = option;
                    meshRenderer->OnValidate();
                    RTBEngine::Core::ResourceManager::GetInstance().ResolveShader(option);
                    changed = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        EndInspectorRow();

        if (ImGui::Button("Create Shader...##MeshRendererCreateShader")) {
            const std::string createdShaderRef =
                CreateShaderAssetInDirectory(GetProjectAssetRoot() / "Shaders");
            if (!createdShaderRef.empty()) {
                meshRenderer->shaderRef = createdShaderRef;
                changed = true;
            }
        }

        if (RTBEngine::Core::ResourceManager::IsShaderAssetRef(meshRenderer->shaderRef)) {
            if (ImGui::Button("Reload Shader##MeshRendererReloadShader")) {
                resources.LoadShaderAsset(meshRenderer->shaderRef, true);
                meshRenderer->OnValidate();
                changed = true;
            }
        }

        DrawMeshShaderProperties(meshRenderer, changed);

        if (changed) {
            meshRenderer->OnValidate();
            MarkDirtyFromInspector();
        }
    }

    void InspectorPanel::DrawAnimatorComponent(RTBEngine::Animation::Animator* animator) {
        bool changed = false;

        if (animatorScanTarget != animator) {
            animatorScanTarget = animator;
            animatorScanStatus.clear();
        }

        //Model path
        {
            const std::string previousModel = animator->modelRef;
            RTBEngine::Reflection::PropertyInfo modelProp;
            modelProp.name = "modelRef";
            modelProp.displayName = "Model";
            modelProp.assetType = "fbx";
            DrawAssetRefProperty(animator, modelProp, &animator->modelRef, changed);
            if (animator->modelRef != previousModel) {
                animatorScanStatus.clear();
            }
        }

        //Additional models are drawn via generic list property reflection.

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

        DrawAnimatorKeyClips(animator, changed);

        //Default clip combo
        {
            std::vector<std::string> clipNames = animator->GetClipNames();
            std::string currentDefault = animator->defaultClip;
            const char* previewValue = currentDefault.empty() ? "(none)" : currentDefault.c_str();
            BeginInspectorRow("Default Clip");
            if (ImGui::BeginCombo("##animDefaultClip", previewValue)) {
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
            EndInspectorRow();
        }

        //Speed slider
        BeginInspectorRow("Speed");
        if (ImGui::SliderFloat("##animSpeed", &animator->speed, 0.0f, 3.0f)) {
            animator->SetSpeed(animator->speed);
            changed = true;
        }
        EndInspectorRow();

        //Playing and looping checkboxes
        {
            const bool wasPlaying = animator->playing;
            BeginInspectorRow("Playing");
            if (ImGui::Checkbox("##animPlaying", &animator->playing)) {
                if (wasPlaying && !animator->playing) {
                    animator->Stop();
                }
                changed = true;
            }
            EndInspectorRow();
        }
        BeginInspectorRow("Looping");
        if (ImGui::Checkbox("##animLooping", &animator->looping)) {
            changed = true;
        }
        EndInspectorRow();

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

        if (const RTBEngine::Reflection::TypeInfo* typeInfo = animator->GetTypeInfo()) {
            for (const RTBEngine::Reflection::PropertyInfo* prop : typeInfo->GetInspectorProperties()) {
                if (prop->type == RTBEngine::Reflection::PropertyType::List) {
                    if (DrawListProperty(animator, *prop)) {
                        animatorScanStatus.clear();
                        changed = true;
                    }
                }
            }
        }

        if (changed) {
            animator->OnValidate();
            MarkDirtyFromInspector();
        }
    }

    void InspectorPanel::DrawAnimatorKeyClips(RTBEngine::Animation::Animator* animator, bool& changed)
    {
        if (!animator) {
            return;
        }

        ImGui::Spacing();
        const size_t entryCount = animator->keyClips.size();
        const std::string header = "Animation Keys (" + std::to_string(entryCount) + ")";
        ImGui::Separator();
        ImGui::TextDisabled("%s", header.c_str());

        std::vector<size_t> indicesToRemove;
        for (size_t index = 0; index < animator->keyClips.size(); ++index) {
            RTBEngine::Animation::AnimationKeyClip& entry = animator->keyClips[index];
            ImGui::PushID(static_cast<int>(index));

            ImGui::Separator();
            ImGui::Text("Entry %zu", index + 1);

            char keyBuffer[128] = {};
            strncpy_s(keyBuffer, sizeof(keyBuffer), entry.key.c_str(), _TRUNCATE);
            if (ImGui::InputText("Key", keyBuffer, sizeof(keyBuffer))) {
                entry.key = keyBuffer;
                changed = true;
            }

            RTBEngine::Reflection::PropertyInfo clipRefProp;
            clipRefProp.name = "clipFbxRef";
            clipRefProp.displayName = "Clip FBX";
            clipRefProp.assetType = "fbx";
            if (DrawAssetRefProperty(animator, clipRefProp, &entry.clipFbxRef, changed, false)) {
                changed = true;
            }

            if (ImGui::Checkbox("Loop", &entry.loop)) {
                changed = true;
            }

            if (ImGui::Button("Remove")) {
                indicesToRemove.push_back(index);
                changed = true;
            }

            ImGui::PopID();
        }

        for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
            if (*it < animator->keyClips.size()) {
                animator->keyClips.erase(animator->keyClips.begin() + static_cast<ptrdiff_t>(*it));
            }
        }

        if (ImGui::Button("Add Animation Key")) {
            animator->keyClips.push_back({});
            changed = true;
        }
    }

    void InspectorPanel::DrawParticleSystemComponent(RTBEngine::ECS::ParticleSystem* particleSystem) {
        if (!particleSystem) {
            return;
        }

        bool changed = false;

        if (ImGui::Button("Play")) {
            particleSystem->Play();
            if (particleSystem->emissionRate <= 0.0f) {
                particleSystem->Emit(particleSystem->burstCount);
                particleSystem->Tick(0.001f);
            }
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
            particleSystem->Tick(0.001f);
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
            MarkDirtyFromInspector();
        }
    }

    void InspectorPanel::DrawNavGridComponent(RTBEngine::ECS::NavGridComponent* navGridComponent,
                                              EditorContext& context)
    {
        if (!navGridComponent) {
            return;
        }

        bool changed = false;

        if (navGridComponent->IsBaked()) {
            ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.4f, 1.0f), "Status: Baked");
            ImGui::Text("Walkable cells: %d", navGridComponent->GetWalkableCellCount());
            ImGui::Text("Resolution: %dx%d",
                navGridComponent->GetGrid().GetWidth(),
                navGridComponent->GetGrid().GetHeight());
        } else {
            ImGui::TextColored(ImVec4(0.95f, 0.55f, 0.2f, 1.0f), "Status: Not baked");
            ImGui::TextDisabled("Bake is stored in a .navmesh file next to the scene.");
        }

        const RTBEngine::Math::Vector3 worldOrigin = navGridComponent->GetWorldOrigin();
        ImGui::Text("World origin: (%.1f, %.1f, %.1f)", worldOrigin.x, worldOrigin.y, worldOrigin.z);

        ImGui::Spacing();
        if (ImGui::Button("Rebake Grid", ImVec2(120.0f, 0.0f))) {
            if (context.ensureScenePhysicsReady) {
                context.ensureScenePhysicsReady();
            }
            if (navGridComponent->BakeGrid()) {
                changed = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Bake", ImVec2(120.0f, 0.0f))) {
            navGridComponent->ClearBakedGrid();
            changed = true;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const RTBEngine::Reflection::TypeInfo* typeInfo = navGridComponent->GetTypeInfo();
        if (typeInfo) {
            auto properties = typeInfo->GetInspectorProperties();
            for (const auto* prop : properties) {
                DrawProperty(navGridComponent, *prop);
            }
        }

        if (changed) {
            navGridComponent->OnValidate();
            MarkDirtyFromInspector();
        }
    }

    std::string InspectorPanel::FormatTypeName(const char* typeName) {
        return RTBEngine::Reflection::FormatPropertyName(typeName);
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
            ImGui::PushID(i);
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
            if (ImGui::SmallButton("...##BrowseFace")) {
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
            if (ImGui::SmallButton("X##ClearFace")) {
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

    void InspectorPanel::DrawShaderAssetInspector(const std::filesystem::path& shaderPath) {
        if (shaderEditorPath != shaderPath) {
            shaderEditorPath = shaderPath;
            shaderAssetVertex.clear();
            shaderAssetFragment.clear();
            shaderAssetProperties.clear();

            Project* project = Project::GetActiveProject();
            const std::string assetRef = project
                ? project->GetAssetReferencePath(shaderPath)
                : shaderPath.generic_string();

            RTBEngine::Rendering::ShaderAssetData assetData;
            if (RTBEngine::Rendering::ShaderAsset::ParseFile(assetRef, assetData)) {
                shaderAssetVertex = assetData.vertexPath;
                shaderAssetFragment = assetData.fragmentPath;
                shaderAssetProperties = assetData.properties;
            }
        }

        ImGui::Text("Shader Asset");
        ImGui::Text("%s", shaderPath.filename().string().c_str());
        ImGui::Separator();
        ImGui::Spacing();

        bool changed = false;

        auto drawShaderSourceField = [&](const char* label,
                                         const char* browseId,
                                         const char* clearId,
                                         std::string& targetPath) {
            ImGui::Text("%s", label);
            ImGui::SameLine();

            const bool hasPath = !targetPath.empty();
            ImGui::PushStyleColor(
                ImGuiCol_Text,
                hasPath ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.8f, 0.5f));

            const std::string buttonLabel = (hasPath
                ? std::filesystem::path(targetPath).filename().string()
                : std::string("[None]")) + browseId;
            ImGui::Button(buttonLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 52.0f, 0));
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            if (ImGui::SmallButton(browseId)) {
                assetBrowserModal->Open(
                    AssetType::Any,
                    [this, &targetPath, &changed](const std::string& path) {
                        std::string extension = std::filesystem::path(path).extension().string();
                        for (char& character : extension) {
                            character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
                        }
                        if (extension == ".vert" || extension == ".frag" || extension == ".glsl") {
                            targetPath = MakeAssetReference(path);
                            changed = true;
                        }
                    });
            }

            ImGui::SameLine();
            if (ImGui::SmallButton(clearId)) {
                targetPath.clear();
                changed = true;
            }
        };

        drawShaderSourceField(
            "Vertex:",
            "...##BrowseShaderVertex",
            "X##ClearShaderVertex",
            shaderAssetVertex);
        ImGui::Spacing();
        drawShaderSourceField(
            "Fragment:",
            "...##BrowseShaderFragment",
            "X##ClearShaderFragment",
            shaderAssetFragment);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Properties");
        ImGui::Spacing();

        static const char* propertyTypeLabels[] = {
            "Color", "Float", "Vector2", "Vector3", "Vector4", "Texture"
        };

        std::vector<size_t> propertyIndicesToRemove;
        for (size_t propertyIndex = 0; propertyIndex < shaderAssetProperties.size(); ++propertyIndex) {
            RTBEngine::Rendering::ShaderPropertyDefinition& property = shaderAssetProperties[propertyIndex];
            ImGui::PushID(static_cast<int>(propertyIndex));

            char uniformBuffer[128] = {};
            strncpy_s(uniformBuffer, sizeof(uniformBuffer), property.uniformName.c_str(), _TRUNCATE);
            if (ImGui::InputText("Uniform", uniformBuffer, sizeof(uniformBuffer))) {
                property.uniformName = uniformBuffer;
                changed = true;
            }

            int typeIndex = static_cast<int>(property.type);
            if (ImGui::Combo("Type", &typeIndex, propertyTypeLabels, IM_ARRAYSIZE(propertyTypeLabels))) {
                property.type = static_cast<RTBEngine::Rendering::ShaderPropertyType>(typeIndex);
                changed = true;
            }

            float defaultComponents[4] = {
                property.defaultValue.x,
                property.defaultValue.y,
                property.defaultValue.z,
                property.defaultValue.w
            };

            switch (property.type) {
            case RTBEngine::Rendering::ShaderPropertyType::Color:
            case RTBEngine::Rendering::ShaderPropertyType::Vector4:
                if (ImGui::DragFloat4("Default", defaultComponents, 0.01f)) {
                    property.defaultValue = RTBEngine::Math::Vector4(
                        defaultComponents[0],
                        defaultComponents[1],
                        defaultComponents[2],
                        defaultComponents[3]);
                    changed = true;
                }
                break;
            case RTBEngine::Rendering::ShaderPropertyType::Float:
                if (ImGui::DragFloat("Default", &defaultComponents[0], 0.01f)) {
                    property.defaultValue.x = defaultComponents[0];
                    changed = true;
                }
                if (ImGui::Checkbox("Range", &property.hasRange)) {
                    changed = true;
                }
                if (property.hasRange) {
                    if (ImGui::DragFloat("Min", &property.minValue, 0.01f)) {
                        changed = true;
                    }
                    if (ImGui::DragFloat("Max", &property.maxValue, 0.01f)) {
                        changed = true;
                    }
                }
                break;
            case RTBEngine::Rendering::ShaderPropertyType::Vector2:
                if (ImGui::DragFloat2("Default", defaultComponents, 0.01f)) {
                    property.defaultValue.x = defaultComponents[0];
                    property.defaultValue.y = defaultComponents[1];
                    changed = true;
                }
                break;
            case RTBEngine::Rendering::ShaderPropertyType::Vector3:
                if (ImGui::DragFloat3("Default", defaultComponents, 0.01f)) {
                    property.defaultValue.x = defaultComponents[0];
                    property.defaultValue.y = defaultComponents[1];
                    property.defaultValue.z = defaultComponents[2];
                    changed = true;
                }
                break;
            case RTBEngine::Rendering::ShaderPropertyType::Texture:
                ImGui::TextDisabled("No default value");
                break;
            }

            if (ImGui::Button("Remove##ShaderProperty")) {
                propertyIndicesToRemove.push_back(propertyIndex);
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        for (auto it = propertyIndicesToRemove.rbegin(); it != propertyIndicesToRemove.rend(); ++it) {
            shaderAssetProperties.erase(shaderAssetProperties.begin() + static_cast<ptrdiff_t>(*it));
            changed = true;
        }

        if (ImGui::Button("Add Property##ShaderAsset")) {
            RTBEngine::Rendering::ShaderPropertyDefinition newProperty;
            newProperty.uniformName = "uParam";
            newProperty.displayName = "Param";
            newProperty.type = RTBEngine::Rendering::ShaderPropertyType::Float;
            shaderAssetProperties.push_back(newProperty);
            changed = true;
        }

        ImGui::Spacing();
        if (ImGui::Button("Reload Shader", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            SaveShaderAssetInspector();
        }

        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0)) || changed) {
            SaveShaderAssetInspector();
        }
    }

    void InspectorPanel::SaveShaderAssetInspector() {
        if (shaderEditorPath.empty()) {
            return;
        }

        Project* project = Project::GetActiveProject();
        const std::string assetRef = project
            ? project->GetAssetReferencePath(shaderEditorPath)
            : shaderEditorPath.generic_string();

        RTBEngine::Rendering::ShaderAssetData assetData;
        assetData.vertexPath = shaderAssetVertex;
        assetData.fragmentPath = shaderAssetFragment;
        assetData.properties = shaderAssetProperties;
        if (!RTBEngine::Rendering::ShaderAsset::SaveFile(assetRef, assetData)) {
            return;
        }

        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        resources.ScanShaderAssets(GetProjectAssetRoot());
        resources.LoadShaderAsset(assetRef, true);
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

    void InspectorPanel::DrawDataAssetProperty(
        RTBEngine::Data::DataAsset* asset,
        const RTBEngine::Reflection::PropertyInfo& prop,
        bool& changed)
    {
        if (!asset) {
            return;
        }

        void* data = prop.GetMutableData(asset->GetActualObject());
        if (!data) {
            return;
        }

        ImGui::PushID(prop.name.c_str());
        const std::string inspectorLabel = prop.GetInspectorLabel();

        switch (prop.type) {
        case RTBEngine::Reflection::PropertyType::Float: {
            float* val = static_cast<float*>(data);
            BeginInspectorRow(inspectorLabel.c_str());
            if (prop.range) {
                changed |= ImGui::SliderFloat("##value", val, prop.range->minValue, prop.range->maxValue);
            }
            else {
                changed |= ImGui::DragFloat("##value", val, 0.1f);
            }
            EndInspectorRow();
            break;
        }
        case RTBEngine::Reflection::PropertyType::Int: {
            int* val = static_cast<int*>(data);
            BeginInspectorRow(inspectorLabel.c_str());
            changed |= ImGui::DragInt("##value", val, 1);
            EndInspectorRow();
            break;
        }
        case RTBEngine::Reflection::PropertyType::Bool: {
            bool* val = static_cast<bool*>(data);
            BeginInspectorRow(inspectorLabel.c_str());
            changed |= ImGui::Checkbox("##value", val);
            EndInspectorRow();
            break;
        }
        case RTBEngine::Reflection::PropertyType::String: {
            std::string* val = static_cast<std::string*>(data);
            char buffer[1024] = {};
            strncpy_s(buffer, sizeof(buffer), val->c_str(), _TRUNCATE);
            BeginInspectorRow(inspectorLabel.c_str());
            if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                *val = buffer;
                changed = true;
            }
            EndInspectorRow();
            break;
        }
        case RTBEngine::Reflection::PropertyType::AssetRef: {
            std::string* val = static_cast<std::string*>(data);
            if (!DrawAssetRefProperty(nullptr, prop, val, changed)) {
                char buffer[1024] = {};
                strncpy_s(buffer, sizeof(buffer), val->c_str(), _TRUNCATE);
                BeginInspectorRow(inspectorLabel.c_str());
                if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                    *val = buffer;
                    changed = true;
                }
                EndInspectorRow();
            }
            break;
        }
        default:
            BeginInspectorRow(inspectorLabel.c_str());
            ImGui::TextDisabled("(unsupported in data asset inspector)");
            EndInspectorRow();
            break;
        }

        ImGui::PopID();
    }

    void InspectorPanel::DrawPrefabAssetInspector(EditorContext& context)
    {
        const std::filesystem::path& prefabPath = context.selectedAssetPath;
        ImGui::Text("Prefab Asset");
        ImGui::Text("%s", prefabPath.filename().string().c_str());
        ImGui::Separator();

        RTBEngine::ECS::Prefab* prefab =
            RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(prefabPath.string());
        if (!prefab) {
            ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f),
                "Prefab is not loaded in the registry.");
            return;
        }

        ImGui::Text("Root Name: %s", prefab->GetName().c_str());
        ImGui::Spacing();
        ImGui::TextWrapped("Double-click the asset in the Content Browser to open Prefab Mode.");
        ImGui::Spacing();

        if (ImGui::Button("Open Prefab", ImVec2(140.0f, 0.0f))) {
            context.pendingPrefabOpen = prefabPath;
        }
    }

    void InspectorPanel::DrawDataAssetInspector(const std::filesystem::path& dataAssetPath)
    {
        if (dataAssetEditorPath != dataAssetPath) {
            dataAssetEditorPath = dataAssetPath;
            dataAssetEditorDirty = false;

            Project* project = Project::GetActiveProject();
            const std::string assetRef = project
                ? project->GetAssetReferencePath(dataAssetPath)
                : dataAssetPath.generic_string();

            dataAssetEditorInstance =
                RTBEngine::Core::ResourceManager::GetInstance().LoadDataAsset(assetRef);
        }

        ImGui::Text("Data Asset");
        ImGui::Text("%s", dataAssetPath.filename().string().c_str());
        ImGui::Separator();

        if (!dataAssetEditorInstance) {
            ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f),
                "Failed to load data asset. Rebuild GameScripts if the type is new.");
            return;
        }

        ImGui::Text("Type: %s", dataAssetEditorInstance->GetTypeName());
        if (dataAssetEditorDirty) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.82f, 0.35f, 1.0f), "*");
        }

        ImGui::Spacing();

        const RTBEngine::Reflection::TypeInfo* typeInfo = dataAssetEditorInstance->GetTypeInfo();
        if (!typeInfo) {
            ImGui::TextDisabled("No reflection metadata available.");
            return;
        }

        bool changed = false;
        for (const RTBEngine::Reflection::PropertyInfo* prop : typeInfo->GetInspectorProperties()) {
            if (!prop || prop->name == "type") {
                continue;
            }
            DrawDataAssetProperty(dataAssetEditorInstance, *prop, changed);
        }

        if (changed) {
            dataAssetEditorDirty = true;
        }

        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("Save Asset", ImVec2(140.0f, 0.0f))) {
            SaveDataAssetInspector();
        }
    }

    void InspectorPanel::SaveDataAssetInspector()
    {
        if (!dataAssetEditorInstance || dataAssetEditorPath.empty()) {
            return;
        }

        Project* project = Project::GetActiveProject();
        const std::string assetRef = project
            ? project->GetAssetReferencePath(dataAssetEditorPath)
            : dataAssetEditorPath.generic_string();

        if (!RTBEngine::Scripting::DataAssetSaver::Save(assetRef, *dataAssetEditorInstance)) {
            return;
        }

        RTBEngine::Core::ResourceManager::GetInstance().EvictDataAsset(assetRef);
        dataAssetEditorInstance =
            RTBEngine::Core::ResourceManager::GetInstance().LoadDataAsset(assetRef);
        dataAssetEditorDirty = false;
    }
}
