#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace RTBEditor {

    class MultiplayerTestLauncher {
    public:
        struct LaunchSettings {
            int playerCount = 2;
            std::string startScene = "Assets/Scenes/LobbyScene.lua";
        };

        struct PlayerInstance {
            int playerIndex = 0;
            unsigned long processId = 0;
            void* processHandle = nullptr;
            std::filesystem::path workingDirectory;
            bool running = false;
            unsigned long exitCode = 0;
        };

        MultiplayerTestLauncher() = default;
        ~MultiplayerTestLauncher();

        MultiplayerTestLauncher(const MultiplayerTestLauncher&) = delete;
        MultiplayerTestLauncher& operator=(const MultiplayerTestLauncher&) = delete;

        bool PrepareAndLaunch(const LaunchSettings& settings);
        void StopAll();
        bool CleanLastRun();
        void RefreshProcessStates();

        const std::vector<PlayerInstance>& GetInstances() const { return instances; }
        const std::string& GetLastMessage() const { return lastMessage; }
        bool WasLastActionSuccessful() const { return lastActionSuccessful; }
        const std::filesystem::path& GetLastRunDirectory() const { return lastRunDirectory; }

    private:
        std::vector<PlayerInstance> instances;
        std::filesystem::path lastRunDirectory;
        std::string lastMessage;
        bool lastActionSuccessful = true;

        void CloseProcessHandles();
        bool PreparePlayerDirectory(
            const std::filesystem::path& playerDirectory,
            int playerIndex,
            const LaunchSettings& settings,
            const std::filesystem::path& scriptsDllPath,
            std::filesystem::path& outExecutablePath);
        bool LaunchPlayerProcess(
            const std::filesystem::path& executablePath,
            const std::filesystem::path& workingDirectory,
            int playerIndex);
        bool WritePlayerConfig(
            const std::filesystem::path& playerDirectory,
            int playerIndex,
            const LaunchSettings& settings);
        std::filesystem::path BuildRunDirectory() const;
        void SetResult(bool success, const std::string& message);
    };

}
