#pragma once

#include <string>
#include <filesystem>
#include <functional>

namespace RTBEditor {

    struct BuildSettings {
        std::string gameName = "MyGame";
        std::filesystem::path outputDirectory;
        std::string startScene;
        int windowWidth = 1280;
        int windowHeight = 720;
        bool fullscreen = false;
    };

    enum class BuildResult {
        Success,
        NoProjectLoaded,
        InvalidOutputDirectory,
        PlayerNotFound,
        CopyFailed,
        ConfigWriteFailed
    };

    class BuildSystem {
    public:
        using ProgressCallback = std::function<void(const std::string& status, float progress)>;

        static BuildResult Build(const BuildSettings& settings, ProgressCallback onProgress = nullptr);
        static std::string GetResultMessage(BuildResult result);

    private:
        static bool CreateDirectoryStructure(const std::filesystem::path& outputDir);
        static bool CopyPlayerExecutable(const std::filesystem::path& outputDir, const std::string& gameName);
        static bool CopyDLLs(const std::filesystem::path& outputDir);
        static bool CopyDefaultFolder(const std::filesystem::path& outputDir);
        static bool CopyAssetsFolder(const std::filesystem::path& outputDir);
        static bool WriteGameConfig(const std::filesystem::path& outputDir, const BuildSettings& settings);

        static std::filesystem::path GetSDKPath();
    };

}
