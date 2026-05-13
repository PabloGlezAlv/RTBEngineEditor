#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/ECS/PrefabRegistry.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/Cubemap.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include <RTBEngine/Rendering/FbxBinding.h>
#include <RTBEngine/UI/Canvas.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIContainer.h>
#include <RTBEngine/UI/Elements/UIInputField.h>
#include <RTBEngine/UI/Elements/UIPanel.h>
#include <RTBEngine/UI/Elements/UISlider.h>
#include <RTBEngine/UI/Elements/UIText.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include "../DragDropPayloads.h"
#include "../../Project/Project.h"
#include <filesystem>
#include <vector>

namespace RTBEditor {

    namespace {
        std::filesystem::path GetAssetRootPath() {
            Project* project = Project::GetActiveProject();
            return project ? project->GetAssetRootPath() : std::filesystem::path("Assets");
        }

        std::string MakeAssetReference(const std::filesystem::path& relativePath) {
            Project* project = Project::GetActiveProject();
            if (project) {
                return project->GetAssetReferencePath(relativePath);
            }
            return (std::filesystem::path("Assets") / relativePath).lexically_normal().generic_string();
        }
    }

    SceneHierarchyPanel::SceneHierarchyPanel() {}
    SceneHierarchyPanel::~SceneHierarchyPanel() {}

    void SceneHierarchyPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Hierarchy");

