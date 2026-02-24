#include "MainMenuBar.h"
#include <imgui.h>
#include <string>

namespace RTBEditor {

    MainMenuBar::MainMenuBar() {}
    MainMenuBar::~MainMenuBar() {}

    void MainMenuBar::OnUIRender() {
        // Global Keyboard Shortcuts
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            if (sceneDirty && saveSceneCallback) {
                saveSceneCallback();
            }
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                std::string saveLabel = sceneDirty ? "Save Scene* (Ctrl+S)" : "Save Scene (Ctrl+S)";

                if (ImGui::MenuItem(saveLabel.c_str(), nullptr, false, sceneDirty)) {
                    if (saveSceneCallback) saveSceneCallback();
                }

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                    if (saveSceneAsCallback) saveSceneAsCallback();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Build...", "Ctrl+B")) {
                    if (buildCallback) buildCallback();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    if (exitCallback) exitCallback();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

}