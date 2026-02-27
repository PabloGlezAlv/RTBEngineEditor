#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/MeshRenderer.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/UI/Canvas.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIContainer.h>
#include <RTBEngine/UI/Elements/UIPanel.h>
#include <RTBEngine/UI/Elements/UIText.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include "../DragDropPayloads.h"
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

            for (const auto& gameObject : activeScene->GetGameObjects()) {
                // Only start drawing from root objects (those without a parent)
                if (gameObject->GetParent() == nullptr) {
                    DrawGameObjectNode(gameObject.get(), context);
                }
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
                context.selectedGameObject = nullptr;
            }

            // Delete selected GameObject
            if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete) && context.selectedGameObject) {
                DeleteGameObject(activeScene, context.selectedGameObject, context);
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
            payload.gameObjectId = reinterpret_cast<uint64_t>(gameObject);
            ImGui::SetDragDropPayload(PAYLOAD_GAMEOBJECT, &payload, sizeof(GameObjectPayload));
            ImGui::Text("GameObject: %s", name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drop target: reparent dragged GO under this node
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

        auto* renderer = new RTBEngine::ECS::MeshRenderer();
        renderer->SetMesh(RTBEngine::Core::ResourceManager::GetInstance().GetDefaultSphere());
        go->AddComponent(renderer);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreateCube(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Cube");
        if (parent) go->SetParent(parent);

        auto* renderer = new RTBEngine::ECS::MeshRenderer();
        renderer->SetMesh(RTBEngine::Core::ResourceManager::GetInstance().GetDefaultCube());
        go->AddComponent(renderer);

        scene->AddGameObject(go);
        context.selectedGameObject = go;
        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

    void SceneHierarchyPanel::CreatePlane(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent) {
        auto* go = new RTBEngine::ECS::GameObject("Plane");
        if (parent) go->SetParent(parent);

        auto* renderer = new RTBEngine::ECS::MeshRenderer();
        renderer->SetMesh(RTBEngine::Core::ResourceManager::GetInstance().GetDefaultPlane());
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
