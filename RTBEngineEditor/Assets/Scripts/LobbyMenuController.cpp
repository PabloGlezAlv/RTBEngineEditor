#include "LobbyMenuController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Online/OnlineResult.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <RTBEngine/UI/Elements/UIText.h>

#include <algorithm>
#include <cstdint>
#include <cstring>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

using ThisClass = LobbyMenuController;

namespace {

    constexpr std::size_t MaxJoinLobbyIdLength = 128;

    bool IsLobbyOperationInProgress(RTBEngine::Online::OnlineLobbyState state)
    {
        using RTBEngine::Online::OnlineLobbyState;
        return state == OnlineLobbyState::Creating ||
            state == OnlineLobbyState::Searching ||
            state == OnlineLobbyState::Joining ||
            state == OnlineLobbyState::Leaving ||
            state == OnlineLobbyState::Destroying;
    }

    std::string FormatResult(const RTBEngine::Online::OnlineResult& result)
    {
        if (result.success) {
            return result.message.empty() ? "Action started." : result.message;
        }

        std::string message = RTBEngine::Online::ToString(result.errorCode);
        if (!result.message.empty()) {
            message += ": ";
            message += result.message;
        }
        return message;
    }

    bool IsAllowedLobbyIdCharacter(char value)
    {
        return (value >= 'a' && value <= 'z') ||
            (value >= 'A' && value <= 'Z') ||
            (value >= '0' && value <= '9') ||
            value == '-' ||
            value == '_' ||
            value == '.';
    }

    std::string SanitizeLobbyIdText(const std::string& value)
    {
        std::string sanitized;
        sanitized.reserve(std::min(value.size(), MaxJoinLobbyIdLength));

        for (char character : value) {
            if (sanitized.size() >= MaxJoinLobbyIdLength) {
                break;
            }

            if (IsAllowedLobbyIdCharacter(character)) {
                sanitized.push_back(character);
            }
        }

        return sanitized;
    }

    std::string ReadClipboardText()
    {
        if (!OpenClipboard(nullptr)) {
            return {};
        }

        HANDLE clipboardData = GetClipboardData(CF_TEXT);
        if (!clipboardData) {
            CloseClipboard();
            return {};
        }

        const char* text = static_cast<const char*>(GlobalLock(clipboardData));
        std::string result = text ? text : "";
        if (text) {
            GlobalUnlock(clipboardData);
        }

        CloseClipboard();
        return result;
    }

    bool WriteClipboardText(const std::string& value)
    {
        if (value.empty() || !OpenClipboard(nullptr)) {
            return false;
        }

        if (!EmptyClipboard()) {
            CloseClipboard();
            return false;
        }

        const SIZE_T byteCount = value.size() + 1;
        HGLOBAL clipboardData = GlobalAlloc(GMEM_MOVEABLE, byteCount);
        if (!clipboardData) {
            CloseClipboard();
            return false;
        }

        void* destination = GlobalLock(clipboardData);
        if (!destination) {
            GlobalFree(clipboardData);
            CloseClipboard();
            return false;
        }

        std::memcpy(destination, value.c_str(), byteCount);
        GlobalUnlock(clipboardData);

        if (!SetClipboardData(CF_TEXT, clipboardData)) {
            GlobalFree(clipboardData);
            CloseClipboard();
            return false;
        }

        CloseClipboard();
        return true;
    }

    bool AppendIfPressed(
        RTBEngine::Input::InputManager& input,
        RTBEngine::Input::KeyCode key,
        char character,
        std::string& target)
    {
        if (!input.IsKeyJustPressed(key) || target.size() >= MaxJoinLobbyIdLength) {
            return false;
        }

        target.push_back(character);
        return true;
    }

}

