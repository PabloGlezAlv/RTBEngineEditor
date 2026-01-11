#pragma once

#include <string>
#include <functional>
#include "../../Build/BuildSystem.h" // Rel path, check include

namespace RTBEditor {

    class BuildDialog {
    public:
        BuildDialog();
        ~BuildDialog() = default;

        void Render();
        void Open();

    private:
        void DrawDirectorySelector();
        void OnBuild();

    private:
        bool m_Open = false;
        bool m_ShowResult = false;
        BuildSettings m_Settings;
        BuildResult m_LastResult = BuildResult::Success;
        
        bool m_IsBuilding = false;
        float m_BuildProgress = 0.0f;
        std::string m_StatusMessage;
    };

}
