#include "MainMenuBar.h"
#include <imgui.h>
#include <string>

namespace RTBEditor {

    MainMenuBar::MainMenuBar() {}
    MainMenuBar::~MainMenuBar() {}

    void MainMenuBar::OnUIRender(EditorContext& context) {
        ImGuiIO& io = ImGui::GetIO();

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
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

            if (ImGui::BeginMenu("Edit")) {
                const bool canCopy = !canCopyProvider || canCopyProvider();
                const bool canPaste = !canPasteProvider || canPasteProvider();

                if (ImGui::MenuItem("Copy", "Ctrl+C", false, canCopy)) {
                    if (copyCallback) copyCallback();
                }

                if (ImGui::MenuItem("Paste", "Ctrl+V", false, canPaste)) {
                    if (pasteCallback) pasteCallback();
                }

                if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, canCopy)) {
                    if (duplicateCallback) duplicateCallback();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                OptionalWindowState previousWindows = context.optionalWindows;

                ImGui::MenuItem("Online", nullptr, &context.optionalWindows.online);
                ImGui::MenuItem("Physics Layers", nullptr, &context.optionalWindows.physicsLayers);
                ImGui::MenuItem("Navigation Debug", nullptr, &context.optionalWindows.navigationDebug);

                if (context.optionalWindows.online != previousWindows.online ||
                    context.optionalWindows.physicsLayers != previousWindows.physicsLayers ||
                    context.optionalWindows.navigationDebug != previousWindows.navigationDebug) {
                    if (persistWindowPrefsCallback) {
                        persistWindowPrefsCallback();
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

}
