#include "ToolbarPanel.h"
#include <imgui.h>

namespace RTBEditor {

    ToolbarPanel::ToolbarPanel(std::function<void()> onPlay,
                               std::function<void()> onPause,
                               std::function<void()> onStop,
                               std::function<EditorState()> getState,
                               std::function<bool()> isCompilingScriptsProvider,
                               std::function<void()> onCompileScripts)
        : onPlay(onPlay)
        , onPause(onPause)
        , onStop(onStop)
        , getState(getState)
        , isCompilingScriptsProvider(isCompilingScriptsProvider)
        , onCompileScripts(onCompileScripts) {}

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

        bool isCompiling = isCompilingScriptsProvider ? isCompilingScriptsProvider() : false;
        bool compileDisabled = (state != EditorState::Edit) || isCompiling;
        if (compileDisabled) ImGui::BeginDisabled();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.7f, 1.0f));
        if (ImGui::Button(isCompiling ? "Compiling..." : "Compile Scripts", ImVec2(compileButtonWidth, 0))) {
            if (onCompileScripts) {
                onCompileScripts();
            }
        }
        ImGui::PopStyleColor();

        if (compileDisabled) ImGui::EndDisabled();

        // Centered modal overlay while scripts are compiling
        if (isCompiling) {
            ImGui::OpenPopup("ScriptCompileProgress");
        }

        if (ImGui::BeginPopupModal("ScriptCompileProgress", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            // Center the modal in the main viewport
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 center = viewport->GetCenter();
            ImGui::SetWindowPos(ImVec2(center.x - ImGui::GetWindowSize().x * 0.5f,
                                       center.y - ImGui::GetWindowSize().y * 0.5f));

            ImGui::Text("Recompilando scripts...");
            ImGui::Spacing();

            // Indeterminate progress bar using time-based animation
            float t = static_cast<float>(fmod(ImGui::GetTime(), 1.0));
            ImGui::ProgressBar(t, ImVec2(250.0f, 0.0f));

            if (!isCompiling) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
    }
}
