#include "EditorLayer.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "MainMenuBar.h"
#include <imgui_internal.h>
#include <utility>
#include <algorithm>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

namespace RTBEditor {

    EditorLayer::EditorLayer() {
        menuBar = std::make_unique<MainMenuBar>();
        
        // Add default panels
        AddPanel(std::make_unique<SceneHierarchyPanel>());
        AddPanel(std::make_unique<InspectorPanel>());
        AddPanel(std::make_unique<ContentBrowserPanel>());
        AddPanel(std::make_unique<ConsolePanel>());
    }

    EditorLayer::~EditorLayer() {}

    void EditorLayer::Initialize(SDL_Window* window) {
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
        SetupDockspace();

        // Render all registered panels
        for (auto& panel : panels) {
            panel->OnUIRender(context);
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
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &dockspaceId);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.2f, nullptr, &dockspaceId);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.3f, nullptr, &dockspaceId);
        
        // Assign windows
        ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
        ImGui::DockBuilderDockWindow("Content Browser", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
        
        ImGui::DockBuilderFinish(dockspaceId);
    }

}
