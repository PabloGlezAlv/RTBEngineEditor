#pragma once

#include <string>
#include <functional>
#include <vector>
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
        void DrawStartSceneSelector();
        void RefreshAvailableScenes();
        bool IsSelectedStartSceneValid() const;
        void OnBuild();

    private:
        bool open = false;
        bool showResult = false;
        BuildSettings settings;
        BuildResult lastResult = BuildResult::Success;

        bool isBuilding = false;
        float buildProgress = 0.0f;
        std::string statusMessage;

        std::vector<std::string> availableScenes;
        int selectedStartSceneIndex = -1;
        std::string selectedStartScene;
    };

}
