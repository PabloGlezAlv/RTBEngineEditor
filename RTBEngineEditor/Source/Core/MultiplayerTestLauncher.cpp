#include "MultiplayerTestLauncher.h"

#include "EditorOnlineSettings.h"
#include "../Build/BuildSystem.h"
#include "../Project/Project.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Online/OnlineConfig.h>

#include <Windows.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {
    namespace fs = std::filesystem;

    std::string NormalizeReferencePath(const std::string& path)
    {
        return fs::path(path).lexically_normal().generic_string();
    }

    std::string QuotePath(const fs::path& path)
    {
        return "\"" + path.string() + "\"";
    }

    fs::path GetLocalAppDataDirectory()
    {
        char* value = nullptr;
        size_t valueSize = 0;
        if (_dupenv_s(&value, &valueSize, "LOCALAPPDATA") == 0 && value) {
            fs::path result(value);
            std::free(value);
            return result;
        }

        return fs::current_path();
    }

    std::string SerializeBackend(RTBEngine::Online::OnlineBackendType backend)
    {
        return backend == RTBEngine::Online::OnlineBackendType::EOS ? "EOS" : "Null";
    }

    bool CopyDirectoryTree(const fs::path& source, const fs::path& destination, std::string& outError)
    {
        try {
            if (!fs::exists(source)) {
                outError = "Missing source directory: " + source.string();
                return false;
            }

            fs::create_directories(destination);
            fs::copy(source, destination, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            return true;
        }
        catch (const std::exception& e) {
            outError = e.what();
            return false;
        }
    }

    bool CopyFileToDirectory(const fs::path& source, const fs::path& destinationDirectory, std::string& outError)
    {
        try {
            if (!fs::exists(source)) {
                outError = "Missing source file: " + source.string();
                return false;
            }

            fs::create_directories(destinationDirectory);
            fs::copy_file(source, destinationDirectory / source.filename(), fs::copy_options::overwrite_existing);
            return true;
        }
        catch (const std::exception& e) {
            outError = e.what();
            return false;
        }
    }

}

namespace RTBEditor {

    MultiplayerTestLauncher::~MultiplayerTestLauncher()
    {
        StopAll();
    }

    bool MultiplayerTestLauncher::PrepareAndLaunch(const LaunchSettings& requestedSettings)
    {
        LaunchSettings settings = requestedSettings;
        settings.playerCount = std::clamp(settings.playerCount, 2, 6);
        if (settings.startScene.empty()) {
            settings.startScene = "Assets/Scenes/LobbyScene.lua";
        }

        StopAll();

        const Project* project = Project::GetActiveProject();
        if (!project) {
            SetResult(false, "No active project loaded.");
            return false;
        }

        lastRunDirectory = BuildRunDirectory();
        std::error_code error;
        fs::create_directories(lastRunDirectory, error);
        if (error) {
            SetResult(false, "Could not create multiplayer test directory: " + error.message());
            return false;
        }

        fs::path scriptsDllPath;
        const fs::path scriptProjectPath = project->GetGameScriptsProjectPath();
        if (fs::exists(scriptProjectPath)) {
            const fs::path scriptBuildDirectory = lastRunDirectory / "ScriptBuild";
            const ScriptCompileResult compileResult = BuildSystem::CompileScriptsToDirectory(
                scriptProjectPath.string(),
                scriptBuildDirectory,
                "Debug");
            if (compileResult != ScriptCompileResult::Success) {
                SetResult(false, "GameScripts Debug build failed.");
                return false;
            }

            scriptsDllPath = scriptBuildDirectory / "GameScripts.dll";
        }

        for (int playerIndex = 1; playerIndex <= settings.playerCount; ++playerIndex) {
            const fs::path playerDirectory = lastRunDirectory / ("Player" + std::to_string(playerIndex));
            fs::path executablePath;
            if (!PreparePlayerDirectory(playerDirectory, playerIndex, settings, scriptsDllPath, executablePath)) {
                StopAll();
                return false;
            }

            if (!LaunchPlayerProcess(executablePath, playerDirectory, playerIndex)) {
                StopAll();
                return false;
            }
        }

        SetResult(true, "Launched " + std::to_string(settings.playerCount) + " multiplayer test players.");
        return true;
    }

