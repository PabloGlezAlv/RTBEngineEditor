#include "PhysicsLayersPanel.h"

#include "../EditorDockingUtils.h"

#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <cstring>

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Physics/PhysicsLayerSettings.h>

#include "../../Project/Project.h"

namespace RTBEditor {

    PhysicsLayersPanel::PhysicsLayersPanel()
    {
        LoadFromProject();
    }

    PhysicsLayersPanel::~PhysicsLayersPanel() = default;

    void PhysicsLayersPanel::LoadFromProject()
    {
        RTBEngine::Physics::PhysicsLayerSettings& settings =
            RTBEngine::Physics::PhysicsLayerSettings::Get();

        if (Project* project = Project::GetActiveProject()) {
            const std::filesystem::path path =
                project->GetProjectDirectory() /
                RTBEngine::Physics::PhysicsLayerSettings::GetDefaultSettingsFileName();
            if (!settings.LoadFromFile(path)) {
                settings.ResetToDefaults();
            }
        }

        layerCount = settings.GetLayerCount();
        for (int i = 0; i < RTBEngine::Physics::kMaxPhysicsLayers; ++i) {
            const std::string& name = settings.GetLayerName(i);
            std::snprintf(layerNameBuffers[i], sizeof(layerNameBuffers[i]), "%s", name.c_str());

            for (int j = 0; j < RTBEngine::Physics::kMaxPhysicsLayers; ++j) {
                collisionMatrix[i][j] = settings.GetLayerCollision(i, j);
            }
        }
    }

    bool PhysicsLayersPanel::SaveToProject()
    {
        Project* project = Project::GetActiveProject();
        if (!project) {
            lastSaveSucceeded = false;
            lastMessage = "No active project.";
            return false;
        }

        RTBEngine::Physics::PhysicsLayerSettings& settings =
            RTBEngine::Physics::PhysicsLayerSettings::Get();
        settings.SetLayerCount(layerCount);

        for (int i = 0; i < layerCount; ++i) {
            settings.SetLayerName(i, layerNameBuffers[i]);
        }

        for (int row = 0; row < layerCount; ++row) {
            for (int column = 0; column < layerCount; ++column) {
                settings.SetLayerCollision(row, column, collisionMatrix[row][column]);
            }
        }

        const std::filesystem::path path =
            project->GetProjectDirectory() /
            RTBEngine::Physics::PhysicsLayerSettings::GetDefaultSettingsFileName();
        lastSaveSucceeded = settings.SaveToFile(path);
        lastMessage = lastSaveSucceeded
            ? "Physics layers saved."
            : "Failed to save physics_layers.ini.";

        RefreshActivePhysicsFilters();
        return lastSaveSucceeded;
    }

    void PhysicsLayersPanel::RefreshActivePhysicsFilters()
    {
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) {
            return;
        }

        const RTBEngine::Physics::PhysicsLayerSettings& settings =
            RTBEngine::Physics::PhysicsLayerSettings::Get();