RTB_REGISTER_COMPONENT(LobbyMenuController)
    RTB_PROPERTY_COMPONENT(statusText, UIText)
    RTB_PROPERTY_COMPONENT(lobbyIdText, UIText)
    RTB_PROPERTY_COMPONENT(playerCountText, UIText)
    RTB_PROPERTY_COMPONENT(joinHintText, UIText)
    RTB_PROPERTY_COMPONENT(createButton, UIButton)
    RTB_PROPERTY_COMPONENT(joinButton, UIButton)
    RTB_PROPERTY_COMPONENT(copyLobbyIdButton, UIButton)
    RTB_PROPERTY_COMPONENT(finishButton, UIButton)
    RTB_PROPERTY_COMPONENT(startGameButton, UIButton)
    RTB_PROPERTY_ASSET_PATH(gameScenePath, "lua")
    RTB_PROPERTY(lobbyBucketId)
    RTB_PROPERTY(joinLobbyId)
    RTB_PROPERTY_RANGE(maxMembers, 2, 6)
    RTB_PROPERTY(autoLoginOnStart)
RTB_END_REGISTER(LobbyMenuController)

void LobbyMenuController::OnStart()
{
    SetTimeMode(RTBEngine::ECS::ComponentTimeMode::Unscaled);
    SetUpdateTickEnabled(true);
    maxMembers = std::clamp(maxMembers, 2, 6);
    BindButtons();
    TryAutoLogin();
    RefreshView();
}

void LobbyMenuController::OnUpdate(float)
{
    CaptureJoinLobbyIdInput();
    ExecutePendingActionIfReady();
    RefreshView();
}

void LobbyMenuController::OnDestroy()
{
    // Scene teardown can destroy UI elements in any hierarchy order. Do not touch
    // button pointers here; the buttons own and destroy their callbacks safely.
    callbacksBound = false;
    pendingAction = PendingAction::None;
    createButton = nullptr;
    joinButton = nullptr;
    copyLobbyIdButton = nullptr;
    finishButton = nullptr;
    startGameButton = nullptr;
    statusText = nullptr;
    lobbyIdText = nullptr;
    playerCountText = nullptr;
    joinHintText = nullptr;
}

void LobbyMenuController::OnValidate()
{
    maxMembers = std::clamp(maxMembers, 2, 6);
    RefreshView();
}

void LobbyMenuController::BindButtons()
{
    if (callbacksBound) {
        return;
    }

    if (createButton) {
        createButton->SetOnClick([this]() { CreateLobby(); });
    }

    if (joinButton) {
        joinButton->SetOnClick([this]() { JoinLobby(); });
    }

    if (copyLobbyIdButton) {
        copyLobbyIdButton->SetOnClick([this]() { CopyLobbyId(); });
    }

    if (finishButton) {
        finishButton->SetOnClick([this]() { FinishLobby(); });
    }

    if (startGameButton) {
        startGameButton->SetOnClick([this]() { StartGame(); });
    }

    callbacksBound = true;
}

