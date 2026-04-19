#include "GameViewPanel.h"
#include <imgui.h>
#include <RTBEngine/Core/Window.h>
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
        isFocused = ImGui::IsWindowFocused();
        isHovered = ImGui::IsWindowHovered();

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

        UpdateMouseCapture(context);

        // Render scene UI on top of the game view
        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (scene && viewportWidth > 0 && viewportHeight > 0) {
            RTBEngine::Math::Vector2 screenSize((float)viewportWidth, (float)viewportHeight);
            RTBEngine::Math::Vector2 offset(contentPos.x, contentPos.y);

            auto& canvasSystem = RTBEngine::UI::CanvasSystem::GetInstance();

            // Update CanvasSystem to collect active canvases
            canvasSystem.Update(scene);
            canvasSystem.UpdateAllRectTransforms(screenSize);

            // Dispatch pointer events — only when mouse is inside the game viewport
            ImVec2 mousePos = ImGui::GetMousePos();
            bool mouseInViewport = mousePos.x >= contentPos.x && mousePos.x <= contentPos.x + (float)viewportWidth
                                && mousePos.y >= contentPos.y && mousePos.y <= contentPos.y + (float)viewportHeight;
            if (mouseInViewport && context.state == EditorState::Play && !IsGameOwningMouse(context)) {
                RTBEngine::Math::Vector2 localMouse(mousePos.x - contentPos.x, mousePos.y - contentPos.y);
                canvasSystem.ProcessInput(localMouse);
            }

            // Render UI elements into the ImGui draw list
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            canvasSystem.RenderToDrawList(drawList, screenSize, offset);

            // Draw interaction area outline for the selected UI object
            if (context.selectedGameObject) {
                auto rects = canvasSystem.GetRaycastRectsForGameObject(context.selectedGameObject);
                for (const auto& rect : rects) {
                    ImVec2 rMin(rect.x + offset.x, rect.y + offset.y);
                    ImVec2 rMax(rect.x + rect.z + offset.x, rect.y + rect.w + offset.y);
                    drawList->AddRect(rMin, rMax, IM_COL32(255, 0, 0, 220), 0.0f, 0, 1.5f);
                }
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GameViewPanel::ReleaseMouseCapture(EditorContext& context) {
        if (context.window) {
            context.window->SetMouseCaptured(false);
            context.window->SetCursorVisible(true);
        }
    }

    void GameViewPanel::UpdateMouseCapture(EditorContext& context) {
        if (!context.window) {
            return;
        }

        if (context.state != EditorState::Play || !isVisible) {
            ReleaseMouseCapture(context);
            return;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && IsGameOwningMouse(context)) {
            ReleaseMouseCapture(context);
        }
    }

    bool GameViewPanel::IsGameOwningMouse(const EditorContext& context) const {
        if (!context.window) {
            return false;
        }

        return context.window->IsMouseCaptured() || !context.window->IsCursorVisible();
    }

}
