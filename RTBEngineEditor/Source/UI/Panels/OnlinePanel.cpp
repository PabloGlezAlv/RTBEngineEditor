#include "OnlinePanel.h"

#include <imgui.h>

#include <RTBEngine/Online/OnlineSystem.h>

#include <algorithm>
#include <cstdio>
#include <vector>

namespace {

    template <size_t Size>
    void CopyToBuffer(char(&buffer)[Size], const std::string& value)
    {
        std::snprintf(buffer, Size, "%s", value.c_str());
    }

    std::string ReadBuffer(const char* buffer)
    {
        return buffer ? std::string(buffer) : std::string();
    }

}

namespace RTBEditor {

    OnlinePanel::OnlinePanel()
    {
        LoadSettingsIntoFields();
    }

    OnlinePanel::~OnlinePanel() = default;

    void OnlinePanel::OnUIRender(EditorContext&)
    {
        ImGui::Begin("Online");

        DrawInfrastructureSection();
        ImGui::Separator();
        DrawStatusSection();
        ImGui::Separator();
        DrawMultiplayerTestSection();

        ImGui::End();
    }

    void OnlinePanel::LoadSettingsIntoFields()
    {
        const EditorOnlineSettings settings = EditorOnlineSettingsStore::Load();
        onlineSettingsEnabled = settings.enabled;
        lanGamePort = settings.lanGamePort;
        lanDiscoveryPort = settings.lanDiscoveryPort;
        CopyToBuffer(relayMatchmakingUrl, settings.relayMatchmakingUrl);
        CopyToBuffer(defaultStartScene, settings.defaultStartScene);
    }

    EditorOnlineSettings OnlinePanel::BuildSettingsFromFields() const
    {
        EditorOnlineSettings settings;
        settings.enabled = onlineSettingsEnabled;
        settings.lanGamePort = static_cast<std::uint16_t>(std::max(1, lanGamePort));
        settings.lanDiscoveryPort = static_cast<std::uint16_t>(std::max(1, lanDiscoveryPort));
        settings.relayMatchmakingUrl = ReadBuffer(relayMatchmakingUrl);
        settings.defaultStartScene = ReadBuffer(defaultStartScene);
        if (settings.defaultStartScene.empty()) {
            settings.defaultStartScene = "Assets/Scenes/MainMenu.lua";
        }
        if (settings.relayMatchmakingUrl.empty()) {
            settings.relayMatchmakingUrl = "http://localhost:8080/api/v1";
        }
        return settings;
    }

    bool OnlinePanel::ApplyOnlineSettings(const EditorOnlineSettings& settings)
    {
        return EditorOnlineSettingsStore::ApplyAndInitializeOnline(settings);
    }

    void OnlinePanel::DrawInfrastructureSection()
    {
        ImGui::TextUnformatted("Online Infrastructure");
        ImGui::TextDisabled("Lobby type (LAN / Online) is chosen in the game menu.");

        ImGui::Checkbox("Online Enabled", &onlineSettingsEnabled);

        ImGui::InputInt("LAN Game Port", &lanGamePort);
        ImGui::InputInt("LAN Discovery Port", &lanDiscoveryPort);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Relay Matchmaking URL", relayMatchmakingUrl, sizeof(relayMatchmakingUrl));
        ImGui::TextDisabled("HTTP API base URL for the relay server (e.g. http://localhost:8080/api/v1).");

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Default Start Scene", defaultStartScene, sizeof(defaultStartScene));
        ImGui::TextDisabled("Initial scene for Multiplayer Test player launches.");

        const EditorOnlineSettings currentSettings = BuildSettingsFromFields();
        if (ImGui::Button("Reload")) {
            LoadSettingsIntoFields();
            lastSettingsSucceeded = true;
            lastSettingsMessage = "Settings reloaded.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            lastSettingsSucceeded = EditorOnlineSettingsStore::Save(currentSettings);
            lastSettingsMessage = lastSettingsSucceeded ? "Settings saved." : "Could not save settings.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Save And Apply")) {
            lastSettingsSucceeded = ApplyOnlineSettings(currentSettings);
            lastSettingsMessage = lastSettingsSucceeded
                ? "Settings applied."
                : "Could not apply online settings.";
        }

