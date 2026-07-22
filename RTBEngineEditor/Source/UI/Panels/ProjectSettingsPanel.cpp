#include "ProjectSettingsPanel.h"

#include "../EditorDockingUtils.h"
#include "../../Project/Project.h"

#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Rendering/RHI/GraphicsAPI.h>

#include <imgui.h>

namespace RTBEditor {

    ProjectSettingsPanel::ProjectSettingsPanel() = default;

    void ProjectSettingsPanel::SyncFromActiveProject()
    {
        if (Project* project = Project::GetActiveProject()) {
            selectedGraphicsAPI = project->GetGraphicsAPI();
        }

        if (RTBEngine::Rendering::RHI::RenderDevice::HasDevice()) {
            activeRuntimeAPI = RTBEngine::Rendering::RHI::RenderDevice::Get().GetAPI();
        }
        syncedOnce = true;
    }

    bool ProjectSettingsPanel::SaveToActiveProject()
    {
        Project* project = Project::GetActiveProject();
        if (!project) {
            lastMessage = "No active project loaded.";
            lastSaveSucceeded = false;
            return false;
        }

        project->SetGraphicsAPI(selectedGraphicsAPI);
        lastSaveSucceeded = project->Save();
        lastMessage = lastSaveSucceeded
            ? "Project settings saved."
            : "Could not save project file.";
        return lastSaveSucceeded;
    }

    void ProjectSettingsPanel::OnUIRender(EditorContext& context)
    {
        if (!context.optionalWindows.projectSettings) {
            return;
        }

        if (!syncedOnce) {
            SyncFromActiveProject();
        }

        PrepareOptionalWindowDocking("Project Settings");

        if (!ImGui::Begin("Project Settings", &context.optionalWindows.projectSettings)) {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted("Rendering");
        ImGui::Separator();

        const char* apiLabels[] = { "OpenGL", "Vulkan" };
        int apiIndex = (selectedGraphicsAPI == RTBEngine::Rendering::RHI::GraphicsAPI::Vulkan) ? 1 : 0;
        if (ImGui::Combo("Graphics API", &apiIndex, apiLabels, 2)) {
            selectedGraphicsAPI = (apiIndex == 1)
                ? RTBEngine::Rendering::RHI::GraphicsAPI::Vulkan
                : RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
        }

        ImGui::TextDisabled("Active this session: %s",
            RTBEngine::Rendering::RHI::GraphicsAPIToString(activeRuntimeAPI));

        if (selectedGraphicsAPI != activeRuntimeAPI) {
            ImGui::Spacing();
            ImGui::TextWrapped(
                "Changing the Graphics API requires restarting the editor. "
                "Save, then close and reopen RTBEngineEditor.");
        }

        ImGui::Spacing();
        if (ImGui::Button("Reload")) {
            SyncFromActiveProject();
            lastSaveSucceeded = true;
            lastMessage = "Settings reloaded from project.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            SaveToActiveProject();
        }

        if (!lastMessage.empty()) {
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text,
                lastSaveSucceeded
                    ? ImVec4(0.45f, 0.85f, 0.45f, 1.0f)
                    : ImVec4(1.0f, 0.45f, 0.45f, 1.0f));
            ImGui::TextUnformatted(lastMessage.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::End();
    }

}
