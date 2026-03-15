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
        std::unique_ptr<RTBEngine::ECS::Prefab> prefab; // null si es copia directa
        RTBEngine::ECS::GameObject* source = nullptr;    // solo válido mientras exista el GO original
        bool isPrefabInstanceSource = false;
        RTBEngine::Math::Vector3 position;
        RTBEngine::Math::Quaternion rotation;
        RTBEngine::Math::Vector3 scale;
    };

    class EditorLayer {
    public:
        EditorLayer();
        ~EditorLayer();

        void Initialize(SDL_Window* window, void* imguiContext = nullptr);
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
        RTBEngine::ECS::GameObject* GetSelectedGameObject() const { return context.selectedGameObject; }
        void SetSelectedGameObject(RTBEngine::ECS::GameObject* go) { context.selectedGameObject = go; }
        EditorContext& GetContext() { return context; }
        void OpenBuildDialog();

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

        void HandleGlobalShortcuts();
        void CopySelectionToClipboard();
        void PasteClipboardIntoScene();
    };

}
