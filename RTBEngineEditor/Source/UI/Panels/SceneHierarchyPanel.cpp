#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/ECS/PrefabRegistry.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Rendering/Cubemap.h>
#include <RTBEngine/UI/Canvas.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIContainer.h>
#include <RTBEngine/UI/Elements/UIPanel.h>
#include <RTBEngine/UI/Elements/UIText.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include "../DragDropPayloads.h"
#include "../../Project/Project.h"
#include <filesystem>
#include <vector>

namespace RTBEditor {

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

                        std::filesystem::path assetRoot = Project::GetActiveProject()
                            ? Project::GetActiveProject()->GetAssetDirectory()
                            : std::filesystem::path("Assets");
                        std::string absolutePath = (assetRoot / data->path).string();

                        RTBEngine::Rendering::Cubemap* cubemap =
                            RTBEngine::Core::ResourceManager::GetInstance().LoadCubemapAsset(absolutePath);
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

                std::filesystem::path assetRoot = Project::GetActiveProject()
                    ? Project::GetActiveProject()->GetAssetDirectory()
                    : std::filesystem::path("Assets");
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
        if (gameObject->IsPrefabInstance())
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));

        bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, name.c_str());

        if (gameObject->IsPrefabInstance())
            ImGui::PopStyleColor();

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

                std::filesystem::path assetRoot = Project::GetActiveProject()
                    ? Project::GetActiveProject()->GetAssetDirectory()
                    : std::filesystem::path("Assets");
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
        uiText->anchorMin        = RTBEngine::Math::Vector2(0.5f, 0.5f);
        uiText->anchorMax        = RTBEngine::Math::Vector2(0.5f, 0.5f);
        uiText->anchoredPosition = RTBEngine::Math::Vector2(0.0f, 0.0f);
        uiText->sizeDelta        = RTBEngine::Math::Vector2(160.0f, 30.0f);
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
        uiPanel->anchorMin        = RTBEngine::Math::Vector2(0.5f, 0.5f);
        uiPanel->anchorMax        = RTBEngine::Math::Vector2(0.5f, 0.5f);
        uiPanel->anchoredPosition = RTBEngine::Math::Vector2(0.0f, 0.0f);
        uiPanel->sizeDelta        = RTBEngine::Math::Vector2(160.0f, 40.0f);
        buttonGO->AddComponent(uiPanel);

        // UIPanel must be added first so UIButton::OnAwake() finds it via GetComponent<UIPanel>()
        auto* uiButton = new RTBEngine::UI::UIButton();
        buttonGO->AddComponent(uiButton);

        scene->AddGameObject(buttonGO);

        // Text GO (child): UIText centered, stretches to fill parent
        auto* textGO = new RTBEngine::ECS::GameObject("Text");
        textGO->SetParent(buttonGO);

        auto* uiText = new RTBEngine::UI::UIText();
        uiText->anchorMin        = RTBEngine::Math::Vector2(0.0f, 0.0f);
        uiText->anchorMax        = RTBEngine::Math::Vector2(1.0f, 1.0f);
        uiText->anchoredPosition = RTBEngine::Math::Vector2(0.0f, 0.0f);
        uiText->sizeDelta        = RTBEngine::Math::Vector2(0.0f, 0.0f);
        uiText->SetText("Button");
        uiText->SetAlignment(RTBEngine::UI::TextAlignment::Center);
        textGO->AddComponent(uiText);

        scene->AddGameObject(textGO);

        context.selectedGameObject = buttonGO;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CollectDescendants(RTBEngine::ECS::GameObject* gameObject, std::vector<RTBEngine::ECS::GameObject*>& out) {
        for (auto* child : gameObject->GetChildren()) {
            CollectDescendants(child, out);
            out.push_back(child);
        }
    }

    void SceneHierarchyPanel::DeleteGameObject(RTBEngine::ECS::Scene* scene, RTBEngine::ECS::GameObject* gameObject, EditorContext& context) {
        // Collect all descendants (deepest first) so they can be removed from the scene
        std::vector<RTBEngine::ECS::GameObject*> descendants;
        CollectDescendants(gameObject, descendants);

        // Detach descendants from their parents to avoid dangling child pointers
        for (auto* desc : descendants) {
            if (desc->GetParent()) {
                desc->SetParent(nullptr);
            }
        }

        // Detach the root GO from its parent
        if (gameObject->GetParent()) {
            gameObject->SetParent(nullptr);
        }

        // Remove descendants from scene (unique_ptr destroyed here)
        for (auto* desc : descendants) {
            scene->RemoveGameObject(desc);
        }

        // Remove the GO itself
        scene->RemoveGameObject(gameObject);

        context.selectedGameObject = nullptr;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

}
