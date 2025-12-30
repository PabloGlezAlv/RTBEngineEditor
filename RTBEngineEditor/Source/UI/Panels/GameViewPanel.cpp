#include "GameViewPanel.h"
#include <imgui.h>

namespace RTBEditor {

    GameViewPanel::GameViewPanel() {
        m_Framebuffer = std::make_unique<RTBEngine::Rendering::Framebuffer>();
        m_Framebuffer->CreateWithColorAndDepth(1280, 720);
    }

    GameViewPanel::~GameViewPanel() {}

    void GameViewPanel::OnUIRender(EditorContext& context) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game", &m_IsVisible);

        // Check resize
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        uint32_t newWidth = (uint32_t)availableSize.x;
        uint32_t newHeight = (uint32_t)availableSize.y;

        if (newWidth > 0 && newHeight > 0) {
            if (m_ViewportWidth != newWidth || m_ViewportHeight != newHeight) {
                m_ViewportWidth = newWidth;
                m_ViewportHeight = newHeight;
                m_Framebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
            }
        }

        // Draw texture
        GLuint textureID = m_Framebuffer->GetColorTextureID();
        if (textureID != 0) {
            ImGui::Image(
                (void*)(intptr_t)textureID,
                ImVec2((float)m_ViewportWidth, (float)m_ViewportHeight),
                ImVec2(0, 1), // Flip Y
                ImVec2(1, 0)
            );
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

}
