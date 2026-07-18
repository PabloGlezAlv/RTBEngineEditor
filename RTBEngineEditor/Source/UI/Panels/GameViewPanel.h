#pragma once
#include "EditorPanel.h"
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <memory>

namespace RTBEditor {

    class GameViewPanel : public EditorPanel {
    public:
        GameViewPanel();
        ~GameViewPanel();

        virtual void OnUIRender(EditorContext& context) override;

        uint32_t GetViewportWidth() const { return viewportWidth; }
        uint32_t GetViewportHeight() const { return viewportHeight; }
        uint32_t GetRenderWidth() const { return renderWidth; }
        uint32_t GetRenderHeight() const { return renderHeight; }
        float GetPreviewZoom() const { return previewZoom; }
        float GetDisplayScale() const { return displayScale; }
        RTBEngine::Math::Vector2 GetDisplayOffset() const { return displayOffset; }
        RTBEngine::Rendering::Framebuffer* GetFramebuffer() { return framebuffer.get(); }
        bool IsVisible() const { return isVisible; }

    private:
        void ReleaseMouseCapture(EditorContext& context);
        void UpdateMouseCapture(EditorContext& context);
        bool IsGameOwningMouse(const EditorContext& context) const;
        void ResolveReferenceResolution(RTBEngine::Scene::Scene* scene);
        void UpdatePreviewLayout(const RTBEngine::Math::Vector2& availableSize);

        std::unique_ptr<RTBEngine::Rendering::Framebuffer> framebuffer;
        uint32_t viewportWidth = 1280;
        uint32_t viewportHeight = 720;
        uint32_t renderWidth = 1920;
        uint32_t renderHeight = 1080;
        float previewZoom = 1.0f;
        float displayScale = 1.0f;
        RTBEngine::Math::Vector2 displayOffset = RTBEngine::Math::Vector2(0.0f, 0.0f);
        RTBEngine::Math::Vector2 referenceResolution = RTBEngine::Math::Vector2(1920.0f, 1080.0f);
        bool isVisible = true;
        bool isFocused = false;
        bool isHovered = false;
    };
}
