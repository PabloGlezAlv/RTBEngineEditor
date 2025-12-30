#pragma once

#include "EditorPanel.h"
#include <RTBEngine/Core/Logger.h>
#include <vector>
#include <string>

namespace RTBEditor {

    class ConsolePanel : public EditorPanel {
    public:
        ConsolePanel();
        virtual ~ConsolePanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void Clear();
        void AddLog(const RTBEngine::Core::LogMessage& message);

    private:
        bool autoScroll = true;
        bool filterInfo = true;
        bool filterWarning = true;
        bool filterError = true;
        char searchFilter[256] = "";

        // Display settings
        float iconSize = 16.0f;
    };

}
