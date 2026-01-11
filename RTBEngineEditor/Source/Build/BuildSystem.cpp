#include "BuildSystem.h"
#include <fstream>
#include <iostream>
#include <RTBEngine/Core/Logger.h>

namespace RTBEditor {

    BuildResult BuildSystem::Build(const BuildSettings& settings, ProgressCallback onProgress) {
        RTB_INFO("Starting build process for game: " + settings.gameName);
        if (onProgress) onProgress("Starting build...", 0.0f);

        if (settings.outputDirectory.empty()) {
            RTB_ERROR("Build failed: Output directory is empty");
            return BuildResult::InvalidOutputDirectory;
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

        if (onProgress) onProgress("Copying DLLs...", 0.3f);
        if (!CopyDLLs(settings.outputDirectory)) {
            RTB_ERROR("Build failed: Could not copy DLLs");
            return BuildResult::CopyFailed;
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
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return false;
        }
    }

    bool BuildSystem::CopyPlayerExecutable(const std::filesystem::path& outputDir, const std::string& gameName) {
        try {
            auto sdkPath = GetSDKPath();
            auto playerPath = sdkPath / "Bin/RTBPlayer.exe";

            if (!std::filesystem::exists(playerPath)) {
                std::cerr << "RTBPlayer not found at: " << playerPath << std::endl;
                return false;
            }

            auto destPath = outputDir / (gameName + ".exe");
            std::filesystem::copy_file(playerPath, destPath, std::filesystem::copy_options::overwrite_existing);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error copying player: " << e.what() << std::endl;
            return false;
        }
    }

    bool BuildSystem::CopyDLLs(const std::filesystem::path& outputDir) {
        try {
            auto sdkPath = GetSDKPath();
            auto binPath = sdkPath / "Bin";

            for (const auto& entry : std::filesystem::directory_iterator(binPath)) {
                if (entry.path().extension() == ".dll") {
                    std::filesystem::copy_file(entry.path(), outputDir / entry.path().filename(), std::filesystem::copy_options::overwrite_existing);
                }
            }
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error copying DLLs: " << e.what() << std::endl;
            return false;
        }
    }

    bool BuildSystem::CopyDefaultFolder(const std::filesystem::path& outputDir) {
        try {
            auto sdkPath = GetSDKPath();
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
            std::cerr << "Error copying Default folder: " << e.what() << std::endl;
            return false;
        }
    }

    bool BuildSystem::CopyAssetsFolder(const std::filesystem::path& outputDir) {
        try {
            // Assuming Assets are in current working directory of the editor
            auto assetsPath = std::filesystem::current_path() / "Assets";
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
            std::cerr << "Error copying Assets folder: " << e.what() << std::endl;
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
            cfgFile << "StartScene=" << (settings.startScene.empty() ? "Assets/Scenes/DefaultScene.lua" : settings.startScene) << "\n";

            cfgFile.close();
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error writing config: " << e.what() << std::endl;
            return false;
        }
    }

    std::filesystem::path BuildSystem::GetSDKPath() {
        // Assuming RTBEngine_SDK is relative to the editor executable or project root
        // Adjust this if SDK is located elsewhere
        return std::filesystem::current_path() / "RTBEngine_SDK";
    }

}
