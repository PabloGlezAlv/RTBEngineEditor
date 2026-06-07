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
            lastIdentityActionMessage = result.message.empty() ? "Operacion correcta." : result.message;
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
            lastLobbyActionMessage = result.message.empty() ? "Operacion correcta." : result.message;
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

    void OnlinePanel::DrawSettingsSection()
    {
        ImGui::TextUnformatted("Configuracion Online");
        ImGui::Checkbox("Online Enabled", &onlineSettingsEnabled);

        ImGui::InputInt("LAN Game Port", &lanGamePort);
        ImGui::InputInt("LAN Discovery Port", &lanDiscoveryPort);

        ImGui::SetNextItemWidth(230.0f);
        ImGui::InputText("Login Display Name", loginDisplayName, sizeof(loginDisplayName));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Host remoto por defecto (IP o DNS)", defaultHostAddress, sizeof(defaultHostAddress));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Relay matchmaking URL", relayMatchmakingUrl, sizeof(relayMatchmakingUrl));
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("Escena por defecto", defaultStartScene, sizeof(defaultStartScene));
        ImGui::TextDisabled("Usada por Multiplayer Test y como escena inicial al lanzar jugadores.");

        const EditorOnlineSettings currentSettings = BuildSettingsFromFields();
        if (ImGui::Button("Reload")) {
            LoadSettingsIntoFields();
            lastSettingsSucceeded = true;
            lastSettingsMessage = "Configuracion recargada.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            lastSettingsSucceeded = EditorOnlineSettingsStore::Save(currentSettings);
            lastSettingsMessage = lastSettingsSucceeded ? "Configuracion guardada." : "No se pudo guardar.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Save And Apply")) {
            lastSettingsSucceeded = EditorOnlineSettingsStore::Save(currentSettings);
            if (lastSettingsSucceeded) {
                RTBEngine::Online::OnlineConfig config;
                EditorOnlineSettingsStore::ApplyToOnlineConfig(currentSettings, config);
                lastSettingsSucceeded = RTBEngine::Online::OnlineSystem::GetInstance().Initialize(config);
            }
            lastSettingsMessage = lastSettingsSucceeded
                ? "Configuracion aplicada."
                : "No se pudo aplicar configuracion online.";
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
        RTBEngine::Online::IOnlineLobby* lobby = onlineSystem.GetLobby();

        ImGui::Text("Estado: %s | Backend: %s",
            RTBEngine::Online::ToString(onlineSystem.GetState()),
            RTBEngine::Online::ToString(onlineSystem.GetBackendType()));
        if (!onlineSystem.GetLastError().empty()) {
            ImGui::TextColored(ImVec4(0.95f, 0.45f, 0.35f, 1.0f), "%s", onlineSystem.GetLastError().c_str());
        }

        if (!identity || !lobby) {
            ImGui::TextDisabled("Inicializa el sistema online para continuar.");
            return;
        }

        const bool loggedIn = identity->IsLoggedIn();
        const bool hasLobby = !lobby->GetCurrentLobby().lobbyId.empty();

        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputText("Nombre de sesion", sessionDisplayName, sizeof(sessionDisplayName));
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
            lastIdentityActionMessage = "Logout correcto.";
        }
        EndDisabledIf(!loggedIn);

        ImGui::Text("UserId: %s", identity->GetLocalUserId().ToString().c_str());
        ImGui::Text("Lobby actual: %s", hasLobby ? lobby->GetCurrentLobby().lobbyId.c_str() : "ninguno");

        ImGui::Separator();
        ImGui::SetNextItemWidth(130.0f);
        ImGui::InputInt("Max jugadores", &lobbyMaxMembers);
        lobbyMaxMembers = std::clamp(lobbyMaxMembers, 2, 6);
        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputText("Codigo lobby", lobbyCode, sizeof(lobbyCode));
        ImGui::SetNextItemWidth(320.0f);
        ImGui::InputText("Host remoto (IP/DNS, opcional)", joinHostAddress, sizeof(joinHostAddress));

        const bool canCreate = loggedIn && !hasLobby;
        const bool canSearchJoin = loggedIn && !hasLobby && lobbyCode[0] != '\0';
        const bool canLeave = loggedIn && hasLobby;

        BeginDisabledIf(!canCreate);
        if (ImGui::Button("Crear Lobby")) {
            RTBEngine::Online::OnlineCreateLobbyOptions options;
            options.maxMembers = static_cast<std::uint32_t>(lobbyMaxMembers);
            StoreLobbyActionResult(lobby->CreateLobby(options));
        }
        EndDisabledIf(!canCreate);
        ImGui::SameLine();
        BeginDisabledIf(!canSearchJoin);
        if (ImGui::Button("Buscar")) {
            RTBEngine::Online::OnlineFindLobbiesOptions options;
            options.lobbyId = ReadBuffer(lobbyCode);
            options.hostAddress = ReadBuffer(joinHostAddress);
            StoreLobbyActionResult(lobby->FindLobbies(options));
        }
        EndDisabledIf(!canSearchJoin);
        ImGui::SameLine();
        BeginDisabledIf(!canSearchJoin);
        if (ImGui::Button("Unirse")) {
            RTBEngine::Online::OnlineJoinLobbyOptions options;
            options.lobbyId = ReadBuffer(lobbyCode);
            options.hostAddress = ReadBuffer(joinHostAddress);
            StoreLobbyActionResult(lobby->JoinLobby(options));
        }
        EndDisabledIf(!canSearchJoin);
        ImGui::SameLine();
        BeginDisabledIf(!canLeave);
        if (ImGui::Button("Salir Lobby")) {
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
