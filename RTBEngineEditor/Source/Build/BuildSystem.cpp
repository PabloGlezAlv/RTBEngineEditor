#include "BuildSystem.h"
#include <fstream>
#include <RTBEngine/Core/Logger.h>
#include "../Project/Project.h"
#include <algorithm>
#include <cctype>

// --- MSBuild invocation notes ---
//
// MSBuild is the Microsoft build system used by Visual Studio.
// We invoke it as a child process via system() to compile GameScripts.vcxproj.
//
// system(command)
//   Passes the command string to cmd.exe and waits for it to finish.
//   Returns the exit code of the child process (0 = success for MSBuild).
//   Simple but does not capture stdout/stderr — output goes to the parent console.
//
// MSBuild command format:
//   msbuild <project.vcxproj> /p:Configuration=<Debug|Release> /p:Platform=x64 /t:Build
//   /p:Configuration  — selects which configuration to build
//   /p:Platform       — target architecture (must match the editor: x64)
//   /t:Build          — build target (default); use /t:Rebuild to force a clean rebuild
//   /nologo /v:m      — suppress banner and reduce verbosity (minimal output)
//
// Why msbuild works without full path:
//   Visual Studio's "Developer Command Prompt" adds MSBuild to PATH.
//   When launched from VS or a vswhere-configured shell it resolves automatically.
//   If the editor is launched outside a VS shell, the PATH may not include MSBuild —
//   in that case we could use vswhere.exe to locate it, but for now we rely on PATH.

namespace {
    namespace fs = std::filesystem;

    std::string NormalizeReferencePath(const std::string& path) {
        return fs::path(path).lexically_normal().generic_string();
    }

    bool IsValidStartSceneReference(const RTBEditor::Project& project, const std::string& sceneReference) {
        const std::string normalized = NormalizeReferencePath(sceneReference);
        if (normalized.empty()) {
            return false;
        }

        std::string ext = fs::path(normalized).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (ext != ".lua") {
            return false;
        }

        const std::string assetPrefix = project.GetAssetDirectory().lexically_normal().generic_string();
        if (normalized.rfind(assetPrefix + "/", 0) != 0) {
            return false;
        }

        const fs::path scenePath = (project.GetProjectDirectory() / normalized).lexically_normal();
        return fs::exists(scenePath) && fs::is_regular_file(scenePath);
    }
}

namespace RTBEditor {

    std::filesystem::path BuildSystem::GetCompiledScriptsDllPath(const std::string& configuration)
    {
        const Project* project = Project::GetActiveProject();
        return project ? project->GetGameScriptsDllPath(configuration) : std::filesystem::path();
    }

    BuildResult BuildSystem::Build(const BuildSettings& settings, ProgressCallback onProgress) {
        RTB_INFO("Starting build process for game: " + settings.gameName);
        if (onProgress) onProgress("Starting build...", 0.0f);

        const Project* project = Project::GetActiveProject();
        if (!project) {
            RTB_ERROR("Build failed: No active project loaded");
            return BuildResult::NoProjectLoaded;
        }

        if (settings.outputDirectory.empty()) {
            RTB_ERROR("Build failed: Output directory is empty");
            return BuildResult::InvalidOutputDirectory;
        }

        if (!IsValidStartSceneReference(*project, settings.startScene)) {
            RTB_ERROR("Build failed: Invalid start scene '" + settings.startScene + "'");
            return BuildResult::InvalidStartScene;
        }

        if (onProgress) onProgress("Creating directory structure...", 0.1f);
        if (!CreateDirectoryStructure(settings.outputDirectory)) {
            RTB_ERROR("Build failed: Could not create directory structure at " + settings.outputDirectory.string());
            return BuildResult::InvalidOutputDirectory;
        }

        if (onProgress) onProgress("Copying player executable...", 0.2f);
        if (!CopyPlayerExecutable(settings.outputDirectory, settings.gameName)) {
            RTB_ERROR("Build failed: Could not copy player executable");
            return BuildResult::PlayerNotFound;
        }

        // Compile user scripts before copying DLLs
        if (onProgress) onProgress("Compiling scripts...", 0.25f);
        {
            fs::path gameScriptsVcxproj = project->GetGameScriptsProjectPath();
            if (fs::exists(gameScriptsVcxproj)) {
                auto scriptResult = CompileScripts(gameScriptsVcxproj.string(), "Release");
                if (scriptResult != ScriptCompileResult::Success) {
                    RTB_WARN("BuildSystem: Failed to compile GameScripts — user components won't be available");
                }
            }
        }

        if (onProgress) onProgress("Copying DLLs...", 0.3f);
        if (!CopyDLLs(settings.outputDirectory)) {
            RTB_ERROR("Build failed: Could not copy DLLs");
            return BuildResult::CopyFailed;
        }

        // Copy compiled GameScripts.dll to output
        {
            fs::path compiledDll = GetCompiledScriptsDllPath("Release");
            if (fs::exists(compiledDll)) {
                fs::copy_file(compiledDll, settings.outputDirectory / "GameScripts.dll",
                    fs::copy_options::overwrite_existing);
                RTB_INFO("BuildSystem: Copied GameScripts.dll to output");
            }
        }

        if (onProgress) onProgress("Copying Default Folder...", 0.5f);
        if (!CopyDefaultFolder(settings.outputDirectory)) {
            RTB_ERROR("Build failed: Could not copy Default folder");
            return BuildResult::CopyFailed;
        }

        if (onProgress) onProgress("Copying Assets...", 0.7f);
        if (!CopyAssetsFolder(settings.outputDirectory)) {
             RTB_ERROR("Build failed: Could not copy Assets folder");
            return BuildResult::CopyFailed;
        }

        if (onProgress) onProgress("Generating game.cfg...", 0.9f);
        if (!WriteGameConfig(settings.outputDirectory, settings)) {
            RTB_ERROR("Build failed: Could not write game.cfg");
            return BuildResult::ConfigWriteFailed;
        }

        if (onProgress) onProgress("Build completed!", 1.0f);
        RTB_INFO("Build completed successfully at: " + settings.outputDirectory.string());
        return BuildResult::Success;
    }

