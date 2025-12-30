#pragma once
#include "Panels/EditorPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "MainMenuBar.h"
#include <imgui.h>
#include <vector>
#include <memory>
#include <SDL.h>

/**
 * UI inspired by Dear ImGui Demo and Wiki.
 * Reference: https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
 * Docking API based on imgui_internal.h (Docking branch)
 */
namespace RTBEditor {

    class EditorLayer {
    public:
        EditorLayer();
        ~EditorLayer();

        void Initialize(SDL_Window* window);
        void Begin();
        void End();
        void OnUIRender();

        void AddPanel(std::unique_ptr<EditorPanel> panel);
        MainMenuBar* GetMenuBar() { return menuBar.get(); }
        SceneViewPanel* GetSceneViewPanel() { return sceneViewPanel; }

    private:
        void SetupDockspace();
        void CreateDefaultLayout(ImGuiID dockspaceId);

    private:
        EditorContext context;
        std::unique_ptr<MainMenuBar> menuBar;
        std::vector<std::unique_ptr<EditorPanel>> panels;
        SceneViewPanel* sceneViewPanel = nullptr;
        bool isDockspaceOpen = true;
    };

}
