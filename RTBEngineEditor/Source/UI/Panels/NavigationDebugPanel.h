#pragma once

#include "EditorPanel.h"

namespace RTBEditor {

    class NavigationDebugPanel : public EditorPanel {
    public:
        void OnUIRender(EditorContext& context) override;

    private:
        NavDebugSettings previousNavDebug{};
        bool hasPreviousNavDebug = false;
    };

}
