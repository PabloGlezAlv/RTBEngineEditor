#include "ProjectSettingsPanel.h"

#include "../EditorDockingUtils.h"
#include "../../Project/Project.h"

#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Rendering/RHI/GraphicsAPI.h>
#include <RTBEngine/Rendering/Lighting/LightingProjectSettings.h>
#include <RTBEngine/Rendering/GI/DDGISystem.h>

#include <imgui.h>
#include <algorithm>

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

        auto& lighting = RTBEngine::Rendering::LightingProjectSettings::Get();
        ambientColor = lighting.ambientColor;
        ambientIntensity = lighting.ambientIntensity;
        ddgiEnabled = lighting.IsDDGIEnabled();
        ddgiIntensity = lighting.ddgiIntensity;
        shadowsEnabled = lighting.shadowsEnabled;
        shadowMapResolution = lighting.GetClampedShadowMapResolution();
        ddgiOrigin = lighting.ddgiOrigin;
        ddgiExtent = lighting.ddgiExtent;
        ddgiGridX = lighting.ddgiGridX;
        ddgiGridY = lighting.ddgiGridY;
        ddgiGridZ = lighting.ddgiGridZ;
        ddgiHysteresis = lighting.ddgiHysteresis;
        ddgiNormalBias = lighting.ddgiNormalBias;
        ddgiViewBias = lighting.ddgiViewBias;
        ddgiProbeRadius = lighting.ddgiProbeRadius;

        syncedOnce = true;
    }

    bool ProjectSettingsPanel::SaveToActiveProject(EditorContext& context)
    {
        Project* project = Project::GetActiveProject();
        if (!project) {
            lastMessage = "No active project loaded.";
            lastSaveSucceeded = false;
            return false;
        }

        project->SetGraphicsAPI(selectedGraphicsAPI);

        auto& lighting = RTBEngine::Rendering::LightingProjectSettings::Get();
        lighting.ambientColor = ambientColor;
        lighting.ambientIntensity = std::max(0.0f, ambientIntensity);
        lighting.SetDDGIEnabled(ddgiEnabled);
        lighting.ddgiIntensity = std::max(0.0f, ddgiIntensity);
        lighting.shadowsEnabled = shadowsEnabled;
        lighting.shadowMapResolution =
            RTBEngine::Rendering::LightingProjectSettings::ClampShadowMapResolution(shadowMapResolution);
        lighting.ddgiOrigin = ddgiOrigin;
        lighting.ddgiExtent = ddgiExtent;
        lighting.ddgiGridX = std::clamp(ddgiGridX, 1, 32);
        lighting.ddgiGridY = std::clamp(ddgiGridY, 1, 32);
        lighting.ddgiGridZ = std::clamp(ddgiGridZ, 1, 32);
        lighting.ddgiHysteresis = std::clamp(ddgiHysteresis, 0.0f, 0.85f);
        lighting.ddgiNormalBias = std::max(0.0f, ddgiNormalBias);
        lighting.ddgiViewBias = std::max(0.0f, ddgiViewBias);
        lighting.ddgiProbeRadius = std::max(0.1f, ddgiProbeRadius);

        const std::filesystem::path lightingPath =
            project->GetProjectDirectory()
            / RTBEngine::Rendering::LightingProjectSettings::GetDefaultSettingsFileName();
        const bool lightingSaved = lighting.SaveToFile(lightingPath);
        RTBEngine::Rendering::GI::DDGISystem::GetInstance().SyncFromProjectSettings();

        lastSaveSucceeded = project->Save() && lightingSaved;
        lastMessage = lastSaveSucceeded
            ? "Project and lighting settings saved."
            : "Could not save project or lighting settings.";
        (void)context;
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

        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
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
        }

        if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::TextDisabled("Default ambient used when DDGI is off.");
            ImGui::ColorEdit3("Ambient Color", &ambientColor.x, ImGuiColorEditFlags_Float);
            ImGui::DragFloat("Ambient Intensity", &ambientIntensity, 0.005f, 0.0f, 2.0f, "%.3f");

            ImGui::Spacing();
            if (ImGui::TreeNodeEx("Shadows", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Enable Shadows", &shadowsEnabled);
                ImGui::TextDisabled("Directional light shadow maps (OpenGL and Vulkan).");
                ImGui::BeginDisabled(!shadowsEnabled);
                const char* resolutionLabels[] = { "512", "1024", "2048", "4096", "8192", "16384" };
                const int resolutionValues[] = { 512, 1024, 2048, 4096, 8192, 16384 };
                int resolutionIndex = 2;
                for (int i = 0; i < 6; ++i) {
                    if (resolutionValues[i] == shadowMapResolution) {
                        resolutionIndex = i;
                        break;
                    }
                }
                if (ImGui::Combo("Shadow Map Resolution", &resolutionIndex, resolutionLabels, 6)) {
                    shadowMapResolution = resolutionValues[resolutionIndex];
                }
                ImGui::EndDisabled();
                ImGui::TreePop();
            }

            ImGui::Spacing();
            if (ImGui::TreeNodeEx("DDGI", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Enable DDGI", &ddgiEnabled);
                ImGui::BeginDisabled(!ddgiEnabled);
                ImGui::DragFloat("Indirect Intensity", &ddgiIntensity, 0.005f, 0.0f, 2.0f, "%.3f");
                ImGui::TextDisabled("Scales bounce light only (not direct lights).");
                ImGui::DragFloat3("Volume Origin", &ddgiOrigin.x, 0.05f);
                ImGui::DragFloat3("Volume Extent", &ddgiExtent.x, 0.05f, 0.1f, 512.0f);
                ImGui::DragInt("Grid X", &ddgiGridX, 1.0f, 1, 32);
                ImGui::DragInt("Grid Y", &ddgiGridY, 1.0f, 1, 32);
                ImGui::DragInt("Grid Z", &ddgiGridZ, 1.0f, 1, 32);
                ImGui::DragFloat("Hysteresis", &ddgiHysteresis, 0.01f, 0.0f, 0.85f, "%.2f");
                ImGui::TextDisabled("Keep below ~0.7 so probes can update (0.99 freezes GI).");
                ImGui::DragFloat("Normal Bias", &ddgiNormalBias, 0.01f, 0.0f, 2.0f, "%.2f");
                ImGui::DragFloat("View Bias", &ddgiViewBias, 0.01f, 0.0f, 2.0f, "%.2f");
                ImGui::DragFloat("Probe Radius", &ddgiProbeRadius, 0.05f, 0.1f, 64.0f, "%.2f");
                ImGui::EndDisabled();

                if (ImGui::TreeNode("Debug Visualization")) {
                    ImGui::Checkbox("Show DDGI Debug", &context.ddgiDebug.enabled);
                    ImGui::BeginDisabled(!context.ddgiDebug.enabled);
                    ImGui::Checkbox("Volume Bounds", &context.ddgiDebug.showVolumeBounds);
                    ImGui::Checkbox("Probe Grid", &context.ddgiDebug.showProbeGrid);
                    ImGui::DragFloat("Probe Draw Size", &context.ddgiDebug.probeDrawRadius, 0.01f, 0.02f, 1.0f);
                    ImGui::EndDisabled();
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Reload")) {
            SyncFromActiveProject();
            lastSaveSucceeded = true;
            lastMessage = "Settings reloaded from project.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Apply")) {
            auto& lighting = RTBEngine::Rendering::LightingProjectSettings::Get();
            lighting.ambientColor = ambientColor;
            lighting.ambientIntensity = std::max(0.0f, ambientIntensity);
            lighting.SetDDGIEnabled(ddgiEnabled);
            lighting.ddgiIntensity = std::max(0.0f, ddgiIntensity);
            lighting.shadowsEnabled = shadowsEnabled;
            lighting.shadowMapResolution =
                RTBEngine::Rendering::LightingProjectSettings::ClampShadowMapResolution(shadowMapResolution);
            lighting.ddgiOrigin = ddgiOrigin;
            lighting.ddgiExtent = ddgiExtent;
            lighting.ddgiGridX = std::clamp(ddgiGridX, 1, 32);
            lighting.ddgiGridY = std::clamp(ddgiGridY, 1, 32);
            lighting.ddgiGridZ = std::clamp(ddgiGridZ, 1, 32);
            lighting.ddgiHysteresis = std::clamp(ddgiHysteresis, 0.0f, 0.85f);
            lighting.ddgiNormalBias = std::max(0.0f, ddgiNormalBias);
            lighting.ddgiViewBias = std::max(0.0f, ddgiViewBias);
            lighting.ddgiProbeRadius = std::max(0.1f, ddgiProbeRadius);
            RTBEngine::Rendering::GI::DDGISystem::GetInstance().SyncFromProjectSettings();
            lastSaveSucceeded = true;
            lastMessage = "Lighting settings applied (not saved to disk).";
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            SaveToActiveProject(context);
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
