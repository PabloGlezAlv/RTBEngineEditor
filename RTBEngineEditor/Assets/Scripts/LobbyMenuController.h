#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace UI {
        class UIButton;
        class UIText;
    }
}

class LobbyMenuController : public RTBEngine::ECS::Component {
public:
    LobbyMenuController() = default;
    ~LobbyMenuController() override = default;

    RTBEngine::UI::UIText* statusText = nullptr;
    RTBEngine::UI::UIText* lobbyIdText = nullptr;
    RTBEngine::UI::UIText* playerCountText = nullptr;
    RTBEngine::UI::UIText* joinHintText = nullptr;

    RTBEngine::UI::UIButton* createButton = nullptr;
    RTBEngine::UI::UIButton* joinButton = nullptr;
    RTBEngine::UI::UIButton* copyLobbyIdButton = nullptr;
    RTBEngine::UI::UIButton* finishButton = nullptr;
    RTBEngine::UI::UIButton* startGameButton = nullptr;

    std::string gameScenePath = "Assets/Scenes/DefaultScene.lua";
    std::string lobbyBucketId = "RTBEngine";
    std::string joinLobbyId;
    int maxMembers = 6;
    bool autoLoginOnStart = true;

    RTB_COMPONENT(LobbyMenuController)

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;
    void OnValidate() override;

private:
    enum class PendingAction {
        None,
        CreateLobby,
        JoinLobby
    };

    PendingAction pendingAction = PendingAction::None;
    std::string lastActionMessage;
    bool callbacksBound = false;

    void BindButtons();
    void RefreshView();
    void RefreshButtonState();
    void TryAutoLogin();
    bool EnsureOnlineReady();
    bool EnsureLoggedInOrQueue(PendingAction action);
    void ExecutePendingActionIfReady();
    void CaptureJoinLobbyIdInput();
    void CreateLobby();
    void JoinLobby();
    void CopyLobbyId();
    void FinishLobby();
    void StartGame();
    void SetStatus(const std::string& message);
};
