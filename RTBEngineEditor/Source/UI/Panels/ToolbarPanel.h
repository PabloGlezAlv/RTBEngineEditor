#pragma once
#include "EditorPanel.h"
#include <functional>

#include "../../Core/EditorTypes.h"

namespace RTBEditor {

    class ToolbarPanel : public EditorPanel {
    public:
        ToolbarPanel(std::function<void()> onPlay, std::function<void()> onPause, std::function<void()> onStop, std::function<EditorState()> getState);
        virtual void OnUIRender(EditorContext& context) override;

    private:
        std::function<void()> m_OnPlay;
        std::function<void()> m_OnPause;
        std::function<void()> m_OnStop;
        std::function<EditorState()> m_GetState;
    };

}
