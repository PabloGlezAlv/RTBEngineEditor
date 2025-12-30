#include "ConsolePanel.h"
#include <imgui.h>
#include <map>

namespace RTBEditor {

    ConsolePanel::ConsolePanel() {
        // We can access logs directly from the singleton
    }

    ConsolePanel::~ConsolePanel() {}

    void ConsolePanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Console");

        // Toolbar
        if (ImGui::Button("Clear")) {
            RTBEngine::Core::Logger::GetInstance().Clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        
        // Filter Buttons
        ImGui::PushStyleColor(ImGuiCol_Button, filterInfo ? ImVec4(0.2f, 0.7f, 0.2f, 0.7f) : ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        if (ImGui::Button("Info")) filterInfo = !filterInfo;
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, filterWarning ? ImVec4(0.7f, 0.7f, 0.2f, 0.7f) : ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        if (ImGui::Button("Warning")) filterWarning = !filterWarning;
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, filterError ? ImVec4(0.7f, 0.2f, 0.2f, 0.7f) : ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        if (ImGui::Button("Error")) filterError = !filterError;
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##Search", "Search...", searchFilter, IM_ARRAYSIZE(searchFilter));

        ImGui::Separator();

        // Log List
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto& logs = RTBEngine::Core::Logger::GetInstance().GetLogs();
        
        for (const auto& log : logs) {
            // Apply Filters
            if (log.level == RTBEngine::Core::LogLevel::Info && !filterInfo) continue;
            if (log.level == RTBEngine::Core::LogLevel::Warning && !filterWarning) continue;
            if (log.level == RTBEngine::Core::LogLevel::Error && !filterError) continue;
            
            // Search Filter
            if (searchFilter[0] != '\0') {
                if (log.message.find(searchFilter) == std::string::npos) continue;
            }

            // Colors based on level
            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            const char* levelIcon = "[INFO]";
            
            switch (log.level) {
                case RTBEngine::Core::LogLevel::Info:
                    color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    levelIcon = "(i)";
                    break;
                case RTBEngine::Core::LogLevel::Warning:
                    color = ImVec4(1.0f, 0.9f, 0.0f, 1.0f);
                    levelIcon = "/!\\";
                    break;
                case RTBEngine::Core::LogLevel::Error:
                    color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                    levelIcon = "[X]";
                    break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("[%s] %s %s", log.timestamp.c_str(), levelIcon, log.message.c_str());
            ImGui::PopStyleColor();
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();

        ImGui::End();
    }

}
