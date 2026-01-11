#include "MainMenuBar.h"
#include <imgui.h>

namespace RTBEditor {

    MainMenuBar::MainMenuBar() {}
    MainMenuBar::~MainMenuBar() {}

    void MainMenuBar::OnUIRender() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
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
                // Placeholder for future window toggles
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

}