    void MultiplayerTestLauncher::StopAll()
    {
        for (PlayerInstance& instance : instances) {
            HANDLE processHandle = static_cast<HANDLE>(instance.processHandle);
            if (!processHandle) {
                continue;
            }

            DWORD exitCode = 0;
            if (GetExitCodeProcess(processHandle, &exitCode) && exitCode == STILL_ACTIVE) {
                TerminateProcess(processHandle, 0);
                WaitForSingleObject(processHandle, 2000);
            }

            CloseHandle(processHandle);
            instance.processHandle = nullptr;
            instance.running = false;
        }

        instances.clear();
    }

    bool MultiplayerTestLauncher::CleanLastRun()
    {
        StopAll();

        if (lastRunDirectory.empty() || !fs::exists(lastRunDirectory)) {
            SetResult(true, "No multiplayer test run to clean.");
            return true;
        }

        std::error_code error;
        fs::remove_all(lastRunDirectory, error);
        if (error) {
            SetResult(false, "Could not clean multiplayer test run: " + error.message());
            return false;
        }

        SetResult(true, "Cleaned multiplayer test run: " + lastRunDirectory.string());
        lastRunDirectory.clear();
        return true;
    }

    void MultiplayerTestLauncher::RefreshProcessStates()
    {
        for (PlayerInstance& instance : instances) {
            HANDLE processHandle = static_cast<HANDLE>(instance.processHandle);
            if (!processHandle) {
                instance.running = false;
                continue;
            }

            DWORD exitCode = 0;
            if (!GetExitCodeProcess(processHandle, &exitCode)) {
                instance.running = false;
                instance.exitCode = 0;
                continue;
            }

            instance.running = exitCode == STILL_ACTIVE;
            instance.exitCode = instance.running ? 0 : exitCode;
        }
    }

    void MultiplayerTestLauncher::CloseProcessHandles()
    {
        for (PlayerInstance& instance : instances) {
            HANDLE processHandle = static_cast<HANDLE>(instance.processHandle);
            if (processHandle) {
                CloseHandle(processHandle);
                instance.processHandle = nullptr;
            }
        }
    }

    bool MultiplayerTestLauncher::PreparePlayerDirectory(
        const fs::path& playerDirectory,
        int playerIndex,
        const LaunchSettings& settings,
        const fs::path& scriptsDllPath,
        fs::path& outExecutablePath)
    {
        const Project* project = Project::GetActiveProject();
        if (!project) {
            SetResult(false, "No active project loaded.");
            return false;
        }

        const fs::path sdkPath = project->GetSDKPath();
        const fs::path sdkBinPath = sdkPath / "Bin";
        const fs::path playerSourcePath = sdkBinPath / "RTBPlayer.exe";
        if (!fs::exists(playerSourcePath)) {
            SetResult(false, "RTBPlayer.exe not found in SDK Bin. Build RTBPlayer first.");
            return false;
        }

        try {
            if (fs::exists(playerDirectory)) {
                fs::remove_all(playerDirectory);
            }

            fs::create_directories(playerDirectory);

            std::string error;
            for (const fs::directory_entry& entry : fs::directory_iterator(sdkBinPath)) {
                if (!entry.is_regular_file()) {
                    continue;
                }

                const fs::path extension = entry.path().extension();
                if (extension == ".exe" || extension == ".dll") {
                    if (!CopyFileToDirectory(entry.path(), playerDirectory, error)) {
                        SetResult(false, error);
                        return false;
                    }
                }
            }

            if (!scriptsDllPath.empty() && fs::exists(scriptsDllPath)) {
                fs::copy_file(scriptsDllPath, playerDirectory / "GameScripts.dll", fs::copy_options::overwrite_existing);
            }

            if (!CopyDirectoryTree(project->GetAssetRootPath(), playerDirectory / "Assets", error)) {
                SetResult(false, error);
                return false;
            }

            const fs::path defaultSourcePath = sdkPath / "Default";
            if (fs::exists(defaultSourcePath) &&
                !CopyDirectoryTree(defaultSourcePath, playerDirectory / "Default", error)) {
                SetResult(false, error);
                return false;
            }

            fs::create_directories(playerDirectory / "EOSCache");

            if (!WritePlayerConfig(playerDirectory, playerIndex, settings)) {
                return false;
            }

            outExecutablePath = playerDirectory / "RTBPlayer.exe";
            return true;
        }
        catch (const std::exception& e) {
            SetResult(false, "Failed to prepare Player" + std::to_string(playerIndex) + ": " + e.what());
            return false;
        }
    }