    std::string BuildSystem::GetResultMessage(BuildResult result) {
        switch (result) {
        case BuildResult::Success: return "Build Successful";
        case BuildResult::NoProjectLoaded: return "No Project Loaded";
        case BuildResult::InvalidOutputDirectory: return "Invalid Output Directory";
        case BuildResult::InvalidStartScene: return "Invalid Start Scene";
        case BuildResult::PlayerNotFound: return "Player Executable Not Found";
        case BuildResult::CopyFailed: return "File Copy Failed";
        case BuildResult::ConfigWriteFailed: return "Config Write Failed";
        default: return "Unknown Error";
        }
    }

    bool BuildSystem::CreateDirectoryStructure(const std::filesystem::path& outputDir) {
        try {
            if (!std::filesystem::exists(outputDir)) {
                std::filesystem::create_directories(outputDir);
            }
            return true;
        }
        catch (const std::exception& e) {
            RTB_ERROR(std::string("Error creating directory: ") + e.what());
            return false;
        }
    }

    bool BuildSystem::CopyPlayerExecutable(const std::filesystem::path& outputDir, const std::string& gameName) {
        try {
            auto sdkPath = GetSDKPath();
            if (sdkPath.empty()) {
                RTB_ERROR("RTBPlayer copy failed: No active project SDK path available");
                return false;
            }
            auto playerPath = sdkPath / "Bin/RTBPlayer.exe";

            if (!std::filesystem::exists(playerPath)) {
                RTB_ERROR("RTBPlayer not found at: " + playerPath.string());
                return false;
            }

            auto destPath = outputDir / (gameName + ".exe");
            std::filesystem::copy_file(playerPath, destPath, std::filesystem::copy_options::overwrite_existing);
            return true;
        }
        catch (const std::exception& e) {
            RTB_ERROR(std::string("Error copying player: ") + e.what());
            return false;
        }
    }

    bool BuildSystem::CopyDLLs(const std::filesystem::path& outputDir) {
        try {
            auto sdkPath = GetSDKPath();
            if (sdkPath.empty()) {
                RTB_ERROR("DLL copy failed: No active project SDK path available");
                return false;
            }
            auto binPath = sdkPath / "Bin";

            for (const auto& entry : std::filesystem::directory_iterator(binPath)) {
                if (entry.path().extension() == ".dll") {
                    std::filesystem::copy_file(entry.path(), outputDir / entry.path().filename(), std::filesystem::copy_options::overwrite_existing);
                }
            }
            return true;
        }
        catch (const std::exception& e) {
            RTB_ERROR(std::string("Error copying DLLs: ") + e.what());
            return false;
        }
    }

