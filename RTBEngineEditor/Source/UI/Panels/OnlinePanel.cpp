#include "OnlinePanel.h"

#include <imgui.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <utility>
#include <vector>

namespace {

    constexpr std::size_t MaxLoginEvents = 64;
    constexpr std::size_t MaxLobbyEvents = 64;

    void DrawTextRow(const char* label, const char* value)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(value && value[0] != '\0' ? value : "None");
    }

    void DrawStringRow(const char* label, const std::string& value)
    {
        DrawTextRow(label, value.empty() ? "None" : value.c_str());
    }

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

    template<size_t Size>
    void CopyToBuffer(char (&buffer)[Size], const std::string& value)
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

    OnlinePanel::~OnlinePanel()
    {
        loginStatusSubscription.Reset();
        lobbyStatusSubscription.Reset();
    }

    void OnlinePanel::OnUIRender(EditorContext& /*context*/)
    {
        ImGui::Begin("Online");

        RTBEngine::Online::OnlineSystem& onlineSystem =
            RTBEngine::Online::OnlineSystem::GetInstance();
        RTBEngine::Online::IOnlineIdentity* identity = onlineSystem.GetIdentity();
        RTBEngine::Online::IOnlineLobby* lobby = onlineSystem.GetLobby();

        // Keep the debug panel connected to the currently active backend identity.
        RefreshIdentitySubscription(identity);
        RefreshLobbySubscription(lobby);

        DrawSettingsSection();
        ImGui::Separator();
        DrawSystemSection(onlineSystem);
        ImGui::Separator();
        DrawIdentitySection(identity);
        ImGui::Separator();
        DrawLobbySection(lobby, identity);
        ImGui::Separator();
        DrawMultiplayerTestSection();
        ImGui::Separator();
        DrawLoginEventsSection();
        ImGui::Separator();
        DrawLobbyEventsSection();

        ImGui::End();
    }

    void OnlinePanel::RefreshIdentitySubscription(RTBEngine::Online::IOnlineIdentity* identity)
    {
        if (identity == subscribedIdentity && loginStatusSubscription.IsValid()) {
            return;
        }

        // Drop the old listener before subscribing to a new identity instance.
        loginStatusSubscription.Reset();
        subscribedIdentity = identity;

        if (!identity) {
            return;
        }

        loginStatusSubscription = identity->SubscribeLoginStatusChanged(
            [this](const RTBEngine::Online::OnlineLoginStatusChangedEvent& eventData) {
                AddLoginEvent(eventData);
            });
    }

    void OnlinePanel::RefreshLobbySubscription(RTBEngine::Online::IOnlineLobby* lobby)
    {
        if (lobby == subscribedLobby && lobbyStatusSubscription.IsValid()) {
            return;
        }

        // Drop the old listener before subscribing to a new lobby instance.
        lobbyStatusSubscription.Reset();
        subscribedLobby = lobby;

        if (!lobby) {
            return;
        }

        lobbyStatusSubscription = lobby->SubscribeLobbyStatusChanged(
            [this](const RTBEngine::Online::OnlineLobbyStatusChangedEvent& eventData) {
                AddLobbyEvent(eventData);
            });
    }

    void OnlinePanel::AddLoginEvent(const RTBEngine::Online::OnlineLoginStatusChangedEvent& eventData)
    {
        LoginEventEntry entry;
        entry.previousStatus = RTBEngine::Online::ToString(eventData.previousStatus);
        entry.currentStatus = RTBEngine::Online::ToString(eventData.currentStatus);
        entry.localUserId = eventData.localUserId.ToString();

        loginEvents.push_back(std::move(entry));
        if (loginEvents.size() > MaxLoginEvents) {
            loginEvents.erase(loginEvents.begin());
        }
    }

    void OnlinePanel::AddLobbyEvent(const RTBEngine::Online::OnlineLobbyStatusChangedEvent& eventData)
    {
        LobbyEventEntry entry;
        entry.previousState = RTBEngine::Online::ToString(eventData.previousState);
        entry.currentState = RTBEngine::Online::ToString(eventData.currentState);
        entry.lobbyId = eventData.lobby.lobbyId.empty() ? "None" : eventData.lobby.lobbyId;

        lobbyEvents.push_back(std::move(entry));
        if (lobbyEvents.size() > MaxLobbyEvents) {
            lobbyEvents.erase(lobbyEvents.begin());
        }
    }

    void OnlinePanel::StoreIdentityActionResult(const RTBEngine::Online::OnlineResult& result)
    {
        lastIdentityActionSucceeded = result.success;

        if (result.success) {
            lastIdentityActionMessage = result.message.empty() ? "Success." : result.message;
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
            lastLobbyActionMessage = result.message.empty() ? "Success." : result.message;
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
        onlineBackendIndex = settings.backend == RTBEngine::Online::OnlineBackendType::EOS ? 1 : 0;
        CopyToBuffer(eosProductId, settings.productId);
        CopyToBuffer(eosSandboxId, settings.sandboxId);
        CopyToBuffer(eosDeploymentId, settings.deploymentId);
        CopyToBuffer(eosClientId, settings.clientId);
        CopyToBuffer(eosClientSecret, settings.clientSecret);
    }

    EditorOnlineSettings OnlinePanel::BuildSettingsFromFields() const
    {
        EditorOnlineSettings settings;
        settings.enabled = onlineSettingsEnabled;
        settings.backend = onlineBackendIndex == 1
            ? RTBEngine::Online::OnlineBackendType::EOS
            : RTBEngine::Online::OnlineBackendType::Null;
        settings.productId = ReadBuffer(eosProductId);
        settings.sandboxId = ReadBuffer(eosSandboxId);
        settings.deploymentId = ReadBuffer(eosDeploymentId);
        settings.clientId = ReadBuffer(eosClientId);
        settings.clientSecret = ReadBuffer(eosClientSecret);
        return settings;
    }

    void OnlinePanel::DrawSettingsSection()
    {
        ImGui::TextUnformatted("Settings");

        const std::string settingsPath = EditorOnlineSettingsStore::GetSettingsPath().string();
        ImGui::TextWrapped("Stored in: %s", settingsPath.c_str());

        ImGui::Checkbox("Online Enabled", &onlineSettingsEnabled);

        const char* backendItems[] = { "Null", "EOS" };
        ImGui::SetNextItemWidth(160.0f);
        ImGui::Combo("Backend", &onlineBackendIndex, backendItems, IM_ARRAYSIZE(backendItems));

        const bool usingEos = onlineBackendIndex == 1;
        BeginDisabledIf(!usingEos);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Product Id", eosProductId, sizeof(eosProductId));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Sandbox Id", eosSandboxId, sizeof(eosSandboxId));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Deployment Id", eosDeploymentId, sizeof(eosDeploymentId));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Client Id", eosClientId, sizeof(eosClientId));
        ImGui::Checkbox("Show Client Secret", &showClientSecret);
        ImGui::SetNextItemWidth(-1.0f);
        const ImGuiInputTextFlags secretFlags = showClientSecret ? 0 : ImGuiInputTextFlags_Password;
        ImGui::InputText("Client Secret", eosClientSecret, sizeof(eosClientSecret), secretFlags);
        EndDisabledIf(!usingEos);

        const EditorOnlineSettings currentSettings = BuildSettingsFromFields();
        if (usingEos && !currentSettings.HasCompleteEosConfig()) {
            ImGui::TextDisabled("EOS needs all credential fields before it can be applied.");
        }

        if (ImGui::Button("Reload Settings")) {
            LoadSettingsIntoFields();
            lastSettingsSucceeded = true;
            lastSettingsMessage = "Settings reloaded.";
        }

        ImGui::SameLine();

        if (ImGui::Button("Save Settings")) {
            lastSettingsSucceeded = EditorOnlineSettingsStore::Save(currentSettings);
            lastSettingsMessage = lastSettingsSucceeded
                ? "Settings saved."
                : "Failed to save settings.";
        }

        ImGui::SameLine();

        const bool applyBlocked = usingEos && !currentSettings.HasCompleteEosConfig();
        BeginDisabledIf(applyBlocked);
        if (ImGui::Button("Save And Apply")) {
            lastSettingsSucceeded = EditorOnlineSettingsStore::Save(currentSettings);
            if (!lastSettingsSucceeded) {
                lastSettingsMessage = "Failed to save settings.";
            } else {
                RTBEngine::Online::OnlineConfig config;
                EditorOnlineSettingsStore::ApplyToOnlineConfig(currentSettings, config);
                lastSettingsSucceeded = RTBEngine::Online::OnlineSystem::GetInstance().Initialize(config);
                lastSettingsMessage = lastSettingsSucceeded
                    ? "Settings saved and applied."
                    : "Settings saved, but online initialization failed.";
                loginEvents.clear();
                lobbyEvents.clear();
                loginStatusSubscription.Reset();
                lobbyStatusSubscription.Reset();
                subscribedIdentity = nullptr;
                subscribedLobby = nullptr;
            }
        }
        EndDisabledIf(applyBlocked);

        if (!lastSettingsMessage.empty()) {
            const ImVec4 color = lastSettingsSucceeded
                ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
            ImGui::TextColored(color, "%s", lastSettingsMessage.c_str());
        }
    }

    void OnlinePanel::DrawSystemSection(const RTBEngine::Online::OnlineSystem& onlineSystem)
    {
        ImGui::TextUnformatted("System");

        if (ImGui::BeginTable("##OnlineSystemTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
            DrawTextRow("Enabled", onlineSystem.IsEnabled() ? "true" : "false");
            DrawTextRow("Initialized", onlineSystem.IsInitialized() ? "true" : "false");
            DrawTextRow("State", RTBEngine::Online::ToString(onlineSystem.GetState()));
            DrawTextRow("Backend", RTBEngine::Online::ToString(onlineSystem.GetBackendType()));
            DrawStringRow("Last Error", onlineSystem.GetLastError());
            ImGui::EndTable();
        }
    }

    void OnlinePanel::DrawIdentitySection(RTBEngine::Online::IOnlineIdentity* identity)
    {
        ImGui::TextUnformatted("Identity");

        if (!identity) {
            ImGui::TextDisabled("Identity is not available for the current online state.");
            return;
        }

        const RTBEngine::Online::OnlineLoginStatus status = identity->GetLoginStatus();
        const std::string localUserId = identity->GetLocalUserId().ToString();

        if (ImGui::BeginTable("##OnlineIdentityTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
            DrawTextRow("Login Status", RTBEngine::Online::ToString(status));
            DrawStringRow("Local User Id", localUserId);
            DrawStringRow("Display Name", identity->GetDisplayName());
            DrawTextRow("Last Error", identity->GetLastError());
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputText("Display Name", displayName, sizeof(displayName));

        const bool loginBlocked =
            status == RTBEngine::Online::OnlineLoginStatus::LoggingIn ||
            status == RTBEngine::Online::OnlineLoginStatus::LoggedIn;
        const bool logoutBlocked =
            status == RTBEngine::Online::OnlineLoginStatus::NotLoggedIn ||
            status == RTBEngine::Online::OnlineLoginStatus::LoggingIn;

        BeginDisabledIf(loginBlocked);
        if (ImGui::Button("Login Device ID")) {
            RTBEngine::Online::OnlineLoginOptions options;
            options.type = RTBEngine::Online::OnlineLoginType::DeviceId;
            options.displayName = displayName;
            StoreIdentityActionResult(identity->Login(options));
        }
        EndDisabledIf(loginBlocked);

        ImGui::SameLine();

        BeginDisabledIf(logoutBlocked);
        if (ImGui::Button("Logout")) {
            identity->Logout();
            lastIdentityActionSucceeded = true;
            lastIdentityActionMessage = "Logout requested.";
        }
        EndDisabledIf(logoutBlocked);

        if (!lastIdentityActionMessage.empty()) {
            const ImVec4 color = lastIdentityActionSucceeded
                ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
            ImGui::TextColored(color, "Last action: %s", lastIdentityActionMessage.c_str());
        }
    }

    void OnlinePanel::DrawLobbySection(RTBEngine::Online::IOnlineLobby* lobby,
                                       const RTBEngine::Online::IOnlineIdentity* identity)
    {
        ImGui::TextUnformatted("Lobby");

        if (!lobby) {
            ImGui::TextDisabled("Lobby is not available for the current online state.");
            return;
        }

        const RTBEngine::Online::OnlineLobbyState state = lobby->GetState();
        const RTBEngine::Online::OnlineLobbyInfo& currentLobby = lobby->GetCurrentLobby();

        if (ImGui::BeginTable("##OnlineLobbyTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
            DrawTextRow("State", RTBEngine::Online::ToString(state));
            DrawStringRow("Lobby Id", currentLobby.lobbyId);
            DrawStringRow("Owner User Id", currentLobby.ownerUserId.ToString());
            DrawTextRow("Is Owner", currentLobby.isOwner ? "true" : "false");
            DrawStringRow("Max Members", std::to_string(currentLobby.maxMembers));
            DrawStringRow("Available Slots", std::to_string(currentLobby.availableSlots));
            DrawTextRow("Last Error", lobby->GetLastError());
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputText("Bucket Id", lobbyBucketId, sizeof(lobbyBucketId));
        ImGui::SetNextItemWidth(120.0f);
        ImGui::InputInt("Max Members", &lobbyMaxMembers);
        lobbyMaxMembers = std::clamp(lobbyMaxMembers, 2, 6);

        ImGui::Checkbox("Public Advertised", &lobbyPublicAdvertised);
        ImGui::SameLine();
        ImGui::Checkbox("Allow Invites", &lobbyAllowInvites);
        ImGui::Checkbox("Allow Join By Id", &lobbyAllowJoinById);
        ImGui::SameLine();
        ImGui::Checkbox("Allow Host Migration", &lobbyAllowHostMigration);

        const bool loggedIn = identity && identity->IsLoggedIn();
        const bool hasLobby = !currentLobby.lobbyId.empty();
        const bool hasTargetLobby = lobbyTargetId[0] != '\0';
        const bool operationInProgress =
            state == RTBEngine::Online::OnlineLobbyState::Creating ||
            state == RTBEngine::Online::OnlineLobbyState::Searching ||
            state == RTBEngine::Online::OnlineLobbyState::Joining ||
            state == RTBEngine::Online::OnlineLobbyState::Leaving ||
            state == RTBEngine::Online::OnlineLobbyState::Destroying;
        const bool createBlocked =
            !loggedIn ||
            operationInProgress ||
            hasLobby;
        const bool findBlocked =
            !loggedIn ||
            operationInProgress ||
            hasLobby ||
            !hasTargetLobby;
        const bool joinBlocked =
            !loggedIn ||
            operationInProgress ||
            hasLobby ||
            !hasTargetLobby;
        const bool leaveBlocked =
            !loggedIn ||
            operationInProgress ||
            !hasLobby;
        const bool destroyBlocked =
            !loggedIn ||
            operationInProgress ||
            !hasLobby ||
            !currentLobby.isOwner;

        BeginDisabledIf(createBlocked);
        if (ImGui::Button("Create Lobby")) {
            RTBEngine::Online::OnlineCreateLobbyOptions options;
            options.maxMembers = static_cast<std::uint32_t>(lobbyMaxMembers);
            options.bucketId = lobbyBucketId;
            options.publicAdvertised = lobbyPublicAdvertised;
            options.allowInvites = lobbyAllowInvites;
            options.allowJoinById = lobbyAllowJoinById;
            options.allowHostMigration = lobbyAllowHostMigration;
            StoreLobbyActionResult(lobby->CreateLobby(options));
        }
        EndDisabledIf(createBlocked);

        ImGui::SameLine();

        BeginDisabledIf(destroyBlocked);
        if (ImGui::Button("Destroy Lobby")) {
            StoreLobbyActionResult(lobby->DestroyLobby());
        }
        EndDisabledIf(destroyBlocked);

        ImGui::Spacing();
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Lobby Id", lobbyTargetId, sizeof(lobbyTargetId));

        BeginDisabledIf(findBlocked);
        if (ImGui::Button("Find Lobby")) {
            RTBEngine::Online::OnlineFindLobbiesOptions options;
            options.lobbyId = lobbyTargetId;
            options.maxResults = 10;
            StoreLobbyActionResult(lobby->FindLobbies(options));
        }
        EndDisabledIf(findBlocked);

        ImGui::SameLine();

        BeginDisabledIf(joinBlocked);
        if (ImGui::Button("Join Lobby")) {
            RTBEngine::Online::OnlineJoinLobbyOptions options;
            options.lobbyId = lobbyTargetId;
            StoreLobbyActionResult(lobby->JoinLobby(options));
        }
        EndDisabledIf(joinBlocked);

        ImGui::SameLine();

        BeginDisabledIf(leaveBlocked);
        if (ImGui::Button("Leave Lobby")) {
            StoreLobbyActionResult(lobby->LeaveLobby());
        }
        EndDisabledIf(leaveBlocked);

        const std::vector<RTBEngine::Online::OnlineLobbyInfo>& searchResults = lobby->GetSearchResults();
        if (searchResults.empty()) {
            ImGui::TextDisabled("No lobby search results yet.");
        } else if (ImGui::BeginTable("##OnlineLobbySearchResultsTable", 5,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Lobby Id");
            ImGui::TableSetupColumn("Owner");
            ImGui::TableSetupColumn("Max");
            ImGui::TableSetupColumn("Slots");
            ImGui::TableSetupColumn("Owner Local");
            ImGui::TableHeadersRow();

            for (const RTBEngine::Online::OnlineLobbyInfo& result : searchResults) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(result.lobbyId.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(result.ownerUserId.ToString().c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(std::to_string(result.maxMembers).c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(std::to_string(result.availableSlots).c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(result.isOwner ? "true" : "false");
            }

            ImGui::EndTable();
        }

        if (!loggedIn) {
            ImGui::TextDisabled("Login is required before lobby operations.");
        }

        if (!lastLobbyActionMessage.empty()) {
            const ImVec4 color = lastLobbyActionSucceeded
                ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
            ImGui::TextColored(color, "Last lobby action: %s", lastLobbyActionMessage.c_str());
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
        ImGui::InputText("Start Scene", multiplayerStartScene, sizeof(multiplayerStartScene));

        if (ImGui::Button("Prepare & Launch")) {
            MultiplayerTestLauncher::LaunchSettings settings;
            settings.playerCount = multiplayerPlayerCount;
            settings.startScene = multiplayerStartScene;
            multiplayerLauncher.PrepareAndLaunch(settings);
        }

        ImGui::SameLine();

        if (ImGui::Button("Stop All")) {
            multiplayerLauncher.StopAll();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clean Last Run")) {
            multiplayerLauncher.CleanLastRun();
        }

        if (!multiplayerLauncher.GetLastMessage().empty()) {
            const ImVec4 color = multiplayerLauncher.WasLastActionSuccessful()
                ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
            ImGui::TextColored(color, "%s", multiplayerLauncher.GetLastMessage().c_str());
        }

        const std::filesystem::path& runDirectory = multiplayerLauncher.GetLastRunDirectory();
        if (!runDirectory.empty()) {
            ImGui::TextWrapped("Run directory: %s", runDirectory.string().c_str());
        }

        const std::vector<MultiplayerTestLauncher::PlayerInstance>& instances =
            multiplayerLauncher.GetInstances();
        if (instances.empty()) {
            ImGui::TextDisabled("No multiplayer test players launched.");
            return;
        }

        if (ImGui::BeginTable("##MultiplayerTestPlayersTable", 4,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Player");
            ImGui::TableSetupColumn("PID");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Working Directory");
            ImGui::TableHeadersRow();

            for (const MultiplayerTestLauncher::PlayerInstance& instance : instances) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Player %d", instance.playerIndex);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", instance.processId);
                ImGui::TableSetColumnIndex(2);
                if (instance.running) {
                    ImGui::TextUnformatted("Running");
                } else {
                    ImGui::Text("Exited (%lu)", instance.exitCode);
                }
                ImGui::TableSetColumnIndex(3);
                ImGui::TextWrapped("%s", instance.workingDirectory.string().c_str());
            }

            ImGui::EndTable();
        }
    }

    void OnlinePanel::DrawLoginEventsSection()
    {
        ImGui::TextUnformatted("Login Events");

        if (ImGui::Button("Clear Events")) {
            loginEvents.clear();
        }

        if (loginEvents.empty()) {
            ImGui::TextDisabled("No login status events captured yet.");
            return;
        }

        if (ImGui::BeginTable("##OnlineLoginEventsTable", 3,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Previous");
            ImGui::TableSetupColumn("Current");
            ImGui::TableSetupColumn("Local User Id");
            ImGui::TableHeadersRow();

            for (const LoginEventEntry& eventEntry : loginEvents) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(eventEntry.previousStatus.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(eventEntry.currentStatus.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(eventEntry.localUserId.c_str());
            }

            ImGui::EndTable();
        }
    }

    void OnlinePanel::DrawLobbyEventsSection()
    {
        ImGui::TextUnformatted("Lobby Events");

        if (ImGui::Button("Clear Lobby Events")) {
            lobbyEvents.clear();
        }

        if (lobbyEvents.empty()) {
            ImGui::TextDisabled("No lobby status events captured yet.");
            return;
        }

        if (ImGui::BeginTable("##OnlineLobbyEventsTable", 3,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Previous");
            ImGui::TableSetupColumn("Current");
            ImGui::TableSetupColumn("Lobby Id");
            ImGui::TableHeadersRow();

            for (const LobbyEventEntry& eventEntry : lobbyEvents) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(eventEntry.previousState.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(eventEntry.currentState.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(eventEntry.lobbyId.c_str());
            }

            ImGui::EndTable();
        }
    }

}