        auto& sceneManager = RTBEngine::ECS::SceneManager::GetInstance();
        auto activeScene = sceneManager.GetActiveScene();
        if (activeScene) {
            // Scene header
            std::string sceneName = activeScene->GetName();
            if (sceneManager.IsSceneDirty()) {
                sceneName += " *";
            }
            
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
            ImGui::Text("Scene: %s", sceneName.c_str());
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::Spacing();

            //Scene Settings
            if (ImGui::CollapsingHeader("Scene Settings")) {
                ImGui::Spacing();

                // Skybox enabled toggle
                bool skyboxEnabled = activeScene->IsSkyboxEnabled();
                if (ImGui::Checkbox("Skybox Enabled", &skyboxEnabled)) {
                    activeScene->SetSkyboxEnabled(skyboxEnabled);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                ImGui::Spacing();

                // Skybox cubemap field
                RTBEngine::Rendering::Cubemap* currentCubemap = activeScene->GetSkyboxCubemap();
                std::string cubemapLabel = "None";
                if (currentCubemap) {
                    std::string path = RTBEngine::Core::ResourceManager::GetInstance().GetCubemapPath(currentCubemap);
                    if (!path.empty()) {
                        cubemapLabel = std::filesystem::path(path).filename().string();
                    } else {
                        cubemapLabel = "(unknown)";
                    }
                }

                ImGui::Text("Skybox");
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                ImGui::Button(cubemapLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 26.0f, 0));
                ImGui::PopStyleColor();

                // Fill remaining window space to accept drops anywhere
                ImVec2 available = ImGui::GetContentRegionAvail();
                if (available.y > 0)
                    ImGui::Dummy(available);

                // Drop target for cubemap folder
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_CUBEMAP)) {
                        const CubemapPayload* data = static_cast<const CubemapPayload*>(payload->Data);
                        std::string assetPath = MakeAssetReference(data->path);

                        RTBEngine::Rendering::Cubemap* cubemap =
                            RTBEngine::Core::ResourceManager::GetInstance().LoadCubemapAsset(assetPath);
                        if (cubemap) {
                            activeScene->SetSkyboxCubemap(cubemap);
                            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // Clear cubemap button
                ImGui::SameLine();
                if (ImGui::Button("x", ImVec2(22.0f, 0))) {
                    activeScene->SetSkyboxCubemap(nullptr);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }

            for (const auto& gameObject : activeScene->GetGameObjects()) {
                // Only start drawing from root objects (those without a parent)
                if (gameObject->GetParent() == nullptr) {
                    DrawGameObjectNode(gameObject.get(), context);
                }
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ClearSelection(context);
            }

            // Delete selected GameObjects (supports multi-selection)
            if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                if (!context.selectedGameObjects.empty() && activeScene) {
                    // Build a unique list of roots (no object is a descendant of another selected one)
                    std::vector<RTBEngine::ECS::GameObject*> roots = context.selectedGameObjects;

                    auto isDescendantOfAnyRoot = [](RTBEngine::ECS::GameObject* candidate,
                        const std::vector<RTBEngine::ECS::GameObject*>& all) {
                        for (auto* other : all) {
                            if (other == candidate) continue;
                            RTBEngine::ECS::GameObject* p = candidate->GetParent();
                            while (p) {
                                if (p == other) return true;
                                p = p->GetParent();
                            }
                        }
                        return false;
                    };

                    roots.erase(
                        std::remove_if(
                            roots.begin(),
                            roots.end(),
                            [&](RTBEngine::ECS::GameObject* go) { return isDescendantOfAnyRoot(go, roots); }),
                        roots.end());

                    for (auto* go : roots) {
                        if (go) {
                            DeleteGameObject(activeScene, go, context);
                        }
                    }

                    ClearSelection(context);
                } else if (context.selectedGameObject && activeScene) {
                    DeleteGameObject(activeScene, context.selectedGameObject, context);
                    ClearSelection(context);
                }
            }

            // Parent for newly created objects: same parent as selected, or root if none
            RTBEngine::ECS::GameObject* creationParent = context.selectedGameObject
                ? context.selectedGameObject->GetParent()
                : nullptr;

            // Context menu for the window
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::BeginMenu("GameObject")) {
                    if (ImGui::MenuItem("Empty Object")) {
                        auto* go = new RTBEngine::ECS::GameObject("GameObject");
                        if (creationParent) go->SetParent(creationParent);
                        activeScene->AddGameObject(go);
                        context.selectedGameObject = go;
                        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                    }
                    if (ImGui::MenuItem("Sphere")) {
                        CreateSphere(activeScene, context, creationParent);
                    }
                    if (ImGui::MenuItem("Cube")) {
                        CreateCube(activeScene, context, creationParent);
                    }
                    if (ImGui::MenuItem("Plane")) {
                        CreatePlane(activeScene, context, creationParent);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("UI")) {
                    if (ImGui::MenuItem("Empty")) {
                        auto* go = new RTBEngine::ECS::GameObject("GameObject");
                        auto* container = new RTBEngine::UI::UIContainer();
                        go->AddComponent(container);
                        if (creationParent) go->SetParent(creationParent);
                        activeScene->AddGameObject(go);
                        context.selectedGameObject = go;
                        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                    }
                    if (ImGui::MenuItem("Canvas")) {
                        CreateCanvas(activeScene, context, creationParent);
                    }
                    if (ImGui::MenuItem("Button")) {
                        CreateUIButton(activeScene, context, creationParent);
                    }
                    if (ImGui::MenuItem("InputField")) {
                        CreateUIInputField(activeScene, context, creationParent);
                    }
                    if (ImGui::MenuItem("Slider")) {
                        CreateUISlider(activeScene, context, creationParent);
                    }
                    if (ImGui::MenuItem("Text")) {
                        CreateUIText(activeScene, context, creationParent);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        }

        // Invisible drop area covering remaining empty space
        ImVec2 availableRegion = ImGui::GetContentRegionAvail();
        float minHeight = 50.0f;
        if (availableRegion.y < minHeight) availableRegion.y = minHeight;
        ImGui::InvisibleButton("##HierarchyDropArea", ImVec2(availableRegion.x, availableRegion.y));

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                const GameObjectPayload* data = static_cast<const GameObjectPayload*>(payload->Data);
                RTBEngine::ECS::GameObject* dragged = reinterpret_cast<RTBEngine::ECS::GameObject*>(data->gameObjectId);
                if (dragged && dragged->GetParent()) {
                    dragged->SetParent(nullptr);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_PREFAB)) {
                const PrefabPayload* data = static_cast<const PrefabPayload*>(payload->Data);

                std::filesystem::path assetRoot = GetAssetRootPath();
                std::string absolutePath = (assetRoot / data->path).string();

                RTBEngine::ECS::Prefab* prefab = RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(absolutePath);

                if (prefab && activeScene) {
                    std::vector<RTBEngine::ECS::GameObject*> childGOs;
                    RTBEngine::ECS::GameObject* go = prefab->Instantiate(nullptr, childGOs);
                    if (go) {
                        activeScene->AddGameObject(go);
                        for (auto* child : childGOs) {
                            if (child) activeScene->AddGameObject(child);
                        }
                        context.selectedGameObject = go;
                        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                    }
                }
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_MESH)) {
                const MeshPayload* data = static_cast<const MeshPayload*>(payload->Data);
                const std::string assetPath = MakeAssetReference(data->path);

                if (activeScene) {
                    auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
                    RTBEngine::Rendering::ModelData modelData =
                        RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(assetPath);
                    resources.RegisterMeshes(assetPath, modelData.meshes);

                    RTBEngine::ECS::GameObject* root =
                        RTBEngine::Rendering::BuildFbxHierarchy(activeScene, modelData, assetPath, resources);
                    if (root) {
                        context.selectedGameObject = root;
                        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawGameObjectNode(RTBEngine::ECS::GameObject* gameObject, EditorContext& context) {
        auto& name = gameObject->GetName();

        bool isSelected = std::find(
            context.selectedGameObjects.begin(),
            context.selectedGameObjects.end(),
            gameObject) != context.selectedGameObjects.end();

        ImGuiTreeNodeFlags flags = (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        
        const auto& children = gameObject->GetChildren();
        if (children.empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        // Using pointer as ID to ensure uniqueness
        int styleColorCount = 0;
        if (gameObject->IsTransient()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 0.7f));
            styleColorCount++;
        }
        else if (gameObject->IsPrefabInstance()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
            styleColorCount++;
        }

        bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, name.c_str());

        if (styleColorCount > 0)
            ImGui::PopStyleColor(styleColorCount);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)
            && ImGui::GetDragDropPayload() == nullptr) {
            if (ImGui::GetIO().KeyCtrl) {
                ToggleSelection(context, gameObject);
            } else {
                SetSingleSelection(context, gameObject);
            }
        }

        // Drag-and-drop source for GameObject
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            GameObjectPayload payload;
            payload.gameObjectId = reinterpret_cast<uint64_t>(gameObject);
            ImGui::SetDragDropPayload(PAYLOAD_GAMEOBJECT, &payload, sizeof(GameObjectPayload));
            ImGui::Text("GameObject: %s", name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drop target: reparent GO or instantiate Prefab as child of this node
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                const GameObjectPayload* data = static_cast<const GameObjectPayload*>(payload->Data);
                RTBEngine::ECS::GameObject* dragged = reinterpret_cast<RTBEngine::ECS::GameObject*>(data->gameObjectId);

                // Prevent parenting a node to itself or one of its own descendants
                bool isCycle = false;
                RTBEngine::ECS::GameObject* check = gameObject;
                while (check) {
                    if (check == dragged) { isCycle = true; break; }
                    check = check->GetParent();
                }

                if (!isCycle) {
                    dragged->SetParent(gameObject);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                }
            }

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_PREFAB)) {
                const PrefabPayload* data = static_cast<const PrefabPayload*>(payload->Data);

                std::filesystem::path assetRoot = GetAssetRootPath();
                std::string absolutePath = (assetRoot / data->path).string();

                RTBEngine::ECS::Prefab* prefab = RTBEngine::ECS::PrefabRegistry::GetInstance().GetByPath(absolutePath);
                RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
                if (prefab && scene) {
                    std::vector<RTBEngine::ECS::GameObject*> childGOs;
                    RTBEngine::ECS::GameObject* go = prefab->Instantiate(gameObject, childGOs);
                    if (go) {
                        scene->AddGameObject(go);
                        for (auto* child : childGOs) {
                            if (child) scene->AddGameObject(child);
                        }
                        context.selectedGameObject = go;
                        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        if (opened) {
            for (auto* child : children) {
                DrawGameObjectNode(child, context);
            }
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::CreateSphere(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Sphere");
        if (parent) go->SetParent(parent);

        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        auto* renderer = new RTBEngine::ECS::MeshRenderer();
        renderer->SetMesh(resources.GetDefaultSphere());
        renderer->SetShader(resources.GetShader("basic"));
        go->AddComponent(renderer);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateCube(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Cube");
        if (parent) go->SetParent(parent);

        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        auto* renderer = new RTBEngine::ECS::MeshRenderer();
        renderer->SetMesh(resources.GetDefaultCube());
        renderer->SetShader(resources.GetShader("basic"));
        go->AddComponent(renderer);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreatePlane(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Plane");
        if (parent) go->SetParent(parent);

        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
        auto* renderer = new RTBEngine::ECS::MeshRenderer();
        renderer->SetMesh(resources.GetDefaultPlane());
        renderer->SetShader(resources.GetShader("basic"));
        go->AddComponent(renderer);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateCanvas(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Canvas");
        if (parent) go->SetParent(parent);

        auto* canvas = new RTBEngine::UI::Canvas();
        canvas->SetRenderMode(RTBEngine::UI::Canvas::RenderMode::ScreenSpaceOverlay);
        go->AddComponent(canvas);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateUIText(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Text");
        if (parent) go->SetParent(parent);

        auto* uiText = new RTBEngine::UI::UIText();
        uiText->SetAnchorMin(RTBEngine::Math::Vector2(0.5f, 0.5f));
        uiText->SetAnchorMax(RTBEngine::Math::Vector2(0.5f, 0.5f));
        uiText->SetAnchoredPosition(RTBEngine::Math::Vector2(0.0f, 0.0f));
        uiText->SetSizeDelta(RTBEngine::Math::Vector2(160.0f, 30.0f));
        uiText->SetText("Text");
        uiText->SetAlignment(RTBEngine::UI::TextAlignment::Center);
        go->AddComponent(uiText);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateUIButton(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        // Button GO: UIPanel (visual + hit area) + UIButton (logic) on the same GameObject
        auto* buttonGO = new RTBEngine::ECS::GameObject("Button");
        if (parent) buttonGO->SetParent(parent);

        auto* uiPanel = new RTBEngine::UI::UIPanel();
        uiPanel->SetAnchorMin(RTBEngine::Math::Vector2(0.5f, 0.5f));
        uiPanel->SetAnchorMax(RTBEngine::Math::Vector2(0.5f, 0.5f));
        uiPanel->SetAnchoredPosition(RTBEngine::Math::Vector2(0.0f, 0.0f));
        uiPanel->SetSizeDelta(RTBEngine::Math::Vector2(160.0f, 40.0f));
        buttonGO->AddComponent(uiPanel);

        // UIPanel must be added first so UIButton::OnAwake() finds it via GetComponent<UIPanel>()
        auto* uiButton = new RTBEngine::UI::UIButton();
        buttonGO->AddComponent(uiButton);

        scene->AddGameObject(buttonGO);

        // Text GO (child): UIText centered, stretches to fill parent
        auto* textGO = new RTBEngine::ECS::GameObject("Text");
        textGO->SetParent(buttonGO);

        auto* uiText = new RTBEngine::UI::UIText();
        uiText->SetAnchorMin(RTBEngine::Math::Vector2(0.0f, 0.0f));
        uiText->SetAnchorMax(RTBEngine::Math::Vector2(1.0f, 1.0f));
        uiText->SetAnchoredPosition(RTBEngine::Math::Vector2(0.0f, 0.0f));
        uiText->SetSizeDelta(RTBEngine::Math::Vector2(0.0f, 0.0f));
        uiText->SetText("Button");
        uiText->SetAlignment(RTBEngine::UI::TextAlignment::Center);
        textGO->AddComponent(uiText);

        scene->AddGameObject(textGO);

        context.selectedGameObject = buttonGO;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateUIInputField(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* inputGO = new RTBEngine::ECS::GameObject("InputField");
        if (parent) inputGO->SetParent(parent);

        auto* panel = new RTBEngine::UI::UIPanel();
        panel->SetAnchorMin(RTBEngine::Math::Vector2(0.5f, 0.5f));
        panel->SetAnchorMax(RTBEngine::Math::Vector2(0.5f, 0.5f));
        panel->SetAnchoredPosition(RTBEngine::Math::Vector2(0.0f, 0.0f));
        panel->SetSizeDelta(RTBEngine::Math::Vector2(260.0f, 42.0f));
        panel->SetBackgroundColor(RTBEngine::Math::Vector4(0.10f, 0.11f, 0.14f, 0.88f));
        panel->SetBorderColor(RTBEngine::Math::Vector4(0.62f, 0.69f, 0.94f, 1.0f));
        panel->SetBorderThickness(1.0f);
        panel->SetHasBorder(true);
        inputGO->AddComponent(panel);

        auto* inputField = new RTBEngine::UI::UIInputField();
        inputField->placeholder = "Enter name";
        inputField->maxLength = 24;
        inputField->backgroundPanel = panel;
        inputGO->AddComponent(inputField);

        scene->AddGameObject(inputGO);

        auto* textGO = new RTBEngine::ECS::GameObject("Text");
        textGO->SetParent(inputGO);

        auto* uiText = new RTBEngine::UI::UIText();
        uiText->SetAnchorMin(RTBEngine::Math::Vector2(0.0f, 0.0f));
        uiText->SetAnchorMax(RTBEngine::Math::Vector2(1.0f, 1.0f));
        uiText->SetAnchoredPosition(RTBEngine::Math::Vector2(12.0f, 0.0f));
        uiText->SetSizeDelta(RTBEngine::Math::Vector2(-24.0f, 0.0f));
        uiText->SetText("Enter name");
        uiText->SetColor(RTBEngine::Math::Vector4(0.68f, 0.70f, 0.76f, 1.0f));
        uiText->SetAlignment(RTBEngine::UI::TextAlignment::Left);
        uiText->SetRaycastTarget(false);
        textGO->AddComponent(uiText);

        inputField->textComponent = uiText;
        inputField->OnValidate();

        scene->AddGameObject(textGO);

        context.selectedGameObject = inputGO;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateUISlider(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* sliderGO = new RTBEngine::ECS::GameObject("Slider");
        if (parent) sliderGO->SetParent(parent);

        auto* trackPanel = new RTBEngine::UI::UIPanel();
        trackPanel->SetAnchorMin(RTBEngine::Math::Vector2(0.5f, 0.5f));
        trackPanel->SetAnchorMax(RTBEngine::Math::Vector2(0.5f, 0.5f));
        trackPanel->SetAnchoredPosition(RTBEngine::Math::Vector2(0.0f, 0.0f));
        trackPanel->SetSizeDelta(RTBEngine::Math::Vector2(200.0f, 20.0f));
        trackPanel->SetBackgroundColor(RTBEngine::Math::Vector4(0.18f, 0.18f, 0.18f, 1.0f));
        sliderGO->AddComponent(trackPanel);

        auto* slider = new RTBEngine::UI::UISlider();
        slider->minValue = 0.0f;
        slider->maxValue = 1.0f;
        slider->value = 0.5f;
        sliderGO->AddComponent(slider);

        scene->AddGameObject(sliderGO);

        auto* fillGO = new RTBEngine::ECS::GameObject("Fill");
        fillGO->SetParent(sliderGO);

        auto* fillPanel = new RTBEngine::UI::UIPanel();
        fillPanel->SetAnchorMin(RTBEngine::Math::Vector2(0.0f, 0.0f));
        fillPanel->SetAnchorMax(RTBEngine::Math::Vector2(0.0f, 1.0f));
        fillPanel->SetPivot(RTBEngine::Math::Vector2(0.0f, 0.5f));
        fillPanel->SetAnchoredPosition(RTBEngine::Math::Vector2(0.0f, 0.0f));
        fillPanel->SetSizeDelta(RTBEngine::Math::Vector2(100.0f, 0.0f));
        fillPanel->SetBackgroundColor(RTBEngine::Math::Vector4(0.22f, 0.72f, 0.28f, 1.0f));
        fillPanel->SetRaycastTarget(false);
        fillGO->AddComponent(fillPanel);
        scene->AddGameObject(fillGO);

        auto* handleGO = new RTBEngine::ECS::GameObject("Handle");
        handleGO->SetParent(sliderGO);

        auto* handlePanel = new RTBEngine::UI::UIPanel();
        handlePanel->SetAnchorMin(RTBEngine::Math::Vector2(0.0f, 0.5f));
        handlePanel->SetAnchorMax(RTBEngine::Math::Vector2(0.0f, 0.5f));
        handlePanel->SetPivot(RTBEngine::Math::Vector2(0.5f, 0.5f));
        handlePanel->SetAnchoredPosition(RTBEngine::Math::Vector2(100.0f, 0.0f));
        handlePanel->SetSizeDelta(RTBEngine::Math::Vector2(20.0f, 20.0f));
        handlePanel->SetBackgroundColor(RTBEngine::Math::Vector4(0.92f, 0.92f, 0.92f, 1.0f));
        handlePanel->SetRaycastTarget(false);
        handleGO->AddComponent(handlePanel);
        scene->AddGameObject(handleGO);

        slider->fillPanel = fillPanel;
        slider->handlePanel = handlePanel;
        slider->SetValue(0.5f);

        context.selectedGameObject = sliderGO;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::DeleteGameObject(RTBEngine::ECS::Scene* scene, RTBEngine::ECS::GameObject* gameObject, EditorContext& context) {
        scene->RemoveGameObject(gameObject);
        context.selectedGameObject = nullptr;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

}
