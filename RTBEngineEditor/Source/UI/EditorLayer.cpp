#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "EditorLayer.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/OnlinePanel.h"
#include "MainMenuBar.h"
#include <imgui_internal.h>
#include <utility>
#include <algorithm>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Core/Logger.h>

namespace RTBEditor {

    EditorLayer::EditorLayer() {
        menuBar = std::make_unique<MainMenuBar>();
        buildDialog = std::make_unique<BuildDialog>();
        
        // Add default panels
        auto sceneView = std::make_unique<SceneViewPanel>();
        sceneViewPanel = sceneView.get();
        AddPanel(std::move(sceneView));

        auto gameView = std::make_unique<GameViewPanel>();
        gameViewPanel = gameView.get();
        AddPanel(std::move(gameView));
        
        AddPanel(std::make_unique<SceneHierarchyPanel>());
        AddPanel(std::make_unique<InspectorPanel>());
        AddPanel(std::make_unique<ContentBrowserPanel>());
        AddPanel(std::make_unique<ConsolePanel>());
        AddPanel(std::make_unique<OnlinePanel>());
        AddPanel(std::make_unique<StatsOverlayPanel>());
    }

    EditorLayer::~EditorLayer() {}

    void EditorLayer::Initialize(void* imguiContext) {
        if (imguiContext) {
            ImGui::SetCurrentContext((ImGuiContext*)imguiContext);
        }
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    void EditorLayer::Begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void EditorLayer::OnUIRender() {
        PruneSelectionToScene(
            context,
            RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene());

        SetupDockspace();

        // Render all registered panels
        for (auto& panel : panels) {
            panel->OnUIRender(context);
        }

        HandleGlobalShortcuts();

        if (buildDialog) {
            buildDialog->Render();
        }

        if (renderPopupCallback) {
            renderPopupCallback();
        }
    }

    void EditorLayer::End() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }

    void EditorLayer::AddPanel(std::unique_ptr<EditorPanel> panel) {
        panels.push_back(std::move(panel));
    }

    void EditorLayer::SetupDockspace() {
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // Configure the background window for the dockspace
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        
        // Match the background window to the main viewport
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        // Make the background window look clean
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        
        // Disable title bar, resizing, and movement for the root dock window
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("MainDockSpace", &isDockspaceOpen, windowFlags);
        ImGui::PopStyleVar(2);

        // Core DockSpace logic
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

        // Setup default layout once
        static bool firstTime = true;
        if (firstTime) {
            firstTime = false;
            CreateDefaultLayout(dockspaceId);
        }

        // Menu bar
        if (menuBar) {
            menuBar->OnUIRender();
        }

        ImGui::End();
    }

    void EditorLayer::CreateDefaultLayout(ImGuiID dockspaceId) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

