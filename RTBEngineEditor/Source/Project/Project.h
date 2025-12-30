#pragma once

#include <string>
#include <filesystem>

namespace RTBEditor {

    class Project {
    public:
        Project();
        ~Project();

        bool Load(const std::filesystem::path& path);
        bool Save(const std::filesystem::path& path);

        const std::string& GetName() const { return name; }
        const std::string& GetStartScene() const { return startScene; }
        const std::filesystem::path& GetProjectDirectory() const { return projectDirectory; }
        const std::filesystem::path& GetAssetDirectory() const { return assetDirectory; }

        static Project* GetActiveProject() { return activeProject; }

    private:
        std::string name = "New Project";
        std::string startScene = "Default/Scenes/DefaultScene.lua";
        std::filesystem::path projectDirectory;
        std::filesystem::path assetDirectory = "Assets";

        static Project* activeProject;
    };

}