    bool BuildSystem::CopyDefaultFolder(const std::filesystem::path& outputDir) {
        try {
            auto sdkPath = GetSDKPath();
            if (sdkPath.empty()) {
                RTB_ERROR("Default folder copy failed: No active project SDK path available");
                return false;
            }
            auto defaultPath = sdkPath / "Default";
            auto destPath = outputDir / "Default";

            if (std::filesystem::exists(defaultPath)) {
                if (!std::filesystem::exists(destPath)) {
                    std::filesystem::create_directory(destPath);
                }
                std::filesystem::copy(defaultPath, destPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
            }
            return true;
        }
        catch (const std::exception& e) {
            RTB_ERROR(std::string("Error copying Default folder: ") + e.what());
            return false;
        }
    }

    bool BuildSystem::CopyAssetsFolder(const std::filesystem::path& outputDir) {
        try {
            const Project* project = Project::GetActiveProject();
            if (!project) {
                RTB_ERROR("Assets copy failed: No active project loaded");
                return false;
            }

            auto assetsPath = project->GetAssetRootPath();
            auto destPath = outputDir / "Assets";

            if (std::filesystem::exists(assetsPath)) {
                if (!std::filesystem::exists(destPath)) {
                    std::filesystem::create_directory(destPath);
                }
                std::filesystem::copy(assetsPath, destPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
            }
            return true;
        }
        catch (const std::exception& e) {
            RTB_ERROR(std::string("Error copying Assets folder: ") + e.what());
            return false;
        }
    }

    bool BuildSystem::WriteGameConfig(const std::filesystem::path& outputDir, const BuildSettings& settings) {
        try {
            std::ofstream cfgFile(outputDir / "game.cfg");
            if (!cfgFile.is_open()) return false;

            cfgFile << "[Game]\n";
            cfgFile << "Name=" << settings.gameName << "\n\n";

            cfgFile << "[Window]\n";
            cfgFile << "Title=" << settings.gameName << "\n";
            cfgFile << "Width=" << settings.windowWidth << "\n";
            cfgFile << "Height=" << settings.windowHeight << "\n";
            cfgFile << "Fullscreen=" << (settings.fullscreen ? "true" : "false") << "\n\n";

            cfgFile << "[Scene]\n";
            cfgFile << "StartScene=" << NormalizeReferencePath(settings.startScene) << "\n";

            cfgFile.close();
            return true;
        }
        catch (const std::exception& e) {
            RTB_ERROR(std::string("Error writing config: ") + e.what());
            return false;
        }
    }

    std::filesystem::path BuildSystem::GetSDKPath() {
        const Project* project = Project::GetActiveProject();
        return project ? project->GetSDKPath() : std::filesystem::path();
    }

    ScriptCompileResult BuildSystem::CompileScripts(const std::string& vcxprojPath, const std::string& configuration)
    {
        if (!std::filesystem::exists(vcxprojPath)) {
            RTB_ERROR("CompileScripts: vcxproj not found at '" + vcxprojPath + "'");
            return ScriptCompileResult::MSBuildNotFound;
        }

        // Use an explicit path to MSBuild so the editor does not depend on PATH.
        // This path is based on your current Visual Studio installation.
        const std::string msbuildPath =
            "C:\\Program Files\\Microsoft Visual Studio\\18\\Community\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe";

        if (!std::filesystem::exists(msbuildPath)) {
            RTB_ERROR("CompileScripts: MSBuild.exe not found at the expected path. "
                "Please update BuildSystem::CompileScripts with the correct MSBuild location.");
            return ScriptCompileResult::MSBuildNotFound;
        }

        // Build the MSBuild command.
        // We invoke via cmd /C and carefully quote the path with spaces.
        // /nologo suppresses the banner, /v:m = minimal verbosity.
        std::string cmd =
            "cmd /C \"\"" + msbuildPath + "\" \"" + vcxprojPath + "\""
            " /p:Configuration=" + configuration +
            " /p:Platform=x64"
            " /t:Build"
            " /nologo /v:m\"";

        RTB_INFO("CompileScripts: Running: " + cmd);

        // system() blocks until MSBuild finishes and returns its exit code.
        // MSBuild returns 0 on success and non-zero on any build error.
        int exitCode = system(cmd.c_str());

        if (exitCode != 0) {
            RTB_ERROR("CompileScripts: MSBuild failed with exit code " + std::to_string(exitCode));
            return ScriptCompileResult::CompileError;
        }

        RTB_INFO("CompileScripts: GameScripts compiled successfully.");
        const std::filesystem::path outputDll = GetCompiledScriptsDllPath(configuration);
        if (outputDll.empty()) {
            RTB_ERROR("CompileScripts: No active project loaded to resolve GameScripts.dll output path");
            return ScriptCompileResult::Failure;
        }
        if (!std::filesystem::exists(outputDll)) {
            RTB_ERROR("CompileScripts: Build succeeded but expected DLL not found at '" + outputDll.string() + "'");
            return ScriptCompileResult::Failure;
        }
        RTB_INFO("CompileScripts: Output DLL at '" + outputDll.string() + "'");
        return ScriptCompileResult::Success;
    }

}
