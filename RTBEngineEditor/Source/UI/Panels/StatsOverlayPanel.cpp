#include "StatsOverlayPanel.h"
#include <imgui.h>

namespace RTBEditor {

    void StatsOverlayPanel::OnUIRender(EditorContext& context) {
        if (!context.showStatsOverlay) return;

        const float padding = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 workPos = viewport->WorkPos;
        ImVec2 workSize = viewport->WorkSize;
        ImVec2 windowPos = ImVec2(workPos.x + workSize.x - padding, workPos.y + padding);
        ImVec2 windowPivot = ImVec2(1.0f, 0.0f);

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
        ImGui::SetNextWindowBgAlpha(0.75f);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoFocusOnAppearing;

        if (ImGui::Begin("##StatsOverlay", nullptr, flags)) {
            const StatsData& s = context.stats;
            ImGui::Text("FPS          %.1f", s.fps);
            ImGui::Text("Frame Time   %.2f ms", s.frameTimeMs);
            ImGui::Separator();
            ImGui::Text("Draw Calls   %u", s.drawCalls);
            ImGui::Text("Triangles    %u", s.triangles);
            ImGui::Text("Culled       %u", s.culledObjects);
            ImGui::Separator();
            ImGui::Text("GameObjects  %u", s.gameObjects);
            ImGui::Text("Components   %u", s.components);
            ImGui::Separator();
            ImGui::Text("Phys Bodies  %u", s.physicsBodies);
            ImGui::Text("Audio Srcs   %u", s.audioSources);
        }
        ImGui::End();
    }

}