        // Split nodes
        ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Up, 0.06f, nullptr, &dockspaceId);
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &dockspaceId);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.25f, nullptr, &dockspaceId);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.3f, nullptr, &dockspaceId);
        ImGuiID dock_id_center = dockspaceId; // The remaining space is the center
        
        // Assign windows
        ImGui::DockBuilderDockWindow("Toolbar", dock_id_top);
        ImGui::DockBuilderDockWindow("Scene", dock_id_center);
        ImGui::DockBuilderDockWindow("Game", dock_id_center);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
        ImGui::DockBuilderDockWindow("Online", dock_id_right);
        ImGui::DockBuilderDockWindow("Content Browser", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
        
        ImGui::DockBuilderFinish(dockspaceId);
    }

    void EditorLayer::SetupToolbar(std::function<void()> onPlay,
                                   std::function<void()> onPause,
                                   std::function<void()> onStop,
                                   std::function<EditorState()> getState,
                                   std::function<bool()> isCompilingScriptsProvider,
                                   std::function<void()> onCompileScripts) {
        AddPanel(std::make_unique<ToolbarPanel>(onPlay, onPause, onStop, getState, isCompilingScriptsProvider, onCompileScripts));
    }

    void EditorLayer::OpenBuildDialog() {
        if (buildDialog) buildDialog->Open();
    }

    void EditorLayer::ClearSelection() {
        RTBEditor::ClearSelection(context);
    }

    void EditorLayer::HandleGlobalShortcuts() {
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard) {
            return;
        }

        if (io.WantTextInput) {
            return;
        }

        const bool ctrlDown = io.KeyCtrl;

        if (ctrlDown && ImGui::IsKeyPressed(ImGuiKey_C, false)) {
            CopySelectionToClipboard();
        }

        if (ctrlDown && ImGui::IsKeyPressed(ImGuiKey_V, false)) {
            PasteClipboardIntoScene();
        }
    }

    void EditorLayer::CopySelectionToClipboard() {
        clipboardPrefabs.clear();

        if (context.selectedGameObjects.empty()) {
            if (context.selectedGameObject) {
                context.selectedGameObjects.push_back(context.selectedGameObject);
            } else {
                return;
            }
        }

        for (auto* go : context.selectedGameObjects) {
            if (!go) continue;

            ClipboardEntry entry;
            entry.source = go;
            entry.baseName = go->GetName();
            entry.pasteCount = 0;

            auto& t = go->GetTransform();
            entry.position = t.GetPosition();
            entry.rotation = t.GetRotation();
            entry.scale = t.GetScale();

            // Always snapshot at copy time to avoid dangling source pointers
            auto prefab = RTBEngine::ECS::Prefab::CreateFromGameObject(go);
            if (prefab) {
                entry.prefab = std::move(prefab);
                entry.isPrefabInstanceSource = go->IsPrefabInstance();
            }

            clipboardPrefabs.push_back(std::move(entry));
        }
    }

    void EditorLayer::PasteClipboardIntoScene() {
        if (clipboardPrefabs.empty()) {
            return;
        }

        auto* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) {
            return;
        }

        RTBEditor::ClearSelection(context);

        const float positionOffset = 0.5f;
        int index = 0;

        for (auto& entry : clipboardPrefabs) {
            RTBEngine::ECS::GameObject* go = nullptr;
            std::vector<RTBEngine::ECS::GameObject*> childGOs;

            if (entry.prefab) {
                go = entry.prefab->Instantiate(nullptr, childGOs);

                if (go && !entry.isPrefabInstanceSource) {
                    // No debe tratarse como instancia de prefab real
                    go->SetPrefabName("");
                }
            } else if (entry.source) {
                auto tempPrefab = RTBEngine::ECS::Prefab::CreateFromGameObject(entry.source);
                if (tempPrefab) {
                    go = tempPrefab->Instantiate(nullptr, childGOs);
                    if (go) {
                        // Mantener el nombre visible del objeto original
                        go->SetName(entry.source->GetName());
                        // No es instancia de prefab
                        go->SetPrefabName("");
                    }
                }
            }

            // Clear prefab name on all child GOs when not a real prefab instance
            if (!entry.isPrefabInstanceSource) {
                for (auto* child : childGOs) {
                    if (child)
                        child->SetPrefabName("");
                }
            }

            if (!go) continue;

            entry.pasteCount++;
            go->SetName(entry.baseName + " (" + std::to_string(entry.pasteCount) + ")");

            auto& transform = go->GetTransform();
            transform.SetPosition(entry.position);
            transform.SetRotation(entry.rotation);
            transform.SetScale(entry.scale);

            auto posOffset = transform.GetPosition();
            posOffset.x += positionOffset * index;
            posOffset.z += positionOffset * index;
            transform.SetPosition(posOffset);

            scene->AddGameObject(go);

            // Add child GOs to scene so Scene::Render can iterate them
            for (auto* child : childGOs) {
                if (child) scene->AddGameObject(child);
            }

            ToggleSelection(context, go);
        }

        RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
    }

}
