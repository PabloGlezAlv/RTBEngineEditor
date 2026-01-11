#include "GameViewPanel.h"
#include <imgui.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/UI/CanvasSystem.h>

namespace RTBEditor {

    GameViewPanel::GameViewPanel() {
        framebuffer = std::make_unique<RTBEngine::Rendering::Framebuffer>();
        framebuffer->CreateWithColorAndDepth(1280, 720);
    }

    GameViewPanel::~GameViewPanel() {}

    void GameViewPanel::OnUIRender(EditorContext& context) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game", &isVisible);

        // Check resize
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        uint32_t newWidth = (uint32_t)availableSize.x;
        uint32_t newHeight = (uint32_t)availableSize.y;

        if (newWidth > 0 && newHeight > 0) {
            if (viewportWidth != newWidth || viewportHeight != newHeight) {
                viewportWidth = newWidth;
                viewportHeight = newHeight;
                framebuffer->Resize(viewportWidth, viewportHeight);
            }
        }

        // Get the position where content starts (for UI offset calculation)
        ImVec2 contentPos = ImGui::GetCursorScreenPos();

        // Draw texture
        GLuint textureID = framebuffer->GetColorTextureID();
        if (textureID != 0) {
            ImGui::Image(
                (void*)(intptr_t)textureID,
                ImVec2((float)viewportWidth, (float)viewportHeight),
                ImVec2(0, 1), // Flip Y
                ImVec2(1, 0)
            );
        }

        // Render scene UI on top of the game view
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (scene && viewportWidth > 0 && viewportHeight > 0) {
            // Update CanvasSystem to collect active canvases
            RTBEngine::UI::CanvasSystem::GetInstance().Update(scene);

            // Get the window's DrawList and render canvases with offset
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            RTBEngine::Math::Vector2 screenSize((float)viewportWidth, (float)viewportHeight);
            RTBEngine::Math::Vector2 offset(contentPos.x, contentPos.y);

            RTBEngine::UI::CanvasSystem::GetInstance().RenderCanvasesToDrawList(drawList, screenSize, offset);
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

}
