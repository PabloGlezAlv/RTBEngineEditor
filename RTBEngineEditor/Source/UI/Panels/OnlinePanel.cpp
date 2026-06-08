#include "OnlinePanel.h"

#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <vector>

namespace {

    void BeginDisabledIf(bool disabled)
    {
        if (disabled) {
            ImGui::BeginDisabled();
        }
    }

    void EndDisabledIf(bool disabled)
    {
        if (disabled) {
            ImGui::EndDisabled();
        }
    }

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
        RTBEngine::Online::OnlineSystem& onlineSystem = RTBEngine::Online::OnlineSystem::GetInstance();

        onlineSettingsSyncedOnOpen = true;

        DrawSettingsSection();
        ImGui::Separator();
        DrawSessionSection(onlineSystem);
        ImGui::Separator();
        DrawMultiplayerTestSection();

        ImGui::End();
    }

    void OnlinePanel::StoreIdentityActionResult(const RTBEngine::Online::OnlineResult& result)
    {
        lastIdentityActionSucceeded = result.success;
        if (result.success) {
            lastIdentityActionMessage = result.message.empty() ? "Operation succeeded." : result.message;
            return;
        }

        lastIdentityActionMessage = std::string(RTBEngine::Online::ToString(result.errorCode));
        if (!result.message.empty()) {
            lastIdentityActionMessage += ": ";
            lastIdentityActionMessage += result.message;
        }
    }

    void OnlinePanel::StoreLobbyActionResult(const RTBEngine::Online::OnlineResult& result)
    {
        lastLobbyActionSucceeded = result.success;
        if (result.success) {
            lastLobbyActionMessage = result.message.empty() ? "Operation succeeded." : result.message;
            return;
        }

        lastLobbyActionMessage = std::string(RTBEngine::Online::ToString(result.errorCode));
        if (!result.message.empty()) {
            lastLobbyActionMessage += ": ";
            lastLobbyActionMessage += result.message;
        }
    }

    void OnlinePanel::LoadSettingsIntoFields()
    {
        const EditorOnlineSettings settings = EditorOnlineSettingsStore::Load();
        onlineSettingsEnabled = settings.enabled;
        defaultLobbyBackendIndex =
            RTBEngine::Online::IsRelayBackend(settings.backendType) ? 1 : 0;
        sessionLobbyBackendIndex = defaultLobbyBackendIndex;
        lanGamePort = settings.lanGamePort;
        lanDiscoveryPort = settings.lanDiscoveryPort;
        CopyToBuffer(defaultHostAddress, settings.defaultHostAddress);
        CopyToBuffer(joinHostAddress, settings.defaultHostAddress);
        CopyToBuffer(relayMatchmakingUrl, settings.relayMatchmakingUrl);
        CopyToBuffer(loginDisplayName, settings.loginDisplayName);
        CopyToBuffer(sessionDisplayName, settings.loginDisplayName);
        CopyToBuffer(defaultStartScene, settings.defaultStartScene);
    }

    EditorOnlineSettings OnlinePanel::BuildSettingsFromFields() const
    {
        EditorOnlineSettings settings;
        settings.enabled = onlineSettingsEnabled;
        settings.backendType = defaultLobbyBackendIndex == 1
            ? RTBEngine::Online::OnlineBackendType::RelayOnline
            : RTBEngine::Online::OnlineBackendType::Lan;
        settings.lanGamePort = static_cast<std::uint16_t>(std::max(1, lanGamePort));
        settings.lanDiscoveryPort = static_cast<std::uint16_t>(std::max(1, lanDiscoveryPort));
        settings.defaultHostAddress = ReadBuffer(defaultHostAddress);
        settings.relayMatchmakingUrl = ReadBuffer(relayMatchmakingUrl);
        settings.loginDisplayName = ReadBuffer(loginDisplayName);
        settings.defaultStartScene = ReadBuffer(defaultStartScene);
        if (settings.defaultStartScene.empty()) {
            settings.defaultStartScene = "Assets/Scenes/MainMenu.lua";
        }
        return settings;
    }

    bool OnlinePanel::ApplyOnlineSettings(const EditorOnlineSettings& settings)
    {
        return EditorOnlineSettingsStore::ApplyAndInitializeOnline(settings);
    }

    void OnlinePanel::DrawSettingsSection()
    {
        ImGui::TextUnformatted("Online Settings");
        ImGui::Checkbox("Online Enabled", &onlineSettingsEnabled);

        const char* backendLabels[] = { "LAN", "Relay" };
        ImGui::Combo("Default lobby backend", &defaultLobbyBackendIndex, backendLabels, 2);
        ImGui::TextDisabled("LAN and Relay are both initialized when online is enabled.");

        ImGui::InputInt("LAN Game Port", &lanGamePort);
        ImGui::InputInt("LAN Discovery Port", &lanDiscoveryPort);

        ImGui::SetNextItemWidth(230.0f);
        ImGui::InputText("Login Display Name", loginDisplayName, sizeof(loginDisplayName));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Default remote host (IP or DNS)", defaultHostAddress, sizeof(defaultHostAddress));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Relay matchmaking URL", relayMatchmakingUrl, sizeof(relayMatchmakingUrl));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Default scene", defaultStartScene, sizeof(defaultStartScene));
        ImGui::TextDisabled("Used by Multiplayer Test and as the initial scene when launching players.");

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

    void OnlinePanel::DrawSessionSection(RTBEngine::Online::OnlineSystem& onlineSystem)
    {
        RTBEngine::Online::IOnlineIdentity* identity = onlineSystem.GetIdentity();

        const RTBEngine::Online::OnlineBackendType sessionBackend =
            sessionLobbyBackendIndex == 1
                ? RTBEngine::Online::OnlineBackendType::RelayOnline
                : RTBEngine::Online::OnlineBackendType::Lan;

        RTBEngine::Online::IOnlineLobby* lobby = onlineSystem.GetLobby(sessionBackend);

        ImGui::Text("State: %s | Session: %s",
            RTBEngine::Online::ToString(onlineSystem.GetState()),
            onlineSystem.IsInLobby()
                ? RTBEngine::Online::ToString(onlineSystem.GetActiveLobbyBackend())
                : "none");
        ImGui::Text("LAN: %s | Relay: %s",
            onlineSystem.IsLobbyBackendReady(RTBEngine::Online::OnlineBackendType::Lan) ? "ready" : "unavailable",
            onlineSystem.IsLobbyBackendReady(RTBEngine::Online::OnlineBackendType::RelayOnline) ? "ready" : "not configured");
        if (!onlineSystem.GetLastError().empty()) {
            ImGui::TextColored(ImVec4(0.95f, 0.45f, 0.35f, 1.0f), "%s", onlineSystem.GetLastError().c_str());
        }

        if (!identity) {
            ImGui::TextDisabled("Initialize the online system to continue.");
            return;
        }

        const char* sessionBackendLabels[] = { "LAN", "Relay" };
        ImGui::Combo("Lobby backend", &sessionLobbyBackendIndex, sessionBackendLabels, 2);
        if (!onlineSystem.IsLobbyBackendReady(sessionBackend)) {
            ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.25f, 1.0f),
                "Selected lobby backend is not available. Configure it in Online Settings.");
        }

        if (!lobby) {
            ImGui::TextDisabled("Lobby interface is not available for the selected backend.");
            return;
        }

        const bool loggedIn = identity->IsLoggedIn();
        const bool hasLobby = !lobby->GetCurrentLobby().lobbyId.empty();
        const bool useRelayBackend = RTBEngine::Online::IsRelayBackend(sessionBackend);

        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputText("Session display name", sessionDisplayName, sizeof(sessionDisplayName));
        BeginDisabledIf(loggedIn);
        if (ImGui::Button("Login")) {
            RTBEngine::Online::OnlineLoginOptions options;
            options.type = RTBEngine::Online::OnlineLoginType::DeviceId;
            options.displayName = ReadBuffer(sessionDisplayName);
            StoreIdentityActionResult(identity->Login(options));
        }
        EndDisabledIf(loggedIn);
        ImGui::SameLine();
        BeginDisabledIf(!loggedIn);
        if (ImGui::Button("Logout")) {
            identity->Logout();
            lastIdentityActionSucceeded = true;
            lastIdentityActionMessage = "Logged out successfully.";
        }
        EndDisabledIf(!loggedIn);

        ImGui::Text("UserId: %s", identity->GetLocalUserId().ToString().c_str());
        ImGui::Text("Current lobby: %s", hasLobby ? lobby->GetCurrentLobby().lobbyId.c_str() : "none");

        ImGui::Separator();
        ImGui::SetNextItemWidth(130.0f);
        ImGui::InputInt("Max players", &lobbyMaxMembers);
        lobbyMaxMembers = std::clamp(lobbyMaxMembers, 2, 6);
        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputText("Lobby code", lobbyCode, sizeof(lobbyCode));
        BeginDisabledIf(useRelayBackend);
        ImGui::SetNextItemWidth(320.0f);
        ImGui::InputText("Remote host (IP/DNS, optional)", joinHostAddress, sizeof(joinHostAddress));
        EndDisabledIf(useRelayBackend);

        const bool canCreate = loggedIn && !hasLobby;
        const bool canSearchJoin = loggedIn && !hasLobby && lobbyCode[0] != '\0';
        const bool canLeave = loggedIn && hasLobby;

        BeginDisabledIf(!canCreate);
        if (ImGui::Button("Create Lobby")) {
            RTBEngine::Online::OnlineCreateLobbyOptions options;
            options.maxMembers = static_cast<std::uint32_t>(lobbyMaxMembers);
            StoreLobbyActionResult(lobby->CreateLobby(options));
        }
        EndDisabledIf(!canCreate);
        ImGui::SameLine();
        BeginDisabledIf(!canSearchJoin);
        if (ImGui::Button("Find")) {
            RTBEngine::Online::OnlineFindLobbiesOptions options;
            options.lobbyId = ReadBuffer(lobbyCode);
            options.hostAddress = ReadBuffer(joinHostAddress);
            StoreLobbyActionResult(lobby->FindLobbies(options));
        }
        EndDisabledIf(!canSearchJoin);
        ImGui::SameLine();
        BeginDisabledIf(!canSearchJoin);
        if (ImGui::Button("Join")) {
            RTBEngine::Online::OnlineJoinLobbyOptions options;
            options.lobbyId = ReadBuffer(lobbyCode);
            options.hostAddress = ReadBuffer(joinHostAddress);
            StoreLobbyActionResult(lobby->JoinLobby(options));
        }
        EndDisabledIf(!canSearchJoin);
        ImGui::SameLine();
        BeginDisabledIf(!canLeave);
        if (ImGui::Button("Leave Lobby")) {
            StoreLobbyActionResult(lobby->LeaveLobby());
        }
        EndDisabledIf(!canLeave);

        if (!lastIdentityActionMessage.empty()) {
            ImGui::TextColored(lastIdentityActionSucceeded ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                                                           : ImVec4(0.95f, 0.35f, 0.35f, 1.0f),
                "%s", lastIdentityActionMessage.c_str());
        }
        if (!lastLobbyActionMessage.empty()) {
            ImGui::TextColored(lastLobbyActionSucceeded ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                                                        : ImVec4(0.95f, 0.35f, 0.35f, 1.0f),
                "%s", lastLobbyActionMessage.c_str());
        }

        if (hasLobby) {
            const RTBEngine::Online::OnlineLobbyInfo& lobbyInfo = lobby->GetCurrentLobby();
            ImGui::Separator();
            ImGui::Text("Members (%u / %u)", lobbyInfo.currentMembers, lobbyInfo.maxMembers);
            for (const RTBEngine::Online::OnlineUserId& member : lobbyInfo.memberUserIds) {
                const std::string displayName = lobby->GetMemberDisplayName(member);
                ImGui::BulletText("%s | %s",
                    displayName.empty() ? "?" : displayName.c_str(),
                    member.ToString().c_str());
            }
        } else if (!lobby->GetSearchResults().empty()) {
            const RTBEngine::Online::OnlineLobbyInfo& foundLobby = lobby->GetSearchResults().front();
            ImGui::Separator();
            ImGui::Text("Search: %s (%u / %u)",
                foundLobby.lobbyId.c_str(),
                foundLobby.currentMembers,
                foundLobby.maxMembers);
            for (const RTBEngine::Online::OnlineUserId& member : foundLobby.memberUserIds) {
                ImGui::BulletText("%s", member.ToString().c_str());
            }
        }
    }

    void OnlinePanel::DrawMultiplayerTestSection()
    {
        multiplayerLauncher.RefreshProcessStates();

        ImGui::TextUnformatted("Multiplayer Test");
        ImGui::SetNextItemWidth(120.0f);
        ImGui::InputInt("Player Count", &multiplayerPlayerCount);
        multiplayerPlayerCount = std::clamp(multiplayerPlayerCount, 2, 6);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Start Scene", defaultStartScene, sizeof(defaultStartScene));

        const EditorOnlineSettings launchOnlineSettings = BuildSettingsFromFields();
        if (ImGui::Button("Prepare Players")) {
            MultiplayerTestLauncher::LaunchSettings settings;
            settings.playerCount = multiplayerPlayerCount;
            settings.startScene = ReadBuffer(defaultStartScene);
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
