#pragma once

#include <string>
#include <filesystem>

namespace RTBEditor {

    class Project {
    public:
        Project();
        ~Project();

        bool Load(const std::filesystem::path& path);
        bool Save(const std::filesystem::path& path = {});

        const std::string& GetName() const { return name; }
        const std::string& GetStartScene() const { return startScene; }
        const std::string& GetLastOpenScene() const { return lastOpenScene; }
        void SetStartScene(const std::string& path);
        void SetLastOpenScene(const std::string& path);
        const std::filesystem::path& GetProjectFilePath() const { return projectFilePath; }
        const std::filesystem::path& GetProjectDirectory() const { return projectDirectory; }
        const std::filesystem::path& GetAssetDirectory() const { return assetDirectory; }
        std::filesystem::path GetAssetRootPath() const;
        std::filesystem::path ResolveAssetPath(const std::filesystem::path& relativePath) const;
        std::string GetAssetReferencePath(const std::filesystem::path& relativePath) const;
        std::filesystem::path GetGameScriptsProjectPath() const;
        std::filesystem::path GetGameScriptsDllPath(const std::string& configuration = "Debug") const;
        std::filesystem::path GetSDKPath() const;

        static Project* GetActiveProject() { return activeProject; }

    private:
        std::string name = "New Project";
        std::string startScene = "Assets/Scenes/DefaultScene.lua";
        std::string lastOpenScene;
        std::filesystem::path projectFilePath;
        std::filesystem::path projectDirectory;
        std::filesystem::path assetDirectory = "Assets";

        static Project* activeProject;
    };

}
