#include "ToolbarPanel.h"
#include <imgui.h>

namespace RTBEditor {

    ToolbarPanel::ToolbarPanel(std::function<void()> onPlay, std::function<void()> onPause, std::function<void()> onStop, std::function<EditorState()> getState, std::function<void()> onCompileScripts)
        : onPlay(onPlay), onPause(onPause), onStop(onStop), getState(getState), onCompileScripts(onCompileScripts) {}

    void ToolbarPanel::OnUIRender(EditorContext& context) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        
        // Toolbar window with no decoration
        ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

        EditorState state = getState();

        // Center buttons
        float width = ImGui::GetWindowContentRegionMax().x;
        float buttonsWidth = 120.0f; 
        ImGui::SetCursorPosX((width - buttonsWidth) * 0.5f);

        // Play/Stop Button
        if (state == EditorState::Edit) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.7f, 0, 1)); // Green
            if (ImGui::Button("Play", ImVec2(50, 0))) {
                onPlay();
            }
            ImGui::PopStyleColor();
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0, 0, 1)); // Red
            if (ImGui::Button("Stop", ImVec2(50, 0))) {
                onStop();
            }
            ImGui::PopStyleColor();
        }

        ImGui::SameLine();
        
        // Pause/Resume Button
        bool isPaused = (state == EditorState::Pause);
        const char* pauseLabel = isPaused ? "Resume" : "Pause";
        
        if (state == EditorState::Edit) {
             ImGui::BeginDisabled();
        }
        
        if (ImGui::Button(pauseLabel, ImVec2(60, 0))) { 
             onPause();
        }
        
        if (state == EditorState::Edit) {
             ImGui::EndDisabled();
        }

        // Compile Scripts button — right side of toolbar
        float compileButtonWidth = 110.0f;
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - compileButtonWidth);

        bool compileDisabled = (state != EditorState::Edit) || isCompiling;
        if (compileDisabled) ImGui::BeginDisabled();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.7f, 1.0f));
        if (ImGui::Button(isCompiling ? "Compiling..." : "Compile Scripts", ImVec2(compileButtonWidth, 0))) {
            if (onCompileScripts) {
                isCompiling = true;
                onCompileScripts();
                isCompiling = false;
            }
        }
        ImGui::PopStyleColor();

        if (compileDisabled) ImGui::EndDisabled();

        ImGui::End();
        ImGui::PopStyleVar(2);
    }
}