void LobbyMenuController::RefreshView()
{
    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    RTBEngine::Online::IOnlineIdentity* identity = online.GetIdentity();
    RTBEngine::Online::IOnlineLobby* lobby = online.GetLobby();

    std::string status = lastActionMessage.empty() ? "Choose how to enter the lobby." : lastActionMessage;
    status += "\nOnline: ";
    status += RTBEngine::Online::ToString(online.GetState());

    if (identity) {
        status += " | Login: ";
        status += RTBEngine::Online::ToString(identity->GetLoginStatus());
    } else {
        status += " | Login: unavailable";
    }

    if (lobby) {
        status += " | Lobby: ";
        status += RTBEngine::Online::ToString(lobby->GetState());
    } else {
        status += " | Lobby: unavailable";
    }

    if (statusText) {
        statusText->SetText(status);
    }

    if (lobbyIdText) {
        const RTBEngine::Online::OnlineLobbyInfo* currentLobby =
            lobby ? &lobby->GetCurrentLobby() : nullptr;
        if (currentLobby && !currentLobby->lobbyId.empty()) {
            lobbyIdText->SetText("Lobby ID: " + currentLobby->lobbyId);
        } else {
            lobbyIdText->SetText("Lobby ID: None");
        }
    }

    if (playerCountText) {
        const RTBEngine::Online::OnlineLobbyInfo* currentLobby =
            lobby ? &lobby->GetCurrentLobby() : nullptr;
        if (currentLobby && !currentLobby->lobbyId.empty()) {
            const std::uint32_t maxCount = currentLobby->maxMembers;
            const std::uint32_t currentCount = currentLobby->currentMembers > 0
                ? currentLobby->currentMembers
                : (maxCount >= currentLobby->availableSlots ? maxCount - currentLobby->availableSlots : 0);
            playerCountText->SetText("Players: " + std::to_string(currentCount) + " / " + std::to_string(maxCount));
        } else {
            playerCountText->SetText("Players: 0 / " + std::to_string(std::clamp(maxMembers, 2, 6)));
        }
    }

    if (joinHintText) {
        if (joinLobbyId.empty()) {
            joinHintText->SetText("Type or paste a Lobby ID, then press Join Lobby.");
        } else {
            joinHintText->SetText("Join target: " + joinLobbyId + "  |  Backspace edits, Delete clears.");
        }
    }

    RefreshButtonState();
}

void LobbyMenuController::RefreshButtonState()
{
    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    RTBEngine::Online::IOnlineIdentity* identity = online.GetIdentity();
    RTBEngine::Online::IOnlineLobby* lobby = online.GetLobby();

    const bool onlineReady = online.IsInitialized() && identity && lobby;
    const bool loggedIn = identity && identity->IsLoggedIn();
    const bool loginInProgress = identity &&
        identity->GetLoginStatus() == RTBEngine::Online::OnlineLoginStatus::LoggingIn;
    const RTBEngine::Online::OnlineLobbyState lobbyState = lobby
        ? lobby->GetState()
        : RTBEngine::Online::OnlineLobbyState::Error;
    const bool lobbyBusy = lobby && IsLobbyOperationInProgress(lobbyState);
    const bool hasLobby = lobby && !lobby->GetCurrentLobby().lobbyId.empty();

    if (createButton) {
        createButton->SetInteractable(onlineReady && !loginInProgress && !lobbyBusy && !hasLobby);
    }

    if (joinButton) {
        joinButton->SetInteractable(onlineReady && !loginInProgress && !lobbyBusy && !hasLobby && !joinLobbyId.empty());
    }

    if (copyLobbyIdButton) {
        copyLobbyIdButton->SetInteractable(onlineReady && hasLobby);
    }

    if (finishButton) {
        finishButton->SetInteractable(onlineReady && loggedIn && !lobbyBusy && hasLobby);
    }

    if (startGameButton) {
        startGameButton->SetInteractable(!gameScenePath.empty() && hasLobby && !lobbyBusy);
    }
}

void LobbyMenuController::TryAutoLogin()
{
    if (!autoLoginOnStart) {
        return;
    }

    RTBEngine::Online::IOnlineIdentity* identity =
        RTBEngine::Online::OnlineSystem::GetInstance().GetIdentity();
    if (!identity || identity->GetLoginStatus() != RTBEngine::Online::OnlineLoginStatus::NotLoggedIn) {
        return;
    }

    RTBEngine::Online::OnlineLoginOptions options;
    options.type = RTBEngine::Online::OnlineLoginType::DeviceId;
    options.displayName = "LobbyPlayer";

    const RTBEngine::Online::OnlineResult result = identity->Login(options);
    SetStatus(FormatResult(result));
}

bool LobbyMenuController::EnsureOnlineReady()
{
    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    if (!online.IsInitialized() || !online.GetIdentity() || !online.GetLobby()) {
        SetStatus("Online is not initialized. Enable Online in the editor settings first.");
        return false;
    }

    return true;
}

