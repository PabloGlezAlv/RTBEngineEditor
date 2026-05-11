#pragma once

#include <string>
#include <filesystem>
#include <functional>

namespace RTBEditor {

    struct BuildSettings {
        std::string gameName = "MyGame";
        std::filesystem::path outputDirectory;
        std::string startScene = "Assets/Scenes/DefaultScene.lua";
        int windowWidth = 1280;
        int windowHeight = 720;
        bool fullscreen = false;
    };

    enum class BuildResult {
        Success,
        NoProjectLoaded,
        InvalidOutputDirectory,
        InvalidStartScene,
        PlayerNotFound,
        CopyFailed,
        ConfigWriteFailed
    };

    enum class ScriptCompileResult {
        Success,
        MSBuildNotFound,
        CompileError,
        Failure
    };

    class BuildSystem {
    public:
        using ProgressCallback = std::function<void(const std::string& status, float progress)>;

        static BuildResult Build(const BuildSettings& settings, ProgressCallback onProgress = nullptr);
        static std::string GetResultMessage(BuildResult result);
        static std::filesystem::path GetCompiledScriptsDllPath(const std::string& configuration = "Debug");

        // Invokes MSBuild to compile GameScripts.vcxproj into GameScripts.dll
        static ScriptCompileResult CompileScripts(const std::string& vcxprojPath, const std::string& configuration = "Debug");

        // Invokes MSBuild with an isolated output directory so the editor-loaded GameScripts.dll is not overwritten.
        static ScriptCompileResult CompileScriptsToDirectory(
            const std::string& vcxprojPath,
            const std::filesystem::path& outputDirectory,
            const std::string& configuration = "Debug");

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
