#include "EditorLayer.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

namespace RTBEditor {

    EditorLayer::EditorLayer() {
        menuBar = std::make_unique<MainMenuBar>();
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
            panel->OnUIRender();
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

        // DockSpace
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

        // Menu bar
        if (menuBar) {
            menuBar->OnUIRender();
        }

        ImGui::End();
    }

}