bool LobbyMenuController::EnsureLoggedInOrQueue(PendingAction action)
{
    if (!EnsureOnlineReady()) {
        return false;
    }

    RTBEngine::Online::IOnlineIdentity* identity =
        RTBEngine::Online::OnlineSystem::GetInstance().GetIdentity();
    if (!identity) {
        return false;
    }

    if (identity->IsLoggedIn()) {
        return true;
    }

    pendingAction = action;

    if (identity->GetLoginStatus() == RTBEngine::Online::OnlineLoginStatus::LoggingIn) {
        SetStatus("Login in progress. The lobby action will continue after login.");
        return false;
    }

    RTBEngine::Online::OnlineLoginOptions options;
    options.type = RTBEngine::Online::OnlineLoginType::DeviceId;
    options.displayName = "LobbyPlayer";

    const RTBEngine::Online::OnlineResult result = identity->Login(options);
    SetStatus(FormatResult(result));
    return false;
}

void LobbyMenuController::ExecutePendingActionIfReady()
{
    if (pendingAction == PendingAction::None) {
        return;
    }

    RTBEngine::Online::IOnlineIdentity* identity =
        RTBEngine::Online::OnlineSystem::GetInstance().GetIdentity();
    if (!identity || !identity->IsLoggedIn()) {
        return;
    }

    const PendingAction action = pendingAction;
    pendingAction = PendingAction::None;

    if (action == PendingAction::CreateLobby) {
        CreateLobby();
    } else if (action == PendingAction::JoinLobby) {
        JoinLobby();
    }
}

void LobbyMenuController::CaptureJoinLobbyIdInput()
{
    RTBEngine::Online::IOnlineLobby* lobby =
        RTBEngine::Online::OnlineSystem::GetInstance().GetLobby();
    if (lobby && !lobby->GetCurrentLobby().lobbyId.empty()) {
        return;
    }

    RTBEngine::Input::InputManager& input = RTBEngine::Input::InputManager::GetInstance();
    using RTBEngine::Input::KeyCode;

    const bool controlPressed =
        input.IsKeyPressed(KeyCode::Control) ||
        input.IsKeyPressed(KeyCode::LeftControl) ||
        input.IsKeyPressed(KeyCode::RightControl);

    if (controlPressed && input.IsKeyJustPressed(KeyCode::V)) {
        const std::string pastedValue = SanitizeLobbyIdText(ReadClipboardText());
        if (!pastedValue.empty()) {
            joinLobbyId = pastedValue;
            SetStatus("Lobby ID pasted. Press Join Lobby to continue.");
        }
        return;
    }

    if (input.IsKeyJustPressed(KeyCode::Backspace)) {
        if (!joinLobbyId.empty()) {
            joinLobbyId.pop_back();
        }
        return;
    }

    if (input.IsKeyJustPressed(KeyCode::Delete)) {
        joinLobbyId.clear();
        return;
    }

    if (controlPressed) {
        return;
    }

    bool changed = false;
    changed |= AppendIfPressed(input, KeyCode::A, 'a', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::B, 'b', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::C, 'c', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::D, 'd', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::E, 'e', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::F, 'f', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::G, 'g', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::H, 'h', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::I, 'i', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::J, 'j', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::K, 'k', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::L, 'l', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::M, 'm', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::N, 'n', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::O, 'o', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::P, 'p', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Q, 'q', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::R, 'r', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::S, 's', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::T, 't', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::U, 'u', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::V, 'v', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::W, 'w', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::X, 'x', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Y, 'y', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Z, 'z', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num0, '0', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num1, '1', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num2, '2', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num3, '3', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num4, '4', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num5, '5', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num6, '6', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num7, '7', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num8, '8', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Num9, '9', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad0, '0', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad1, '1', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad2, '2', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad3, '3', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad4, '4', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad5, '5', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad6, '6', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad7, '7', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad8, '8', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Numpad9, '9', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Minus, '-', joinLobbyId);
    changed |= AppendIfPressed(input, KeyCode::Period, '.', joinLobbyId);

    if (changed) {
        joinLobbyId = SanitizeLobbyIdText(joinLobbyId);
    }
}

