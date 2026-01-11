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
        bool open = false;
        bool showResult = false;
        BuildSettings settings;
        BuildResult lastResult = BuildResult::Success;

        bool isBuilding = false;
        float buildProgress = 0.0f;
        std::string statusMessage;
    };

}
