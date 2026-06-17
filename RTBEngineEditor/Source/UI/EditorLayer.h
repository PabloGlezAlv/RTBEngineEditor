#pragma once
#include "Panels/EditorPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/GameViewPanel.h"
#include "Panels/ToolbarPanel.h"
#include "Panels/StatsOverlayPanel.h"
#include "Modals/BuildDialog.h"
#include "MainMenuBar.h"
#include <imgui.h>
#include <vector>
#include <memory>
#include <SDL.h>
#include <functional>
#include <RTBEngine/ECS/Prefab.h>
#include <RTBEngine/Math/Math.h>

/**
 * UI inspired by Dear ImGui Demo and Wiki.
 * Reference: https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
 * Docking API based on imgui_internal.h (Docking branch)
 */
namespace RTBEditor {

    struct ClipboardEntry {
        std::unique_ptr<RTBEngine::ECS::Prefab> prefab;
        bool isPrefabInstanceSource = false;
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Quaternion rotation;
        RTBEngine::Math::Vector3 scale;
        std::string baseName;
        std::string sourceScenePath;
        int pasteCount = 0;
    };

    class EditorLayer {
    public:
        EditorLayer();
        ~EditorLayer();

        void Initialize(void* imguiContext = nullptr);
        void Begin();
        void End();
        void OnUIRender();

        void AddPanel(std::unique_ptr<EditorPanel> panel);
        MainMenuBar* GetMenuBar() { return menuBar.get(); }
        SceneViewPanel* GetSceneViewPanel() { return sceneViewPanel; }
        GameViewPanel* GetGameViewPanel() { return gameViewPanel; }
        void SetupToolbar(std::function<void()> onPlay,
                          std::function<void()> onPause,
                          std::function<void()> onStop,
                          std::function<EditorState()> getState,
                          std::function<bool()> isCompilingScriptsProvider,
                          std::function<void()> onCompileScripts);
        void SetEditorState(EditorState s) { context.state = s; }
        void SetRenderPopupCallback(std::function<void()> cb) {renderPopupCallback = cb;}
        RTBEngine::ECS::GameObject* GetSelectedGameObject() const { return context.selectedGameObject; }
        void SetSelectedGameObject(RTBEngine::ECS::GameObject* go) { SetSingleSelection(context, go); }
        void ClearSelection();
        void CopySelectionToClipboard();
        void PasteClipboardIntoScene();
        void DuplicateSelection();
        bool HasClipboardContent() const { return !clipboardPrefabs.empty(); }
        EditorContext& GetContext() { return context; }
        void OpenBuildDialog();
        void PersistWindowPrefs();

    private:
        void SetupDockspace();
        void CreateDefaultLayout(ImGuiID dockspaceId);

    private:
        EditorContext context;
        std::unique_ptr<MainMenuBar> menuBar;
        std::unique_ptr<BuildDialog> buildDialog;
        std::vector<std::unique_ptr<EditorPanel>> panels;
        SceneViewPanel* sceneViewPanel = nullptr;
        GameViewPanel* gameViewPanel = nullptr;
        bool isDockspaceOpen = true;

        std::vector<ClipboardEntry> clipboardPrefabs; 
        
        std::function<void()> renderPopupCallback;

        void HandleGlobalShortcuts();
    };

}