        for (const auto& gameObject : scene->GetGameObjects()) {
            if (gameObject) {
                settings.ApplyToGameObject(gameObject.get());
            }
        }
    }

    void PhysicsLayersPanel::DrawLayerNames()
    {
        ImGui::Text("Layers");
        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::InputInt("Count", &layerCount, 1, 1)) {
            layerCount = std::clamp(layerCount, 1, RTBEngine::Physics::kMaxPhysicsLayers);
        }

        const float rowHeight = ImGui::GetFrameHeightWithSpacing();
        const float listHeight = std::min(rowHeight * static_cast<float>(layerCount) + 8.0f, 220.0f);

        ImGui::BeginChild("LayerNameList", ImVec2(0.0f, listHeight), true);
        for (int i = 0; i < layerCount; ++i) {
            ImGui::PushID(i);
            ImGui::Text("%2d", i);
            ImGui::SameLine(28.0f);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputText("##LayerName", layerNameBuffers[i], sizeof(layerNameBuffers[i]));
            ImGui::PopID();
        }
        ImGui::EndChild();
    }

    bool PhysicsLayersPanel::IsLayerVisible(int layerIndex) const
    {
        if (layerFilter[0] == '\0') {
            return true;
        }

        const char* name = layerNameBuffers[layerIndex];
        if (!name || name[0] == '\0') {
            return false;
        }

        std::string haystack(name);
        std::string needle(layerFilter);
        std::transform(haystack.begin(), haystack.end(), haystack.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        std::transform(needle.begin(), needle.end(), needle.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        return haystack.find(needle) != std::string::npos;
    }

    void PhysicsLayersPanel::BuildVisibleLayerIndices(std::vector<int>& outIndices) const
    {
        outIndices.clear();
        outIndices.reserve(static_cast<size_t>(layerCount));

        for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex) {
            if (IsLayerVisible(layerIndex)) {
                outIndices.push_back(layerIndex);
            }
        }
    }

    const char* PhysicsLayersPanel::AbbreviateLayerLabel(
        const char* fullName,
        char* outBuffer,
        size_t bufferSize)
    {
        if (!fullName || !outBuffer || bufferSize == 0) {
            return "";
        }

        const size_t maxChars = bufferSize > 1 ? bufferSize - 1 : 0;
        const size_t nameLength = std::strlen(fullName);
        if (nameLength <= maxChars) {
            std::snprintf(outBuffer, bufferSize, "%s", fullName);
            return outBuffer;
        }

        if (maxChars <= 1) {
            outBuffer[0] = '\0';
            return outBuffer;
        }

        std::snprintf(outBuffer, bufferSize, "%.*s~", static_cast<int>(maxChars - 1), fullName);
        return outBuffer;
    }

    void PhysicsLayersPanel::DrawCollisionMatrixToolbar(int visibleLayerCount)
    {
        ImGui::Separator();
        ImGui::Text("Collision matrix");

        ImGui::SetNextItemWidth(140.0f);
        ImGui::SliderFloat("Cell size", &matrixCellSize, 16.0f, 36.0f, "%.0f px");

        ImGui::SameLine();
        ImGui::Checkbox("Triangle view", &compactTriangleView);
        ImGui::SameLine();
        ImGui::Checkbox("Show indices", &showLayerIndices);

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##LayerFilter", "Filter layers by name...", layerFilter, sizeof(layerFilter));

        if (visibleLayerCount == 0) {
            ImGui::TextDisabled("No layers match the filter.");
            return;
        }

        if (visibleLayerCount < layerCount) {
            ImGui::TextDisabled("Showing %d of %d layers.", visibleLayerCount, layerCount);
        }

        if (ImGui::Button("Enable all")) {
            for (int row = 0; row < layerCount; ++row) {
                for (int column = 0; column < layerCount; ++column) {
                    collisionMatrix[row][column] = true;
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Disable all pairs")) {
            for (int row = 0; row < layerCount; ++row) {
                for (int column = 0; column < layerCount; ++column) {
                    collisionMatrix[row][column] = row == column;
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Invert filtered")) {
            std::vector<int> visible;
            BuildVisibleLayerIndices(visible);
            for (int row : visible) {
                for (int column : visible) {
                    collisionMatrix[row][column] = !collisionMatrix[row][column];
                    collisionMatrix[column][row] = collisionMatrix[row][column];
                }
            }
        }
    }

    void PhysicsLayersPanel::DrawCollisionMatrix()
    {
        std::vector<int> visibleLayers;
        BuildVisibleLayerIndices(visibleLayers);
        const int visibleCount = static_cast<int>(visibleLayers.size());

        DrawCollisionMatrixToolbar(visibleCount);
        if (visibleCount == 0) {
            return;
        }

        const float labelWidth = 112.0f;
        const float headerHeight = matrixCellSize + 8.0f;
        const float rowHeight = matrixCellSize + 4.0f;
        const int columnCount = visibleCount + 1;
        const float estimatedWidth = labelWidth + static_cast<float>(visibleCount) * matrixCellSize + 24.0f;
        const float estimatedHeight = headerHeight + static_cast<float>(visibleCount) * rowHeight + 12.0f;
        const ImVec2 maxChildSize = ImGui::GetContentRegionAvail();
        const float childHeight = std::min(std::max(estimatedHeight, 120.0f), std::max(180.0f, maxChildSize.y * 0.62f));
        const float childWidth = std::max(estimatedWidth, maxChildSize.x);

        ImGui::BeginChild(
            "CollisionMatrixScroll",
            ImVec2(0.0f, childHeight),
            true,
            ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2.0f, 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 2.0f));

        const ImGuiTableFlags tableFlags =
            ImGuiTableFlags_BordersInner |
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit |
            ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("PhysicsLayerMatrix", columnCount, tableFlags, ImVec2(childWidth, childHeight))) {
            ImGui::TableSetupScrollFreeze(1, 1);

            ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, labelWidth);
            for (int visibleColumn = 0; visibleColumn < visibleCount; ++visibleColumn) {
                const int layerIndex = visibleLayers[static_cast<size_t>(visibleColumn)];
                char headerBuffer[16] = {};
                if (showLayerIndices) {
                    std::snprintf(headerBuffer, sizeof(headerBuffer), "%d", layerIndex);
                } else {
                    AbbreviateLayerLabel(layerNameBuffers[layerIndex], headerBuffer, sizeof(headerBuffer));
                }

                ImGui::TableSetupColumn(
                    headerBuffer,
                    ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize,
                    matrixCellSize);
            }

            ImGui::TableHeadersRow();

            char abbrevBuffer[16] = {};
            for (int visibleRow = 0; visibleRow < visibleCount; ++visibleRow) {
                const int rowLayer = visibleLayers[static_cast<size_t>(visibleRow)];
                ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);

                ImGui::TableSetColumnIndex(0);
                if (showLayerIndices) {
                    std::snprintf(abbrevBuffer, sizeof(abbrevBuffer), "%d", rowLayer);
                } else {
                    AbbreviateLayerLabel(layerNameBuffers[rowLayer], abbrevBuffer, sizeof(abbrevBuffer));
                }

                ImGui::PushID(rowLayer);
                ImGui::Selectable(abbrevBuffer, false, ImGuiSelectableFlags_None, ImVec2(labelWidth - 6.0f, matrixCellSize));
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", layerNameBuffers[rowLayer]);
                }
                ImGui::PopID();

                for (int visibleColumn = 0; visibleColumn < visibleCount; ++visibleColumn) {
                    const int columnLayer = visibleLayers[static_cast<size_t>(visibleColumn)];

                    ImGui::TableSetColumnIndex(visibleColumn + 1);

                    const ImVec2 cellSize(matrixCellSize, matrixCellSize);
                    if (compactTriangleView && columnLayer > rowLayer) {
                        ImGui::Dummy(cellSize);
                        continue;
                    }

                    ImGui::PushID(rowLayer * RTBEngine::Physics::kMaxPhysicsLayers + columnLayer);

                    bool enabled = collisionMatrix[rowLayer][columnLayer];
                    const ImVec4 onColor(0.35f, 0.75f, 0.45f, 1.0f);
                    const ImVec4 offColor(0.22f, 0.22f, 0.24f, 1.0f);

                    ImGui::PushStyleColor(ImGuiCol_Button, enabled ? onColor : offColor);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, enabled ? ImVec4(0.45f, 0.85f, 0.55f, 1.0f) : ImVec4(0.30f, 0.30f, 0.34f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, enabled ? ImVec4(0.25f, 0.65f, 0.35f, 1.0f) : ImVec4(0.40f, 0.40f, 0.44f, 1.0f));

                    if (ImGui::Button("##cell", cellSize)) {
                        enabled = !enabled;
                        collisionMatrix[rowLayer][columnLayer] = enabled;
                        collisionMatrix[columnLayer][rowLayer] = enabled;
                    }

                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s  <->  %s\n%s",
                            layerNameBuffers[rowLayer],
                            layerNameBuffers[columnLayer],
                            enabled ? "Collide" : "Ignore");
                    }

                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                }
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(3);
        ImGui::EndChild();
    }

    void PhysicsLayersPanel::OnUIRender(EditorContext& context)
    {
        if (!context.optionalWindows.physicsLayers) {
            return;
        }

        PrepareOptionalWindowDocking("Physics Layers");

        if (!ImGui::Begin("Physics Layers", &context.optionalWindows.physicsLayers)) {
            ImGui::End();
            return;
        }

        DrawLayerNames();
        DrawCollisionMatrix();

        if (ImGui::Button("Reset to engine default")) {
            RTBEngine::Physics::PhysicsLayerSettings::Get().ResetToDefaults();
            layerCount = RTBEngine::Physics::PhysicsLayerSettings::Get().GetLayerCount();
            for (int i = 0; i < RTBEngine::Physics::kMaxPhysicsLayers; ++i) {
                const std::string& name =
                    RTBEngine::Physics::PhysicsLayerSettings::Get().GetLayerName(i);
                std::snprintf(layerNameBuffers[i], sizeof(layerNameBuffers[i]), "%s", name.c_str());
                for (int j = 0; j < RTBEngine::Physics::kMaxPhysicsLayers; ++j) {
                    collisionMatrix[i][j] =
                        RTBEngine::Physics::PhysicsLayerSettings::Get().GetLayerCollision(i, j);
                }
            }
            RefreshActivePhysicsFilters();
        }

        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            SaveToProject();
        }

        if (!lastMessage.empty()) {
            const ImVec4 color = lastSaveSucceeded
                ? ImVec4(0.4f, 0.9f, 0.5f, 1.0f)
                : ImVec4(1.0f, 0.45f, 0.45f, 1.0f);
            ImGui::TextColored(color, "%s", lastMessage.c_str());
        }

        ImGui::End();
    }

}
