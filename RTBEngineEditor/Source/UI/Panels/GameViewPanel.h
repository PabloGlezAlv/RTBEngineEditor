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
        
        uint32_t GetViewportWidth() const { return m_ViewportWidth; }
        uint32_t GetViewportHeight() const { return m_ViewportHeight; }
        RTBEngine::Rendering::Framebuffer* GetFramebuffer() { return m_Framebuffer.get(); }
        bool IsVisible() const { return m_IsVisible; }

    private:
        std::unique_ptr<RTBEngine::Rendering::Framebuffer> m_Framebuffer;
        uint32_t m_ViewportWidth = 1280;
        uint32_t m_ViewportHeight = 720;
        bool m_IsVisible = true;
    };
}
