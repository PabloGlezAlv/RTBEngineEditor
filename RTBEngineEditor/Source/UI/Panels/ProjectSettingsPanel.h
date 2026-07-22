#pragma once

#include "EditorPanel.h"

#include <RTBEngine/Rendering/RHI/GraphicsAPI.h>

#include <string>

namespace RTBEditor {

    class ProjectSettingsPanel : public EditorPanel {
    public:
        ProjectSettingsPanel();
        ~ProjectSettingsPanel() override = default;

        ProjectSettingsPanel(const ProjectSettingsPanel&) = delete;
        ProjectSettingsPanel& operator=(const ProjectSettingsPanel&) = delete;

        void OnUIRender(EditorContext& context) override;

    private:
        void SyncFromActiveProject();
        bool SaveToActiveProject();

        RTBEngine::Rendering::RHI::GraphicsAPI selectedGraphicsAPI =
            RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
        RTBEngine::Rendering::RHI::GraphicsAPI activeRuntimeAPI =
            RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
        bool syncedOnce = false;
        std::string lastMessage;
        bool lastSaveSucceeded = true;
    };

}
