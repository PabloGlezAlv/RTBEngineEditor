#include "NavigationDebugPanel.h"

#include "../EditorDockingUtils.h"
#include "../EditorWindowPrefs.h"

#include <imgui.h>

#include <RTBEngine/Scene/NavGridComponent.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/GameObject.h>

#include <functional>

namespace RTBEditor {
    namespace {

        RTBEngine::ECS::NavGridComponent* FindFirstNavGrid(RTBEngine::ECS::Scene* scene)
        {
            if (!scene) {
                return nullptr;
            }

            RTBEngine::ECS::NavGridComponent* found = nullptr;
            std::function<void(RTBEngine::ECS::GameObject*)> visit =
                [&](RTBEngine::ECS::GameObject* gameObject) {
                    if (!gameObject || found) {
                        return;
                    }

                    if (auto* navGrid = gameObject->GetComponent<RTBEngine::ECS::NavGridComponent>()) {
                        found = navGrid;
                        return;
                    }

                    for (RTBEngine::ECS::GameObject* child : gameObject->GetChildren()) {
                        visit(child);
                    }
                };

            for (const auto& gameObject : scene->GetGameObjects()) {
                if (gameObject) {
                    visit(gameObject.get());
                }
            }

            return found;
        }

        bool NavDebugSettingsChanged(const NavDebugSettings& previous,
                                     const NavDebugSettings& current)
        {
            return previous.enabled != current.enabled ||
                   previous.showBounds != current.showBounds ||
                   previous.showWalkableCells != current.showWalkableCells ||
                   previous.showBlockedCells != current.showBlockedCells ||
                   previous.showAgentPaths != current.showAgentPaths ||
                   previous.gridCellStep != current.gridCellStep ||
                   previous.yOffset != current.yOffset;
        }

    }

    void NavigationDebugPanel::OnUIRender(EditorContext& context)
    {
        if (!context.optionalWindows.navigationDebug) {
            return;
        }

        PrepareOptionalWindowDocking("Navigation Debug");

        if (!ImGui::Begin("Navigation Debug", &context.optionalWindows.navigationDebug)) {
            ImGui::End();
            return;
        }

        NavDebugSettings& settings = context.navDebug;

        ImGui::Checkbox("Enable debug overlay", &settings.enabled);
        ImGui::Separator();

        ImGui::BeginDisabled(!settings.enabled);
        ImGui::Checkbox("Show grid bounds", &settings.showBounds);
        ImGui::Checkbox("Show walkable cells", &settings.showWalkableCells);
        ImGui::Checkbox("Show blocked cells", &settings.showBlockedCells);
        ImGui::Checkbox("Show agent paths", &settings.showAgentPaths);

        ImGui::SetNextItemWidth(120.0f);
        ImGui::SliderInt("Grid step", &settings.gridCellStep, 0, 4, settings.gridCellStep == 0 ? "Auto" : "%d");
        if (settings.gridCellStep < 0) {
            settings.gridCellStep = 0;
        }

        ImGui::SetNextItemWidth(180.0f);
        ImGui::SliderFloat("Y offset", &settings.yOffset, 0.0f, 1.0f, "%.2f");
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::TextUnformatted("Scene status");

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        RTBEngine::ECS::NavGridComponent* navGrid = FindFirstNavGrid(scene);
        if (!navGrid) {
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.3f, 1.0f), "No NavGridComponent in scene.");
        } else if (!navGrid->IsBaked()) {
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.3f, 1.0f), "Nav grid is not baked.");
        } else {
            ImGui::Text("Baked grid: %dx%d cells",
                        navGrid->GetGrid().GetWidth(),
                        navGrid->GetGrid().GetHeight());
            ImGui::Text("Walkable cells: %d", navGrid->GetWalkableCellCount());
        }

        if (!hasPreviousNavDebug) {
            previousNavDebug = settings;
            hasPreviousNavDebug = true;
        } else if (NavDebugSettingsChanged(previousNavDebug, settings)) {
            EditorWindowPrefs::SaveFrom(context);
            previousNavDebug = settings;
        }

        ImGui::End();
    }

}
