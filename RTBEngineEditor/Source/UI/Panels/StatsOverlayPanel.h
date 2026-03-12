#pragma once
#include "EditorPanel.h"

namespace RTBEditor {

    class StatsOverlayPanel : public EditorPanel {
    public:
        StatsOverlayPanel() = default;
        ~StatsOverlayPanel() override = default;

        StatsOverlayPanel(const StatsOverlayPanel&) = delete;
        StatsOverlayPanel& operator=(const StatsOverlayPanel&) = delete;

        void OnUIRender(EditorContext& context) override;
    };

}
