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
        bool shadowsEnabled = true;
        int shadowMapResolution = 2048;
        RTBEngine::Math::Vector3 ddgiOrigin = RTBEngine::Math::Vector3(-15.0f, 0.0f, -9.0f);
        RTBEngine::Math::Vector3 ddgiExtent = RTBEngine::Math::Vector3(30.0f, 12.0f, 18.0f);
        int ddgiGridX = 16;
        int ddgiGridY = 4;
        int ddgiGridZ = 12;
        float ddgiHysteresis = 0.55f;
        float ddgiNormalBias = 0.2f;
        float ddgiViewBias = 0.25f;
        float ddgiProbeRadius = 2.0f;

        bool fogEnabled = true;
        RTBEngine::Math::Vector3 fogColor = RTBEngine::Math::Vector3(0.55f, 0.62f, 0.72f);
        float fogDensity = 0.018f;
        float fogHeight = 0.0f;
        float fogHeightFalloff = 0.08f;
        float fogStart = 8.0f;
        float fogEnd = 140.0f;
        bool volumetricFogEnabled = true;
        float volumetricIntensity = 0.45f;
        float volumetricAnisotropy = 0.55f;
        int volumetricSamples = 16;
        float volumetricMaxLuminance = 0.85f;
        bool bloomEnabled = true;
        float bloomThreshold = 1.0f;
        float bloomIntensity = 0.8f;

        bool syncedOnce = false;
        std::string lastMessage;
        bool lastSaveSucceeded = true;
    };

}
