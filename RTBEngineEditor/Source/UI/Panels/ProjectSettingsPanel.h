#pragma once

#include "EditorPanel.h"

#include <RTBEngine/Rendering/RHI/GraphicsAPI.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

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
        bool SaveToActiveProject(EditorContext& context);

        RTBEngine::Rendering::RHI::GraphicsAPI selectedGraphicsAPI =
            RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;
        RTBEngine::Rendering::RHI::GraphicsAPI activeRuntimeAPI =
            RTBEngine::Rendering::RHI::GraphicsAPI::OpenGL;

        RTBEngine::Math::Vector3 ambientColor = RTBEngine::Math::Vector3(1.0f, 1.0f, 1.0f);
        float ambientIntensity = 0.08f;

        bool ddgiEnabled = true;
        float ddgiIntensity = 0.32f;
        RTBEngine::Math::Vector3 ddgiOrigin = RTBEngine::Math::Vector3(-15.0f, 0.0f, -9.0f);
        RTBEngine::Math::Vector3 ddgiExtent = RTBEngine::Math::Vector3(30.0f, 12.0f, 18.0f);
        int ddgiGridX = 16;
        int ddgiGridY = 4;
        int ddgiGridZ = 12;
        float ddgiHysteresis = 0.55f;
        float ddgiNormalBias = 0.2f;
        float ddgiViewBias = 0.25f;
        float ddgiProbeRadius = 2.0f;

        bool syncedOnce = false;
        std::string lastMessage;
        bool lastSaveSucceeded = true;
    };

}