        if (!lastSettingsMessage.empty()) {
            ImGui::TextColored(lastSettingsSucceeded ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                                                     : ImVec4(0.95f, 0.35f, 0.35f, 1.0f),
                "%s", lastSettingsMessage.c_str());
        }
    }

    void OnlinePanel::DrawStatusSection()
    {
        RTBEngine::Online::OnlineSystem& onlineSystem = RTBEngine::Online::OnlineSystem::GetInstance();

        ImGui::TextUnformatted("Runtime Status");
        ImGui::Text("State: %s", RTBEngine::Online::ToString(onlineSystem.GetState()));
        ImGui::Text("LAN: %s | Relay: %s",
            onlineSystem.IsLanLobbyReady() ? "ready" : "unavailable",
            onlineSystem.IsRelayLobbyReady() ? "ready" : "not configured");

        if (onlineSystem.IsInitialized() && onlineSystem.IsInLobby()) {
            ImGui::Text("Active session: %s",
                RTBEngine::Online::ToString(onlineSystem.GetActiveLobbyBackend()));
        }

        if (!onlineSystem.GetLastError().empty()) {
            ImGui::TextColored(ImVec4(0.95f, 0.45f, 0.35f, 1.0f), "%s", onlineSystem.GetLastError().c_str());
        }
    }

    void OnlinePanel::DrawMultiplayerTestSection()
    {
        multiplayerLauncher.RefreshProcessStates();

        ImGui::TextUnformatted("Multiplayer Test");
        ImGui::TextDisabled("Launches multiple RTBPlayer instances with the infrastructure settings above.");

        ImGui::SetNextItemWidth(120.0f);
        ImGui::InputInt("Player Count", &multiplayerPlayerCount);
        multiplayerPlayerCount = std::clamp(multiplayerPlayerCount, 2, 6);

        const EditorOnlineSettings launchOnlineSettings = BuildSettingsFromFields();
        if (ImGui::Button("Prepare Players")) {
            MultiplayerTestLauncher::LaunchSettings settings;
            settings.playerCount = multiplayerPlayerCount;
            settings.startScene = launchOnlineSettings.defaultStartScene;
            settings.overrideOnlineSettings = true;
            settings.onlineSettings = launchOnlineSettings;
            multiplayerLauncher.Prepare(settings);
        }
        ImGui::SameLine();
        if (ImGui::Button("Launch All Prepared")) {
            multiplayerLauncher.LaunchAllPrepared();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop All")) {
            multiplayerLauncher.StopAll();
        }

        if (!multiplayerLauncher.GetLastMessage().empty()) {
            ImGui::Text("%s", multiplayerLauncher.GetLastMessage().c_str());
        }

        const std::vector<MultiplayerTestLauncher::PlayerInstance>& instances = multiplayerLauncher.GetInstances();
        if (instances.empty()) {
            return;
        }

        if (ImGui::BeginTable("##MultiplayerTestPlayersTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Player");
            ImGui::TableSetupColumn("PID");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();
            for (const MultiplayerTestLauncher::PlayerInstance& instance : instances) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Player %d", instance.playerIndex);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", instance.processId);
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(instance.running ? "Running" : "Stopped");
                ImGui::TableSetColumnIndex(3);
                ImGui::PushID(instance.playerIndex);
                if (instance.running) {
                    if (ImGui::Button("Stop")) {
                        multiplayerLauncher.StopPlayer(instance.playerIndex);
                    }
                } else if (ImGui::Button("Launch")) {
                    multiplayerLauncher.LaunchPreparedPlayer(instance.playerIndex);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

}
