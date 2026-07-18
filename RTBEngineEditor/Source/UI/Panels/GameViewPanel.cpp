#include "GameViewPanel.h"
#include <imgui.h>
#include <RTBEngine/Core/Window.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/UI/CanvasSystem.h>
#include <RTBEngine/UI/Canvas.h>
#include <algorithm>

namespace RTBEditor {

    GameViewPanel::GameViewPanel() {
        framebuffer = std::make_unique<RTBEngine::Rendering::Framebuffer>();
        framebuffer->CreateWithColorAndDepth(renderWidth, renderHeight);
    }

    GameViewPanel::~GameViewPanel() {}

    void GameViewPanel::ResolveReferenceResolution(RTBEngine::Scene::Scene* scene) {
        referenceResolution = RTBEngine::Math::Vector2(1920.0f, 1080.0f);
        if (!scene) {
            return;
        }

        for (const auto& objPtr : scene->GetGameObjects()) {
            RTBEngine::Scene::GameObject* obj = objPtr.get();
            if (!obj) {
                continue;
            }

            RTBEngine::UI::Canvas* canvas = obj->GetComponent<RTBEngine::UI::Canvas>();
            if (!canvas || !canvas->IsEnabled() || !obj->IsActiveInHierarchy()) {
                continue;
            }

            if (canvas->GetRenderMode() == RTBEngine::UI::Canvas::RenderMode::WorldSpace) {
                continue;
            }

            referenceResolution = canvas->GetCanvasSize();
            return;
        }
    }

    void GameViewPanel::UpdatePreviewLayout(const RTBEngine::Math::Vector2& availableSize) {
        const float refWidth = std::max(1.0f, referenceResolution.x);
        const float refHeight = std::max(1.0f, referenceResolution.y);
        const float safeZoom = std::max(0.01f, previewZoom);

        const float fitScale = std::min(
            availableSize.x / refWidth,
            availableSize.y / refHeight
        );

        displayScale = fitScale * safeZoom;

        const float displayWidth = refWidth * displayScale;
        const float displayHeight = refHeight * displayScale;

        displayOffset.x = std::max(0.0f, (availableSize.x - displayWidth) * 0.5f);
        displayOffset.y = std::max(0.0f, (availableSize.y - displayHeight) * 0.5f);

        viewportWidth = static_cast<uint32_t>(std::max(1.0f, displayWidth));
        viewportHeight = static_cast<uint32_t>(std::max(1.0f, displayHeight));

        renderWidth = static_cast<uint32_t>(refWidth);
        renderHeight = static_cast<uint32_t>(refHeight);

        if (framebuffer) {
            framebuffer->Resize(renderWidth, renderHeight);
        }
    }

    void GameViewPanel::OnUIRender(EditorContext& context) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game", &isVisible);
        isFocused = ImGui::IsWindowFocused();
        isHovered = ImGui::IsWindowHovered();

        RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
        ResolveReferenceResolution(scene);

        ImGui::PopStyleVar();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

        ImGui::SetNextItemWidth(160.0f);
        ImGui::SliderFloat("Zoom##GameView", &previewZoom, 0.25f, 2.0f, "%.2fx");
        ImGui::SameLine();
        ImGui::Text("Ref %.0fx%.0f", referenceResolution.x, referenceResolution.y);

        ImGui::PopStyleVar(2);

        const ImVec2 separatorStart = ImGui::GetCursorScreenPos();
        const float separatorWidth = ImGui::GetContentRegionAvail().x;
        ImGui::GetWindowDrawList()->AddLine(
            separatorStart,
            ImVec2(separatorStart.x + separatorWidth, separatorStart.y),
            IM_COL32(80, 80, 80, 255)
        );
        ImGui::Dummy(ImVec2(0.0f, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        UpdatePreviewLayout(RTBEngine::Math::Vector2(availableSize.x, availableSize.y));

        const ImVec2 viewportAreaOrigin = ImGui::GetCursorScreenPos();
        const ImVec2 displayOrigin(
            viewportAreaOrigin.x + displayOffset.x,
            viewportAreaOrigin.y + displayOffset.y
        );

        GLuint textureID = framebuffer ? framebuffer->GetColorTextureID() : 0;
        if (textureID != 0 && viewportWidth > 0 && viewportHeight > 0) {
            ImGui::SetCursorScreenPos(displayOrigin);
            ImGui::Image(
                (void*)(intptr_t)textureID,
                ImVec2((float)viewportWidth, (float)viewportHeight),
                ImVec2(0, 1),
                ImVec2(1, 0)
            );
        }

        UpdateMouseCapture(context);

        if (scene && viewportWidth > 0 && viewportHeight > 0 && displayScale > 0.0f) {
            RTBEngine::Math::Vector2 logicalSize(referenceResolution.x, referenceResolution.y);
            RTBEngine::Math::Vector2 screenOffset(displayOrigin.x, displayOrigin.y);

            auto& canvasSystem = RTBEngine::UI::CanvasSystem::GetInstance();
            canvasSystem.Update(scene);
            canvasSystem.UpdateAllRectTransforms(logicalSize);

            ImVec2 mousePos = ImGui::GetMousePos();
            const float displayWidth = static_cast<float>(viewportWidth);
            const float displayHeight = static_cast<float>(viewportHeight);
            const bool mouseInViewport = mousePos.x >= displayOrigin.x && mousePos.x <= displayOrigin.x + displayWidth
                                      && mousePos.y >= displayOrigin.y && mousePos.y <= displayOrigin.y + displayHeight;
            const bool mouseReleasedForUi =
                context.window != nullptr && !context.window->IsMouseCaptured();
            const bool routeUiInput = !IsGameOwningMouse(context) || mouseReleasedForUi;
            if (mouseInViewport && context.state == EditorState::Play && routeUiInput) {
                RTBEngine::Math::Vector2 localMouse(
                    (mousePos.x - displayOrigin.x) / displayScale,
                    (mousePos.y - displayOrigin.y) / displayScale
                );
                canvasSystem.ProcessInput(localMouse);
            }

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            canvasSystem.RenderToDrawList(drawList, logicalSize, screenOffset, displayScale);

            if (context.selectedGameObject) {
                auto rects = canvasSystem.GetRaycastRectsForGameObject(context.selectedGameObject);
                for (const auto& rect : rects) {
                    ImVec2 rMin(
                        rect.x * displayScale + screenOffset.x,
                        rect.y * displayScale + screenOffset.y
                    );
                    ImVec2 rMax(
                        (rect.x + rect.z) * displayScale + screenOffset.x,
                        (rect.y + rect.w) * displayScale + screenOffset.y
                    );
                    drawList->AddRect(rMin, rMax, IM_COL32(255, 0, 0, 220), 0.0f, 0, 1.5f);
                }
            }
        }

        ImGui::PopStyleVar();
        ImGui::End();
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