    bool MultiplayerTestLauncher::LaunchPlayerProcess(
        const fs::path& executablePath,
        const fs::path& workingDirectory,
        int playerIndex)
    {
        STARTUPINFOA startupInfo{};
        startupInfo.cb = sizeof(startupInfo);

        PROCESS_INFORMATION processInfo{};
        std::string commandLine = QuotePath(executablePath);

        const BOOL created = CreateProcessA(
            executablePath.string().c_str(),
            commandLine.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            workingDirectory.string().c_str(),
            &startupInfo,
            &processInfo);

        if (!created) {
            SetResult(false, "Failed to launch Player" + std::to_string(playerIndex) + ".");
            return false;
        }

        CloseHandle(processInfo.hThread);

        PlayerInstance instance;
        instance.playerIndex = playerIndex;
        instance.processId = processInfo.dwProcessId;
        instance.processHandle = processInfo.hProcess;
        instance.workingDirectory = workingDirectory;
        instance.running = true;
        instances.push_back(instance);
        return true;
    }

    bool MultiplayerTestLauncher::WritePlayerConfig(
        const fs::path& playerDirectory,
        int playerIndex,
        const LaunchSettings& settings)
    {
        EditorOnlineSettings editorOnlineSettings = EditorOnlineSettingsStore::Load();
        RTBEngine::Online::OnlineConfig onlineConfig;
        EditorOnlineSettingsStore::ApplyToOnlineConfig(editorOnlineSettings, onlineConfig);
        onlineConfig.loadingInEditor = false;
        onlineConfig.cacheDirectory = (playerDirectory / "EOSCache").string();

        std::ofstream cfgFile(playerDirectory / "game.cfg", std::ios::trunc);
        if (!cfgFile.is_open()) {
            SetResult(false, "Could not write game.cfg for Player" + std::to_string(playerIndex) + ".");
            return false;
        }

        const std::string playerTitle = "RTBEngine Player " + std::to_string(playerIndex);

        cfgFile << "[Game]\n";
        cfgFile << "Name=" << playerTitle << "\n\n";

        cfgFile << "[Window]\n";
        cfgFile << "Title=" << playerTitle << "\n";
        cfgFile << "Width=1280\n";
        cfgFile << "Height=720\n";
        cfgFile << "Fullscreen=false\n\n";

        cfgFile << "[Scene]\n";
        cfgFile << "StartScene=" << NormalizeReferencePath(settings.startScene) << "\n";

        cfgFile << "\n[Online]\n";
        cfgFile << "Enabled=" << (onlineConfig.enabled ? "true" : "false") << "\n";
        cfgFile << "FailApplicationOnError=" << (onlineConfig.failApplicationOnError ? "true" : "false") << "\n";
        cfgFile << "Backend=" << SerializeBackend(onlineConfig.backend) << "\n";
        cfgFile << "ProductName=" << onlineConfig.productName << "\n";
        cfgFile << "ProductVersion=" << onlineConfig.productVersion << "\n";
        cfgFile << "ProductId=" << onlineConfig.productId << "\n";
        cfgFile << "SandboxId=" << onlineConfig.sandboxId << "\n";
        cfgFile << "DeploymentId=" << onlineConfig.deploymentId << "\n";
        cfgFile << "ClientId=" << onlineConfig.clientId << "\n";
        cfgFile << "ClientSecret=" << onlineConfig.clientSecret << "\n";
        cfgFile << "IsServer=" << (onlineConfig.isServer ? "true" : "false") << "\n";
        cfgFile << "DisableOverlay=" << (onlineConfig.disableOverlay ? "true" : "false") << "\n";
        cfgFile << "CacheDirectory=" << onlineConfig.cacheDirectory << "\n";
        cfgFile << "TickBudgetMilliseconds=" << onlineConfig.tickBudgetMilliseconds << "\n";

        if (!cfgFile.good()) {
            SetResult(false, "Failed while writing game.cfg for Player" + std::to_string(playerIndex) + ".");
            return false;
        }

        return true;
    }

    fs::path MultiplayerTestLauncher::BuildRunDirectory() const
    {
        const auto now = std::chrono::system_clock::now().time_since_epoch();
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        return GetLocalAppDataDirectory() /
            "RTBEngineEditor" /
            "MultiplayerTests" /
            ("Run_" + std::to_string(milliseconds));
    }

    void MultiplayerTestLauncher::SetResult(bool success, const std::string& message)
    {
        lastActionSuccessful = success;
        lastMessage = message;

        if (success) {
            RTB_INFO("MultiplayerTestLauncher: " + message);
        } else {
            RTB_ERROR("MultiplayerTestLauncher: " + message);
        }
    }

}
