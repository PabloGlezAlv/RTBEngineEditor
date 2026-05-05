#include "OnlinePanel.h"

#include <imgui.h>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace {

    constexpr std::size_t MaxLoginEvents = 64;

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

}

namespace RTBEditor {

    OnlinePanel::OnlinePanel() = default;

    OnlinePanel::~OnlinePanel()
    {
        loginStatusSubscription.Reset();
    }

    void OnlinePanel::OnUIRender(EditorContext& /*context*/)
    {
        ImGui::Begin("Online");

        RTBEngine::Online::OnlineSystem& onlineSystem =
            RTBEngine::Online::OnlineSystem::GetInstance();
        RTBEngine::Online::IOnlineIdentity* identity = onlineSystem.GetIdentity();

        // Keep the debug panel connected to the currently active backend identity.
        RefreshIdentitySubscription(identity);

        DrawSystemSection(onlineSystem);
        ImGui::Separator();
        DrawIdentitySection(identity);
        ImGui::Separator();
        DrawLoginEventsSection();

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

    void OnlinePanel::StoreActionResult(const RTBEngine::Online::OnlineResult& result)
    {
        lastActionSucceeded = result.success;

        if (result.success) {
            lastActionMessage = result.message.empty() ? "Success." : result.message;
            return;
        }

        lastActionMessage = std::string(RTBEngine::Online::ToString(result.errorCode));
        if (!result.message.empty()) {
            lastActionMessage += ": ";
            lastActionMessage += result.message;
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
            StoreActionResult(identity->Login(options));
        }
        EndDisabledIf(loginBlocked);

        ImGui::SameLine();

        BeginDisabledIf(logoutBlocked);
        if (ImGui::Button("Logout")) {
            identity->Logout();
            lastActionSucceeded = true;
            lastActionMessage = "Logout requested.";
        }
        EndDisabledIf(logoutBlocked);

        if (!lastActionMessage.empty()) {
            const ImVec4 color = lastActionSucceeded
                ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
            ImGui::TextColored(color, "Last action: %s", lastActionMessage.c_str());
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

}
