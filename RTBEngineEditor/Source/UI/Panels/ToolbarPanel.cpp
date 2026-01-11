#include "ToolbarPanel.h"
#include <imgui.h>

namespace RTBEditor {

    ToolbarPanel::ToolbarPanel(std::function<void()> onPlay, std::function<void()> onPause, std::function<void()> onStop, std::function<EditorState()> getState)
        : onPlay(onPlay), onPause(onPause), onStop(onStop), getState(getState) {}

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

        ImGui::End();
        ImGui::PopStyleVar(2);
    }
}
