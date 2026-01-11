#pragma once
#include "EditorPanel.h"
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <memory>

namespace RTBEditor {

    class GameViewPanel : public EditorPanel {
    public:
        GameViewPanel();
        ~GameViewPanel();

        virtual void OnUIRender(EditorContext& context) override;
        
        uint32_t GetViewportWidth() const { return viewportWidth; }
        uint32_t GetViewportHeight() const { return viewportHeight; }
        RTBEngine::Rendering::Framebuffer* GetFramebuffer() { return framebuffer.get(); }
        bool IsVisible() const { return isVisible; }

    private:
        std::unique_ptr<RTBEngine::Rendering::Framebuffer> framebuffer;
        uint32_t viewportWidth = 1280;
        uint32_t viewportHeight = 720;
        bool isVisible = true;
    };
}