void LobbyMenuController::CreateLobby()
{
    if (!EnsureLoggedInOrQueue(PendingAction::CreateLobby)) {
        return;
    }

    RTBEngine::Online::IOnlineLobby* lobby =
        RTBEngine::Online::OnlineSystem::GetInstance().GetLobby();
    if (!lobby) {
        SetStatus("Lobby interface is not available.");
        return;
    }

    RTBEngine::Online::OnlineCreateLobbyOptions options;
    options.maxMembers = static_cast<std::uint32_t>(std::clamp(maxMembers, 2, 6));
    options.bucketId = lobbyBucketId.empty() ? "RTBEngine" : lobbyBucketId;
    options.publicAdvertised = true;
    options.allowInvites = true;
    options.allowJoinById = true;

    const RTBEngine::Online::OnlineResult result = lobby->CreateLobby(options);
    SetStatus(FormatResult(result));
}

void LobbyMenuController::JoinLobby()
{
    if (joinLobbyId.empty()) {
        SetStatus("Join needs a Lobby ID. Runtime text input is the next UI step.");
        return;
    }

    if (!EnsureLoggedInOrQueue(PendingAction::JoinLobby)) {
        return;
    }

    RTBEngine::Online::IOnlineLobby* lobby =
        RTBEngine::Online::OnlineSystem::GetInstance().GetLobby();
    if (!lobby) {
        SetStatus("Lobby interface is not available.");
        return;
    }

    RTBEngine::Online::OnlineJoinLobbyOptions options;
    options.lobbyId = joinLobbyId;

    const RTBEngine::Online::OnlineResult result = lobby->JoinLobby(options);
    SetStatus(FormatResult(result));
}

void LobbyMenuController::CopyLobbyId()
{
    RTBEngine::Online::IOnlineLobby* lobby =
        RTBEngine::Online::OnlineSystem::GetInstance().GetLobby();
    if (!lobby || lobby->GetCurrentLobby().lobbyId.empty()) {
        SetStatus("There is no active Lobby ID to copy.");
        return;
    }

    const std::string lobbyId = lobby->GetCurrentLobby().lobbyId;
    if (!WriteClipboardText(lobbyId)) {
        SetStatus("Failed to copy Lobby ID to clipboard.");
        return;
    }

    SetStatus("Lobby ID copied to clipboard: " + lobbyId);
}

void LobbyMenuController::FinishLobby()
{
    if (!EnsureOnlineReady()) {
        return;
    }

    RTBEngine::Online::IOnlineLobby* lobby =
        RTBEngine::Online::OnlineSystem::GetInstance().GetLobby();
    if (!lobby || lobby->GetCurrentLobby().lobbyId.empty()) {
        SetStatus("There is no active lobby to finish.");
        return;
    }

    const RTBEngine::Online::OnlineResult result = lobby->GetCurrentLobby().isOwner
        ? lobby->DestroyLobby()
        : lobby->LeaveLobby();
    SetStatus(FormatResult(result));
}

void LobbyMenuController::StartGame()
{
    if (gameScenePath.empty()) {
        SetStatus("Game scene path is empty.");
        return;
    }

    RTBEngine::Core::Time::SetPaused(false);
    RTBEngine::ECS::SceneManager::GetInstance().RequestSceneLoad(gameScenePath.c_str());
}

void LobbyMenuController::SetStatus(const std::string& message)
{
    lastActionMessage = message;
    if (statusText) {
        statusText->SetText(message);
    }

    if (!message.empty()) {
        RTB_INFO("LobbyMenu: " + message);
    }
}
